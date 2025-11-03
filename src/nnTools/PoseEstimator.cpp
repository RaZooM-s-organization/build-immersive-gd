#include "PoseEstimator.hpp"

#include "../settings/Settings.hpp"

#include <dml_provider_factory.h> // for DirecrtML

#include <codecvt>
#include <cmath>


// in:  [ 1  3   256  192 ]
// out: [ 1  17  64   48  ]

#define MODEL_IN_WIDTH   192
#define MODEL_IN_HEIGHT  256
#define MODEL_IN_DIM     3
#define MODEL_OUT_WIDTH  48
#define MODEL_OUT_HEIGHT 64
#define MODEL_OUT_DIM    17


static const int64_t s_inputDims[4] = {1, MODEL_IN_DIM, MODEL_IN_HEIGHT, MODEL_IN_WIDTH};
static const int64_t s_outputDims[4] = {1, MODEL_OUT_DIM, MODEL_OUT_HEIGHT, MODEL_OUT_WIDTH};

static const size_t s_inputTensorSize = MODEL_IN_DIM * MODEL_IN_HEIGHT * MODEL_IN_WIDTH;
static const size_t s_outputTensorSize = MODEL_OUT_WIDTH * MODEL_OUT_HEIGHT * MODEL_OUT_DIM;



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

    const bool useDirectML = ModSettings::get().m_gpuSupport.m_enable;
    const int deviceId = ModSettings::get().m_gpuSupport.m_deviceId;
    const int fpsLimit = ModSettings::get().m_poseEstimation.m_fpsLimit;
    const int numOfThreads = ModSettings::get().m_poseEstimation.m_numOfThreads;
    const auto m_modelFilepath = getModelPathWstring();

    // auto providers = Ort::GetAvailableProviders();
    // for (auto& p : providers) std::wcout << p.c_str() << std::endl;

    m_env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "hrnet");

    m_sessionOptions = Ort::SessionOptions();
    m_sessionOptions.SetIntraOpNumThreads(numOfThreads);
    m_sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    m_sessionOptions.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
    m_sessionOptions.EnableMemPattern(); // optimize mem allocation

    if (useDirectML) {
        // todo: check that the onnx runtime lib is in resources
        OrtSessionOptionsAppendExecutionProvider_DML(m_sessionOptions, deviceId);
    }
    
    m_memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    
    m_session = Ort::Session(m_env, m_modelFilepath.c_str(), m_sessionOptions);
    
    m_allocator = Ort::Allocator(m_session, m_memoryInfo);

    // todo: use allocator instead of std::makeshared...


    // pre-allocate memory
    m_inputTensorData = (float*) m_allocator.Alloc(s_inputTensorSize * sizeof(float));
    m_outputTensorData = (float*) m_allocator.Alloc(s_outputTensorSize * sizeof(float));

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
    if (m_inputTensorData) m_allocator.Free(m_inputTensorData);
    if (m_outputTensorData) m_allocator.Free(m_outputTensorData);
    log::info("Estimator cleared");
}


PoseResult PoseEstimator::getPendingPoseResult() {
    m_latestPoseOwnership.lock();
    auto copy = m_latestPose;
    m_latestPoseOwnership.unlock();
    return copy;
}


float PoseEstimator::getFps() const {
    return m_fpsLimiter.getActualRefreshRate();
}


int PoseEstimator::getLastInferenceTimeMs() const {
    return m_lastInferenceTimeMs;
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

    // auto inputTensorData = std::make_unique<float[]>(inputTensorSize);
    // auto outputTensorData = std::make_unique<float[]>(outputTensorSize);

    resizeConvertAndSplitImageAllInOne((RGB_888*)rgb888data, m_inputTensorData, 
        frame.m_height, frame.m_width, MODEL_IN_HEIGHT, MODEL_IN_WIDTH);

    
    auto inputTensor = Ort::Value::CreateTensor<float>(
        m_memoryInfo, m_inputTensorData, s_inputTensorSize, s_inputDims, 4);

    auto outputTensor = Ort::Value::CreateTensor<float>(
        m_memoryInfo, m_outputTensorData, s_outputTensorSize, s_outputDims, 4);
    

    const char* inputName = m_inputName.c_str();
    const char* outputName = m_outputName.c_str();

    
    auto dbgTmrBegin = std::chrono::steady_clock::now();
    
    m_session.Run(Ort::RunOptions{}, &inputName, &inputTensor, 1, &outputName, &outputTensor, 1);
    
    auto dbgTmrEnd = std::chrono::steady_clock::now();
    m_lastInferenceTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(dbgTmrEnd - dbgTmrBegin).count();

    
    std::map<int, CCPoint> points;
    
    std::vector<float> values;

    const float threshold = ModSettings::get().m_poseEstimation.m_threshold;
    
    float* outputDataPtr = m_outputTensorData;
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
        if (maxVal > threshold) {
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



