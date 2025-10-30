#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


#include <unordered_map>


enum class PoseId {
    Any,
    ForearmsUp,

};

enum class PlayerAction {
    None, // stop holding
    Click,
    Hold
};


struct Pose {
    enum class Bone {
        Bone_5_7, Bone_7_9,
        Bone_6_8, Bone_8_10
    };

    Pose(std::map<int, CCPoint> points);

    std::unordered_map<Bone, float> m_bones;
};


// Store known poses in this struct
struct PoseActionPrototype {
    const PoseId m_id;
    const char* m_name;

    const Pose m_pose;

    // Actions performed on transition from previous pose to this one
    const std::unordered_map<PoseId, PlayerAction> m_actions;

    float getSimilarityRatio(const Pose &other) const;
    PlayerAction transitionToPose(PoseId prev) const;

};


