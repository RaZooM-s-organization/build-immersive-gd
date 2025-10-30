#pragma once

#include "../nnTools/PoseEstimator.hpp"



class PoseFrame : public CCSprite {

    std::shared_ptr<PoseEstimator> m_poseEstimator;
    float m_sizeMultiplier;

public:

    static PoseFrame* create(std::shared_ptr<PoseEstimator> worker);
    bool init(std::shared_ptr<PoseEstimator> worker);

    void draw() override;

    int getFps();

};