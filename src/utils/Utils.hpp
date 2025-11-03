#pragma once


enum class VideoOutputLayer; // defined in Settings.hpp


VideoOutputLayer str2enum(std::string_view str);

std::string enum2str(VideoOutputLayer val);


// void removeGreenScreen(uint8_t* src_RGB888, uint8_t* dst_RGBA8888, int w, int h);
