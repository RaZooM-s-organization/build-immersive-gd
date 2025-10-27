#include "VideoFrame.hpp"
#include "../settings/Settings.hpp"

// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC

// todo: memory check




VideoFrameAlpha* VideoFrameAlpha::create(std::shared_ptr<CameraMan> cameraMan) {
    auto ret = new VideoFrameAlpha;
    if (ret && ret->init(cameraMan)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}



CCTexture2DPixelFormat VideoFrameAlpha::getTexturePixelFormat() const {
    return kCCTexture2DPixelFormat_RGBA8888;
}


std::unique_ptr<uint8_t[]> VideoFrameAlpha::preprocessImageData(uint8_t* data, int w, int h) {
    // todo: remove green screen
    return nullptr; // already desired format
}


