#include "VideoPlayer.hpp"


VideoPlayer* VideoPlayer::create(ClippingMode clippingMode) {
    auto ret = new VideoPlayer;
    if (ret && ret->init(clippingMode)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool VideoPlayer::init(ClippingMode clippingMode) {
    if (!CCNode::init()) return false;

    m_mode = clippingMode;
    
    m_clippingNode = CCClippingNode::create();
    m_clippingNode->setStencil(CCSprite::create("pixel.png"));
    m_clippingNode->setAlphaThreshold(0.7f);
    m_clippingNode->setAnchorPoint({0.5,0.5});

    addChild(m_clippingNode);
    setAnchorPoint({0.5,0.5});
    setContentSize({100, 100});
    return true;
}


// void VideoPlayer::updateOutputVideoQuality(float qualityMultiplier) {

// }


void VideoPlayer::updateFramesScalePos() {
    // clipping node pos and content size
    m_clippingNode->setPosition(getContentSize() / 2);
    m_clippingNode->setContentSize(getContentSize());

    // stencil node pos and content_size (content_size is set using scale)
    auto stencil = m_clippingNode->m_pStencil;
    stencil->setPosition(getContentSize() / 2);
    stencil->setScaleX(getContentWidth() / stencil->getContentWidth());
    stencil->setScaleY(getContentHeight() / stencil->getContentHeight());

    // frame position ans scale
    for (auto child : m_clippingNode->getChildrenExt()) {
        float ww = getContentWidth() / child->getContentWidth();
        float hh = getContentHeight() / child->getContentHeight();
        float scale = m_mode == ClippingMode::Fit ? std::min(ww, hh) : std::max(ww, hh);
        child->setPosition(getContentSize() / 2);
        child->setScale(scale);
    }
}


void VideoPlayer::addFrame(CCNode* frame) {
    m_clippingNode->addChild(frame);
    updateFramesScalePos();
}


void VideoPlayer::removeFrame(CCNode* frame) {
    m_clippingNode->removeChild(frame);
}


void VideoPlayer::setContentSize(CCSize const& contentSize) {
    CCNode::setContentSize(contentSize);
    updateFramesScalePos();
}

