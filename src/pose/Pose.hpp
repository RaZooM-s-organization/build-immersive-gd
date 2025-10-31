#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


#include <map>


enum class PoseId {
    Any,
    Cube_ForearmsUp,

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

    Pose(std::map<int, CCPoint> points); // from points to bones
    Pose(std::map<Bone, float> bones);
    Pose() = default;

    std::map<Bone, float> m_bones;

    void debugPrint();
};


// Store known poses in this struct
struct PosePrototype {
    const PoseId m_id;
    const char* m_name;

    const Pose m_pose;

    // Actions performed on transition from previous pose to this one
    const std::map<PoseId, PlayerAction> m_actions;

    float getSimilarityRatio(const Pose &other) const;
    PlayerAction transitionToPose(PoseId prev) const;

};



std::map<IconType, std::vector<PosePrototype>> const& getPoseData();
