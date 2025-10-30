#pragma once

#include "../nnTools/PoseEstimator.hpp"
#include "Pose.hpp"


// Takes the data from the pose estimator, processes it, and calls 
// the callback when player needs to jump
class PoseResolver : public CCNode {

    std::shared_ptr<PoseEstimator> m_poseEstimator;
    std::function<void(PlayerAction)> m_actionCallback;
    IconType m_mode;
    PoseId m_currentPoseId;

public:

    static PoseResolver* create(std::shared_ptr<PoseEstimator> worker, std::function<void(PlayerAction)> actionCallback);
    bool init(std::shared_ptr<PoseEstimator> worker, std::function<void(PlayerAction)> actionCallback);

    void update(float delta) override;

    void setGameMode(IconType mode);

};

