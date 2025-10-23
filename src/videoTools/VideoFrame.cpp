#include "VideoFrame.hpp"


// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC

// todo: memory check

// #include <chrono>
// static std::chrono::steady_clock::time_point dbgTmrBegin;
// struct DbgTimer {
//     static void start() {dbgTmrBegin = std::chrono::steady_clock::now();}
//     static void stop(bool print=true) {
//         auto end = std::chrono::steady_clock::now();
//         auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(end - dbgTmrBegin).count();
//         if (print) log::info("Timer: {} mks", d1);
//     }
// };



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


bool VideoFrame::init(std::shared_ptr<CameraMan> cameraMan) {

    // const char* device_name = "video=Live Streamer CAM 313"; // todo:

    m_cameraMan = cameraMan;

    int outWidth, outHeight;
    m_cameraMan->getFrameSizeInPix(&outWidth, &outHeight);

    m_ccTexture = new CCTexture2D();
    m_ccTexture->initWithData(
        nullptr,
        kCCTexture2DPixelFormat_RGB888,
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
    return true;
}


void VideoFrame::visit() {
    
    auto frame = m_cameraMan->getPendingFrame();

    if (frame.m_id != m_currentFrameId && frame.m_data.get() != nullptr) {

        if (auto glid = m_ccTexture->getName()) {
            ccGLDeleteTexture(glid);
        }

        m_ccTexture->initWithData(
            frame.m_data.get(),
            kCCTexture2DPixelFormat_RGB888,
            frame.m_width, frame.m_height,
            CCSizeMake(frame.m_width, frame.m_height)
        );
        updateBlendFunc();

        m_currentFrameId = frame.m_id;
    }
    CCSprite::visit();
}

