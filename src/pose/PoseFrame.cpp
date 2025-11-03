#include "PoseFrame.hpp"
#include "../settings/Settings.hpp"


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

    m_shouldFlipX = ModSettings::get().m_videoOutput.m_mirror;

    return true;
}


float PoseFrame::getFps() {
    return m_poseEstimator->getFps();
}


void PoseFrame::visit() {
    if (m_shouldFlipX) {
        setScaleX(-getScaleX());
        CCSprite::visit();
        setScaleX(-getScaleX());
    } else {
        CCSprite::visit();
    }
}


inline void tryToDrawLine(int a, int b, std::map<int, cocos2d::CCPoint> &map, float mul) {
    auto itA = map.find(a);
    auto itB = map.find(b);
    if (itA != map.end() && itB != map.end()) {
        ccDrawLine(itA->second * mul, itB->second * mul);
    }
}

void PoseFrame::draw() {
    auto poseResult = m_poseEstimator->getPendingPoseResult();
    float scaleMultiplier = 1 / getScale();

    ccDrawColor4B(ccc4(225, 255, 0, 255));
    glLineWidth(5);

    tryToDrawLine(0, 1, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(1, 3, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(0, 2, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(2, 4, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(0, 5, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(5, 7, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(7, 9, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(0, 6, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(6, 8, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(8, 10, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(5, 11, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(11, 13, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(13, 15, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(6, 12, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(12, 14, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(14, 16, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(5, 6, poseResult.m_points, m_sizeMultiplier);
    tryToDrawLine(11, 12, poseResult.m_points, m_sizeMultiplier);

    ccDrawColor4B(ccc4(255, 125, 125, 255));
    for (auto& [k, point] : poseResult.m_points) {
        ccDrawFilledCircle(point * m_sizeMultiplier, 4 * scaleMultiplier, 0, 10);
    }

    glLineWidth(1);

    CCSprite::draw();
}
