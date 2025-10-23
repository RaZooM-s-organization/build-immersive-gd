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

    Ort::Session m_session{nullptr};
    std::optional<Ort::AllocatedStringPtr> inputNamePtr{};
    std::optional<Ort::AllocatedStringPtr> outputNamePtr{};

    std::vector<int64_t> inputDims;
    std::vector<int64_t> outputDims;


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
