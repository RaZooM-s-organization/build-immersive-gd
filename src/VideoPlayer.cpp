#include "VideoTools.hpp"


VideoPlayer* VideoPlayer::create(float qualityMultiplier, ClippingMode clippingMode) {
    auto ret = new VideoPlayer;
    if (ret && ret->init(qualityMultiplier, clippingMode)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool VideoPlayer::init(float qualityMultiplier, ClippingMode clippingMode) {
    if (!CCNode::init()) return false;

    m_mode = clippingMode;
    m_videoFrame = VideoFrame::create(qualityMultiplier);

    
    m_clippingNode = CCClippingNode::create();
    m_clippingNode->addChild(m_videoFrame);
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


void VideoPlayer::updateFrameScalePos() {
    // clipping node pos and content size
    m_clippingNode->setPosition(getContentSize() / 2);
    m_clippingNode->setContentSize(getContentSize());

    // stencil node pos and content_size (content_size is set using scale)
    auto stencil = m_clippingNode->m_pStencil;
    stencil->setPosition(getContentSize() / 2);
    stencil->setScaleX(getContentWidth() / stencil->getContentWidth());
    stencil->setScaleY(getContentHeight() / stencil->getContentHeight());

    // frame position ans scale
    float ww = getContentWidth() / m_videoFrame->getContentWidth();
    float hh = getContentHeight() / m_videoFrame->getContentHeight();
    float scale = m_mode == ClippingMode::Fit ? std::min(ww, hh) : std::max(ww, hh);
    m_videoFrame->setPosition(getContentSize() / 2);
    m_videoFrame->setScale(scale);
}


void VideoPlayer::setContentSize(CCSize const& contentSize) {
    CCNode::setContentSize(contentSize);
    updateFrameScalePos();
}

