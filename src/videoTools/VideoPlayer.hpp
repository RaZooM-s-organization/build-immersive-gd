#pragma once


#include "VideoTools.hpp"


class VideoPlayer : public CCNode {
protected:

    ClippingMode m_mode;
    CCClippingNode* m_clippingNode;

public:

    static VideoPlayer* create(ClippingMode clippingMode = ClippingMode::Fit);

    bool init(ClippingMode clippingMode);
    void setContentSize(CCSize const& contentSize) override;

    void addFrame(CCNode* frame);
    void removeFrame(CCNode* frame);
    void updateFramesScalePos();

};