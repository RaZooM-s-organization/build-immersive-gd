// #include "VideoFrame.hpp"
// #include "../settings/Settings.hpp"

// #include <algorithm>

// // TODO: ffmpeg dlls in Geometry Dash folder
// // https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC

// // todo: memory check


// void removeGreenScreen(
//     uint8_t* src_RGB888,
//     uint8_t* dst_RGBA8888,
//     int w,
//     int h,
//     ccColor3B col,
//     float tolerance = 0.1f,
//     float smoothness = 0.1f
// ) {
    
//     uint8_t keyR = col.r;
//     uint8_t keyG = col.g;
//     uint8_t keyB = col.b;

//     const int pixelCount = w * h;
//     for (int i = 0; i < pixelCount; ++i) {
//         int srcIndex = i * 3;
//         int dstIndex = i * 4;

//         uint8_t r = src_RGB888[srcIndex + 0];
//         uint8_t g = src_RGB888[srcIndex + 1];
//         uint8_t b = src_RGB888[srcIndex + 2];

//         int dr = r - keyR;
//         int dg = g - keyG;
//         int db = b - keyB;
//         float distSquare = (dr * dr + dg * dg + db * db) / (3.f * 255 * 255); // 0..1

//         int alpha = 255;

//         if (distSquare < tolerance * tolerance) {
//             alpha = 0;
//         } else if (distSquare < (tolerance + smoothness) * (tolerance + smoothness)) {
//             // linear smoothness
//             alpha = std::clamp((std::sqrt(distSquare) - tolerance) / smoothness * 255, 0.f, 255.f);
//         }

//         dst_RGBA8888[dstIndex + 0] = r;
//         dst_RGBA8888[dstIndex + 1] = g;
//         dst_RGBA8888[dstIndex + 2] = b;
//         dst_RGBA8888[dstIndex + 3] = alpha;
//     }
// }


// VideoFrameAlpha* VideoFrameAlpha::create(std::shared_ptr<CameraMan> cameraMan) {
//     auto ret = new VideoFrameAlpha;
//     if (ret && ret->init(cameraMan)) {
//         ret->autorelease();
//         return ret;
//     }
//     CC_SAFE_DELETE(ret);
//     return nullptr;
// }



// CCTexture2DPixelFormat VideoFrameAlpha::getTexturePixelFormat() const {
//     return kCCTexture2DPixelFormat_RGBA8888;
// }


// std::unique_ptr<uint8_t[]> VideoFrameAlpha::preprocessImageData(uint8_t* data, int w, int h) {
//     // convert RGB to RGBA
//     auto ret = std::make_unique<uint8_t[]>(w * h * 4);
//     auto green = ModSettings::get().m_greenScreen.m_color;
//     float tolerance = ModSettings::get().m_greenScreen.m_tolerance; // 0..100
//     float smoothness = ModSettings::get().m_greenScreen.m_edgeBlur; // 0..100

//     removeGreenScreen(data, ret.get(), w, h, green, tolerance / 100.f, smoothness / 100.f);
//     return std::move(ret);
// }


