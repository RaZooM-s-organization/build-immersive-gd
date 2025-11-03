#pragma once

#include "../nnTools/PoseEstimator.hpp"



class PoseFrame : public CCSprite {
protected:
    std::shared_ptr<PoseEstimator> m_poseEstimator;
    float m_sizeMultiplier;
    bool m_shouldFlipX;

public:

    static PoseFrame* create(std::shared_ptr<PoseEstimator> worker);
    bool init(std::shared_ptr<PoseEstimator> worker);

    void draw() override;
    void visit() override;

    float getFps() const;
    int getLastInferenceTimeMs() const;

};