#include "VideoFrame.hpp"
#include "../settings/Settings.hpp"

// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC

// todo: memory check




VideoFrame* VideoFrame::create(std::shared_ptr<CameraMan> cameraMan) {
    auto ret = new VideoFrame;
    if (ret && ret->init(cameraMan)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


VideoFrame::~VideoFrame() {
    CC_SAFE_RELEASE(m_ccTexture);
    log::info("Frame cleared");
}


float VideoFrame::getFps() {
    return m_fpsLimiter.getActualRefreshRate();
}


CCTexture2DPixelFormat VideoFrame::getTexturePixelFormat() const {
    return kCCTexture2DPixelFormat_RGB888;
}


std::unique_ptr<uint8_t[]> VideoFrame::preprocessImageData(uint8_t* data, int w, int h) {
    return nullptr; // already desired format
}


bool VideoFrame::init(std::shared_ptr<CameraMan> cameraMan) {

    m_cameraMan = cameraMan;

    int outWidth, outHeight;
    m_cameraMan->getFrameSizeInPix(&outWidth, &outHeight);

    m_ccTexture = new CCTexture2D();
    m_ccTexture->initWithData(
        nullptr,
        getTexturePixelFormat(),
        outWidth, outHeight, CCSizeMake(outWidth, outHeight)
    );
    m_ccTexture->autorelease();
    m_ccTexture->retain();
    
    if (!CCSprite::initWithTexture(m_ccTexture)) return false;

    float sizeMultiplier = 1;
    switch (CCDirector::get()->getLoadedTextureQuality()) {
        case TextureQuality::kTextureQualityLow: sizeMultiplier = 1; break;
        case TextureQuality::kTextureQualityMedium: sizeMultiplier = 0.5; break;
        case TextureQuality::kTextureQualityHigh: sizeMultiplier = 0.25; break;
    }
    setContentSize(CCSizeMake(outWidth, outHeight) * sizeMultiplier);

    m_fpsLimiter.setFpsLimit(ModSettings::get().m_videoOutput.m_fpsLimit);

    setFlipX(ModSettings::get().m_videoOutput.m_mirror);

    return true;
}


void VideoFrame::visit() {

    if (m_fpsLimiter.goodToGo()) {
        auto frame = m_cameraMan->getPendingFrame();
    
        if (frame.m_id != m_currentFrameId && frame.m_data.get() != nullptr) {
    
            if (auto glid = m_ccTexture->getName()) {
                ccGLDeleteTexture(glid);
            }

            auto prepData = preprocessImageData(frame.m_data.get(), frame.m_width, frame.m_height);
            auto imData = prepData ? prepData.get() : frame.m_data.get();
    
            m_ccTexture->initWithData(
                imData,
                getTexturePixelFormat(),
                frame.m_width, frame.m_height,
                CCSizeMake(frame.m_width, frame.m_height)
            );
            updateBlendFunc();
    
            m_currentFrameId = frame.m_id;
            m_fpsLimiter.refresh();
        }
    }
    
    CCSprite::visit();
}

