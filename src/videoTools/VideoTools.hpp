#pragma once




#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>

#include <memory>



using namespace geode::prelude;




// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC




// Frame format is strictly RGB
struct CameraFrame {
    uint32_t m_id = 0;
    uint32_t m_width;
    uint32_t m_height;
    std::shared_ptr<uint8_t> m_data;
};



enum class ClippingMode {
    Crop, Fit
};


