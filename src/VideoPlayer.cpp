#include "VideoTools.hpp"


static VideoPlayer* create(float qualityMultiplier, ClippingMode clippingMode) {
    auto ret = new VideoPlayer;
    if (ret && ret->init(qualityMultiplier, clippingMode)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool VideoPlayer::init(float qualityMultiplier, ClippingMode clippingMode) {
    if (!CCClippingNode::init()) return false;


    return true;
}


void VideoPlayer::updateVideoFrame(float qualityMultiplier) {

}


void VideoPlayer::setContentSize(CCSize const& contentSize) {

}

