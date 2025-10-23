#pragma once

#include "VideoTools.hpp"
#include "CameraMan.hpp"


class VideoFrame : public CCSprite {
protected:

    std::shared_ptr<CameraMan> m_cameraMan;
    CCTexture2D* m_ccTexture{};
    uint32_t m_currentFrameId = 0;

public:

    static VideoFrame* create(std::shared_ptr<CameraMan> cameraMan);
    ~VideoFrame();

    bool init(std::shared_ptr<CameraMan> cameraMan);
    void visit() override;

};


