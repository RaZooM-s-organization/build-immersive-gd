#pragma once


enum class VideoOutputLayer; // defined in Settings.hpp


VideoOutputLayer str2enum(std::string_view str);

std::string enum2str(VideoOutputLayer val);
