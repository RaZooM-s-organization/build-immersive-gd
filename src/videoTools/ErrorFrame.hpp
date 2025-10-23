#pragma once


#include "VideoTools.hpp"


class ErrorFrame : public CCSprite {
    std::string m_errText;
public:
    static ErrorFrame* create(std::string errText);
    bool init(std::string errText);
    void onBtn(CCObject* sender);
};
