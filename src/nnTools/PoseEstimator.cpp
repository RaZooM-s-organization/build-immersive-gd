#include "PoseEstimator.hpp"



#include <opencv2/dnn/dnn.hpp>
// #include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// #include <opencv2/opencv.hpp> // imshow, window

#include <codecvt>


#define MODEL_IN_WIDTH   192
#define MODEL_IN_HEIGHT  256
#define MODEL_OUT_WIDTH  48
#define MODEL_OUT_HEIGHT 64
#define MODEL_OUT_DIM    17

#define THRESHOLD 0.5


template <typename T>
T vectorProduct(const std::vector<T>& v) {
    T ret = 1;
    for (int i = 0; i < v.size(); i++) {
        ret *= v[i];
    }
    return ret;
    // return accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
}


// std::wstring convertToWString(const char* str) {
//     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//     return converter.from_bytes(str);
// }

// std::wstring getModelPathWstring() {
//     auto path = Mod::get()->getResourcesDir().append("hrnet_coco_w32_256x192.onnx");
//     return convertToWString(path.string().c_str());
// }


PoseEstimator::PoseEstimator(std::shared_ptr<CameraMan> cameraman) {
    m_cameraMan = cameraman;

    // setup the model
    bool useCUDA = false;
    // const wchar_t* modelFilepath = getModelPathWstring().c_str(); // todo: broken
    const wchar_t* modelFilepath = L"C:/Users/79215/Desktop/Gaba_Projects/geode/onnx_test/cpp_test/models/hrnet_coco_w32_256x192.onnx";


    Ort::Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "instanceName.c_str()");
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    if (useCUDA) {
        OrtCUDAProviderOptions cuda_options{};
        sessionOptions.AppendExecutionProvider_CUDA(cuda_options);
    }

    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    
    m_session = Ort::Session(env, modelFilepath, sessionOptions);
    
    Ort::AllocatorWithDefaultOptions allocator;

    // get input info
    // size_t numInputNodes = session.GetInputCount();
    // size_t numOutputNodes = session.GetOutputCount();
    
    inputNamePtr = m_session.GetInputNameAllocated(0, allocator);
    // const char* inputName = inputNamePtr.get();
    
    Ort::TypeInfo inputTypeInfo = m_session.GetInputTypeInfo(0);
    auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
    
    // ONNXTensorElementDataType inputType = inputTensorInfo.GetElementType();
    inputDims = inputTensorInfo.GetShape();
    
    // if (inputDims.at(0) == -1) {
    //     std::cout << "Got dynamic batch size. Setting input batch size to " << batchSize << std::endl;
    //     inputDims.at(0) = batchSize;
    // } else {
    //     batchSize = inputDims.at(0);
    // }
    
    // get output info
    outputNamePtr = m_session.GetOutputNameAllocated(0, allocator);
    // const char* outputName = outputNamePtr.get();
    
    Ort::TypeInfo outputTypeInfo = m_session.GetOutputTypeInfo(0);
    auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();
    
    // ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
    outputDims = outputTensorInfo.GetShape();

    
    // if (outputDims.at(0) == -1) {
    //     std::cout << "Got dynamic batch size. Setting output batch size to " << batchSize << std::endl;
    //     outputDims.at(0) = batchSize;
    // }

    m_secondThread = std::thread(&PoseEstimator::processVideoStream, this);

}

PoseEstimator::~PoseEstimator() {
    if (m_secondThread.joinable()) {
        m_secondThreadRunning = false;
        m_secondThread.join();
    }
    log::info("Estimator cleared");
}

PoseResult PoseEstimator::getPendingPoseResult() {
    m_latestPoseOwnership.lock();
    auto copy = m_latestPose;
    m_latestPoseOwnership.unlock();
    return std::move(copy);
}

void PoseEstimator::processVideoStream() {
    m_secondThreadRunning = true;
    while (m_secondThreadRunning) {
        bool success = processFrame();
        std::this_thread::yield();
    }
}

void PoseEstimator::getFrameSizeInPix(int *pixW, int *pixH) {
    m_cameraMan->getFrameSizeInPix(pixW, pixH);
}


bool PoseEstimator::processFrame() {

    auto frame = m_cameraMan->getPendingFrame();


    if (frame.m_id == m_latestFrameId || frame.m_data.get() == nullptr) {
        return false;
    }
    m_latestFrameId = frame.m_id;

    auto ptr = frame.m_data.get();

    cv::Mat frameImage((int)frame.m_height, (int)frame.m_width, (int)CV_8UC3, (void*)ptr); // no copying
    // cv::Mat imageBGR = cv::imread(imageFilepath, cv::ImreadModes::IMREAD_COLOR);
    // cv::Mat resizedImageBGR, resizedImageRGB, resizedImage, preprocessedImage;
    // cv::resize(imageBGR, resizedImageBGR, cv::Size(inputDims.at(3), inputDims.at(2)), cv::INTER_CUBIC); // todo: select the fastest


    cv::Mat resizedImageRGB;
    cv::Mat resizedImage;
    cv::resize(frameImage, resizedImageRGB, cv::Size(inputDims.at(3), inputDims.at(2)), cv::INTER_CUBIC); // todo: select the fastest


    // cv::cvtColor(resizedImageBGR, resizedImageRGB, cv::COLOR_BGR2RGB);
    resizedImageRGB.convertTo(resizedImage, CV_32F, 1.0 / 255);


    
    cv::Mat channels[3];
    cv::split(resizedImage, channels); // todo: simplify?
    // Normalization per channel
    // Normalization parameters obtained from
    // https://github.com/onnx/models/tree/master/vision/classification/squeezenet
    channels[0] = (channels[0] - 0.485) / 0.229;
    channels[1] = (channels[1] - 0.456) / 0.224;
    channels[2] = (channels[2] - 0.406) / 0.225;
    cv::merge(channels, 3, resizedImage);
    // HWC to CHW
    cv::Mat preprocessedImage;
    cv::dnn::blobFromImage(resizedImage, preprocessedImage);
    
    // size_t inputTensorSize = MODEL_IN_WIDTH * MODEL_IN_HEIGHT;
    size_t inputTensorSize = vectorProduct(inputDims);
    std::vector<float> inputTensorValues(inputTensorSize);
    // Make copies of the same image input.
    int batchSize = 1;
    for (int64_t i = 0; i < batchSize; ++i) {
        std::copy(preprocessedImage.begin<float>(),
                    preprocessedImage.end<float>(),
                    inputTensorValues.begin() + i * inputTensorSize / batchSize);
    }
    
    size_t outputTensorSize = vectorProduct(outputDims);
    // size_t outputTensorSize = MODEL_OUT_WIDTH * MODEL_OUT_HEIGHT * MODEL_OUT_DIM;
    // assert(("Output tensor size should equal to the label set size.",
    //         labels.size() * batchSize == outputTensorSize));
    std::vector<float> outputTensorValues(outputTensorSize);
    
    // std::vector<const char*> inputNames{inputName};
    // std::vector<const char*> outputNames{outputName};
    std::vector<Ort::Value> inputTensors;
    std::vector<Ort::Value> outputTensors;
    
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    
    
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo, inputTensorValues.data(), inputTensorSize, inputDims.data(),
        inputDims.size()));
    outputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo, outputTensorValues.data(), outputTensorSize,
        outputDims.data(), outputDims.size()));
    
    
    const char* inputName = inputNamePtr.value().get();
    const char* outputName = outputNamePtr.value().get();
    m_session.Run(Ort::RunOptions{nullptr}, &inputName,
                inputTensors.data(), 1 /*Number of inputs*/, &outputName,
                outputTensors.data(), 1 /*Number of outputs*/);
    
    
    // outputDataWrapper.reshape(1, std::vector<int>(outputDims.begin(), outputDims.end()));

    
    // in this case output dims are [1,17,64,48]
    auto outputDataPtr = outputTensorValues.data();
    auto outFrameSize = MODEL_OUT_HEIGHT * MODEL_OUT_WIDTH;

    CCPoint results[MODEL_OUT_DIM];

    for (int i = 0; i < MODEL_OUT_DIM; i++) {
        float maxVal = -9999;
        cv::Point maxLoc;
        for (int j = 0; j < MODEL_OUT_HEIGHT; j++) {
            for (int k = 0; k < MODEL_OUT_WIDTH; k++) {
                if (*outputDataPtr > maxVal) {
                    maxVal = *outputDataPtr;
                    maxLoc = {k, j};
                }
                outputDataPtr++;
            }
        }
        // todo: threshold
        // maxLoc * 4 - scale factor
        // cv::circle(resizedImageBGR, maxLoc * 4, 3, {0,255,255}, 3);
        if (maxVal > THRESHOLD) {
            results[i] = ccp(
                maxLoc.x * frame.m_width / MODEL_OUT_WIDTH,
                (MODEL_OUT_HEIGHT - maxLoc.y) * frame.m_height / MODEL_OUT_HEIGHT
            );
            log::info("{}", results[i]);
        }
    }


    // update state
    m_latestPoseOwnership.lock();
    m_latestPose.m_height = frame.m_height;
    m_latestPose.m_width = frame.m_width;
    m_latestPose.m_id = frame.m_id;
    m_latestPose.idk.clear();
    for (int i = 0; i < MODEL_OUT_DIM; i++) {
        m_latestPose.idk[i] = results[i];
    }
    m_latestPoseOwnership.unlock();

    return true;
}



