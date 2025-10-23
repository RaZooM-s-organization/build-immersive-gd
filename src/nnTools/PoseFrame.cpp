#include "PoseFrame.hpp"
#include "PoseEstimator.hpp"

PoseFrame* PoseFrame::create(std::shared_ptr<PoseEstimator> worker){
    auto ret = new PoseFrame;
    if (ret && ret->init(worker)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool PoseFrame::init(std::shared_ptr<PoseEstimator> worker) {
    if (!CCSprite::init()) return false;
    m_poseEstimator = worker;

    int outWidth, outHeight;
    m_poseEstimator->getFrameSizeInPix(&outWidth, &outHeight);

    m_sizeMultiplier = 1;
    switch (CCDirector::get()->getLoadedTextureQuality()) {
        case TextureQuality::kTextureQualityLow: m_sizeMultiplier = 1; break;
        case TextureQuality::kTextureQualityMedium: m_sizeMultiplier = 0.5; break;
        case TextureQuality::kTextureQualityHigh: m_sizeMultiplier = 0.25; break;
    }
    
    setContentSize(CCSizeMake(outWidth, outHeight) * m_sizeMultiplier);
    return true;
}

void PoseFrame::draw() {
    ccDrawColor4B(ccc4(255, 124, 124, 255));
    auto idk = m_poseEstimator->getPendingPoseResult();
    for (auto& [k, point] : idk.idk) {
        ccDrawFilledCircle(point * m_sizeMultiplier, 5, 0, 10);
    }
    CCSprite::draw();
}
