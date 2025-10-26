#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

struct PoseResult {
    uint32_t m_id{};
    int m_width{};
    int m_height{};
    // points in source image pixel coords from bottom left
    std::map<int, CCPoint> m_points{};
};



