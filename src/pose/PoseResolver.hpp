#pragma once

#include "Pose.hpp"


class PoseResolver {

    PoseId m_currentPoseId = PoseId::Any;

public:

    // return nullopt if pose did not change since the last update
    std::optional<std::pair<PoseId, PlayerAction>> nextPose(IconType mode, Pose pose);

    std::vector<std::pair<const char*, float>> getScores(IconType mode, Pose pose);
};

