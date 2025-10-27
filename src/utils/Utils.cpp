#include "Utils.hpp"
#include "../settings/Settings.hpp"


std::map<std::string, VideoOutputLayer> str2enumMappings = {
    {"BG", VideoOutputLayer::BG},
    {"B5", VideoOutputLayer::B5},
    {"B4", VideoOutputLayer::B4},
    {"B3", VideoOutputLayer::B3},
    {"B2", VideoOutputLayer::B2},
    {"B1", VideoOutputLayer::B1},
    {"B0", VideoOutputLayer::B0},
    {"T1", VideoOutputLayer::T1},
    {"T2", VideoOutputLayer::T2},
    {"T3", VideoOutputLayer::T3},
    {"T4", VideoOutputLayer::T4}
};

VideoOutputLayer str2enum(std::string_view str) {
    for (auto& [s, e] : str2enumMappings) {
        if (s == str) return e;
    }
    return VideoOutputLayer::BG;
}

std::string enum2str(VideoOutputLayer val) {
    for (auto& [s, e] : str2enumMappings) {
        if (e == val) return s;
    }
    return "";
}