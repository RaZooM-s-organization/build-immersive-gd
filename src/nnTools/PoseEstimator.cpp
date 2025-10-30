#include "PoseEstimator.hpp"

#include "../settings/Settings.hpp"

#include <codecvt>
#include <cmath>

#include <windows.h>

// in:  [ 1  3   256  192 ]
// out: [ 1  17  64   48  ]

#define MODEL_IN_WIDTH   192
#define MODEL_IN_HEIGHT  256
#define MODEL_IN_DIM     3
#define MODEL_OUT_WIDTH  48
#define MODEL_OUT_HEIGHT 64
#define MODEL_OUT_DIM    17

#define THRESHOLD 0.5

// todo: on test setup test with rotated image



typedef std::array<uint8_t, 3> RGB_888;

RGB_888& operator*(RGB_888& arr, float val) {
    arr[0] *= val;
    arr[1] *= val;
    arr[2] *= val;
    return arr;
}

RGB_888& operator+(RGB_888& arr, RGB_888& arr2) {
    arr[0] += arr2[0];
    arr[1] += arr2[1];
    arr[2] += arr2[2];
    return arr;
}

// todo: is datatype correct?
// resize image, change its data type from uchar[0..255] to float[0..1], change shape from HWC to CHW
// (function that literally replaces the dependency from 3 opencv modules)
void resizeConvertAndSplitImageAllInOne(const RGB_888 *src, float *dst, int srcH, int srcW, int dstH, int dstW) {
    float hScaleFactor = (float)srcH / dstH;
    float wScaleFactor = (float)srcW / dstW;

    float* rPtr = dst;
    float* gPtr = dst + dstH * dstW;
    float* bPtr = dst + dstH * dstW * 2;

    for (int i = 0; i < dstH; i++) {
        for (int j = 0; j < dstW; j++) {
            float x = i * hScaleFactor;
            float y = j * wScaleFactor;
            
            int xFloor = floorf(x);
            int yFloor = floorf(y);
            int xCeil = std::min((int)ceilf(x), srcH-1);
            int yCeil = std::min((int)ceilf(y), srcW-1);

            RGB_888 q;

            if (xFloor == xCeil && yFloor == yCeil) {
                q = *(src + xFloor * srcW + yFloor);
            } else if (xFloor == xCeil) {
                RGB_888 q1 = *(src + xFloor * srcW + yFloor);
                RGB_888 q2 = *(src + xFloor * srcW + yCeil);
                q = q1 * (yCeil - y) + q2 * (y - yFloor);
            } else if (yFloor == yCeil) {
                RGB_888 q1 = *(src + xFloor * srcW + yFloor);
                RGB_888 q2 = *(src + xCeil * srcW + yFloor);
                q = (q1 * (xCeil - x)) + (q2 * (x - xFloor));
            } else {
                RGB_888 v1 = *(src + xFloor * srcW + yFloor);
                RGB_888 v2 = *(src + xCeil * srcW + yFloor);
                RGB_888 v3 = *(src + xFloor * srcW + yCeil);
                RGB_888 v4 = *(src + xCeil * srcW + yCeil);
                RGB_888 q1 = v1 * (xCeil - x) + v2 * (x - xFloor);
                RGB_888 q2 = v3 * (xCeil - x) + v4 * (x - xFloor);
                q = q1 * (yCeil - y) + q2 * (y - yFloor);
            }
            
            *(rPtr++) = q[0] / 255.f;
            *(gPtr++) = q[1] / 255.f;
            *(bPtr++) = q[2] / 255.f;
        }
    }
}


inline std::wstring getModelPathWstring() {
    auto path = Mod::get()->getResourcesDir().append("hrnet_coco_w32_256x192.onnx");
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(path.string().c_str());
}


PoseEstimator::PoseEstimator(std::shared_ptr<CameraMan> cameraman) {
    m_cameraMan = cameraman;

    const bool useCuda = ModSettings::get().m_poseEstimation.m_useGPU;
    const int fpsLimit = ModSettings::get().m_poseEstimation.m_fpsLimit;
    const int numOfThreads = ModSettings::get().m_poseEstimation.m_numOfThreads;

    m_modelFilepath = getModelPathWstring();

    m_env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "hrnet");

    m_sessionOptions = Ort::SessionOptions();
    m_sessionOptions.SetIntraOpNumThreads(numOfThreads);
    m_sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    if (useCuda) {
        // todo: check that the onnx runtime lib for cuda is in resources
        OrtCUDAProviderOptions cudaOptions{};
        m_sessionOptions.AppendExecutionProvider_CUDA(cudaOptions);
        // todo: check, maybe use Ort::MemoryInfo::CreateCpu
        m_memoryInfo = Ort::MemoryInfo("Cuda", OrtDeviceAllocator, 0, OrtMemTypeDefault);
    } else {
        m_memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    }
    
    m_session = Ort::Session(m_env, m_modelFilepath.c_str(), m_sessionOptions);
    
    m_allocator = Ort::Allocator(m_session, m_memoryInfo);

    // todo: use allocator instead of std::makeshared...

    // get input/output info
    m_inputName = m_session.GetInputNameAllocated(0, m_allocator).get();
    m_outputName = m_session.GetOutputNameAllocated(0, m_allocator).get();

    m_fpsLimiter.setFpsLimit(fpsLimit);

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
    return copy;
}


int PoseEstimator::getFps() {
    return m_fpsLimiter.getActualRefreshRate();
}


void PoseEstimator::processVideoStream() {
    m_secondThreadRunning = true;
    while (m_secondThreadRunning) {
        if (m_fpsLimiter.goodToGo()) {
            bool success = processFrame();
            if (success) {
                m_fpsLimiter.refresh();
            }
        }
        std::this_thread::yield();
    }
}


void PoseEstimator::getFrameSizeInPix(int *pixW, int *pixH) {
    m_cameraMan->getFrameSizeInPix(pixW, pixH);
}


bool PoseEstimator::processFrame() {

    auto frame = m_cameraMan->getPendingFrame();
    auto rgb888data = frame.m_data.get();


    if (frame.m_id == m_latestFrameId || rgb888data == nullptr) {
        return false;
    }
    m_latestFrameId = frame.m_id;


    int64_t inputDims[4] = {1, MODEL_IN_DIM, MODEL_IN_HEIGHT, MODEL_IN_WIDTH};
    int64_t outputDims[4] = {1, MODEL_OUT_DIM, MODEL_OUT_HEIGHT, MODEL_OUT_WIDTH};

    size_t inputTensorSize = MODEL_IN_DIM * MODEL_IN_HEIGHT * MODEL_IN_WIDTH;
    size_t outputTensorSize = MODEL_OUT_WIDTH * MODEL_OUT_HEIGHT * MODEL_OUT_DIM;
    
    auto inputTensorData = std::make_unique<float[]>(inputTensorSize);
    auto outputTensorData = std::make_unique<float[]>(outputTensorSize);

    // auto inputTensorData = (float*) m_allocator.Alloc(inputTensorSize * sizeof(float));
    // auto outputTensorData = (float*) m_allocator.Alloc(outputTensorSize * sizeof(float));

    resizeConvertAndSplitImageAllInOne((RGB_888*)rgb888data, inputTensorData.get(), 
        frame.m_height, frame.m_width, MODEL_IN_HEIGHT, MODEL_IN_WIDTH);

    
    auto inputTensor = Ort::Value::CreateTensor<float>(
        m_memoryInfo, inputTensorData.get(), inputTensorSize, inputDims, 4);

    auto outputTensor = Ort::Value::CreateTensor<float>(
        m_memoryInfo, outputTensorData.get(), outputTensorSize, outputDims, 4);
    

    const char* inputName = m_inputName.c_str();
    const char* outputName = m_outputName.c_str();
    
    // DbgTimer::start();

    m_session.Run(Ort::RunOptions{}, &inputName, &inputTensor, 1, &outputName, &outputTensor, 1);

    // log::info("d");
    
    // DbgTimer::stop();
    
    
    
    std::map<int, CCPoint> points;
    
    std::vector<float> values;
    
    float* outputDataPtr = outputTensorData.get();
    for (int i = 0; i < MODEL_OUT_DIM; i++) {
        float maxVal = -9999;
        CCPoint maxLoc;
        for (int j = 0; j < MODEL_OUT_HEIGHT; j++) {
            for (int k = 0; k < MODEL_OUT_WIDTH; k++) {
                if (*outputDataPtr > maxVal) {
                    maxVal = *outputDataPtr;
                    maxLoc = ccp(k, j);
                }
                outputDataPtr++;
            }
        }
        if (maxVal > THRESHOLD) {
            points[i] = ccp(
                maxLoc.x * frame.m_width / MODEL_OUT_WIDTH,
                (MODEL_OUT_HEIGHT - maxLoc.y) * frame.m_height / MODEL_OUT_HEIGHT
            );
            values.push_back(maxVal);
        }
    }
    float sum=0;
    for(float v:values) sum+=v;
    // log::info("AVG: {}",sum/values.size());


    // update state
    m_latestPoseOwnership.lock();
    m_latestPose.m_id = frame.m_id;
    m_latestPose.m_points = points;
    m_latestPoseOwnership.unlock();


    return true;
}



