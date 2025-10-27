#pragma once

#include "VideoTools.hpp"
#include "CameraMan.hpp"
#include "../utils/FpsLimiter.hpp"


class VideoFrame : public CCSprite {
protected:

    std::shared_ptr<CameraMan> m_cameraMan;
    CCTexture2D* m_ccTexture{};
    uint32_t m_currentFrameId = 0;
    FpsLimiter m_fpsLimiter;

public:

    static VideoFrame* create(std::shared_ptr<CameraMan> cameraMan);
    ~VideoFrame();

    virtual CCTexture2DPixelFormat getTexturePixelFormat() const;
    virtual std::unique_ptr<uint8_t[]> preprocessImageData(uint8_t* data, int w, int h);

    bool init(std::shared_ptr<CameraMan> cameraMan);
    void visit() override;
    int getFps();

};


class VideoFrameAlpha : public VideoFrame {
public:

    static VideoFrameAlpha* create(std::shared_ptr<CameraMan> cameraMan);

    virtual CCTexture2DPixelFormat getTexturePixelFormat() const override;
    virtual std::unique_ptr<uint8_t[]> preprocessImageData(uint8_t* data, int w, int h) override;

};


