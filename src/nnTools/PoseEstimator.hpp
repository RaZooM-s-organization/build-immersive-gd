#pragma once

#include "NNTools.hpp"

#include "../videoTools/CameraMan.hpp"

#include <onnxruntime_cxx_api.h>


class PoseEstimator {
protected:
    std::shared_ptr<CameraMan> m_cameraMan{};
    PoseResult m_latestPose{};

    std::thread m_secondThread{};
    std::atomic<bool> m_secondThreadRunning{};
    std::mutex m_latestPoseOwnership{};
    uint32_t m_latestFrameId{};

    std::wstring m_modelFilepath;
    std::string m_inputName;
    std::string m_outputName;

    Ort::Env m_env{nullptr};
    Ort::SessionOptions m_sessionOptions{nullptr};
    Ort::Session m_session{nullptr};
    Ort::Allocator m_allocator{nullptr};
    Ort::MemoryInfo m_memoryInfo{nullptr};
    

public:

    PoseEstimator() = delete;
    PoseEstimator(std::shared_ptr<CameraMan> cameraman);
    ~PoseEstimator();

    PoseResult getPendingPoseResult();
    void getFrameSizeInPix(int *pixW, int *pixH);

protected:

    bool processFrame();
    void processVideoStream();

};
