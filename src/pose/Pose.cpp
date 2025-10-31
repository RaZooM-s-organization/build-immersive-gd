#include "Pose.hpp"

#include <cmath>
#include <numbers>

float getAngleOfAVector(float dx, float dy) {
    float angle = std::atan2(dy, dx); // -pi..pi
    if (angle < 0) angle += 2.0f * std::numbers::pi; // 0..2*pi
    return angle;
}

float getAngleOfAVector(CCPoint &begin, CCPoint &end) {
    return getAngleOfAVector(end.x - begin.x, end.y - begin.y);
}

// 0..pi
float angleDiff(float alpha, float beta) {
    float d = std::abs(alpha - beta);
    if (d > std::numbers::pi) {
        return std::numbers::pi * 2 - d;
    }
    return d;
}

Pose::Pose(std::map<int, CCPoint> points) {
    struct {
        int pointBegin, pointEnd; 
        Bone bone;
    } bones[] = {
        {5, 7, Bone::Bone_5_7},
        {6, 8, Bone::Bone_6_8},
        {7, 9, Bone::Bone_7_9},
        {8, 10, Bone::Bone_8_10},
    };

    for (auto &bone : bones) {
        auto a = points.find(bone.pointBegin);
        if (a == points.end()) continue;
        auto b = points.find(bone.pointEnd);
        if (b == points.end()) continue;
        m_bones[bone.bone] = getAngleOfAVector(a->second, b->second);
    }
}

Pose::Pose(std::map<Bone, float> bones) {
    m_bones = bones;
}


// returns 0..1
float PosePrototype::getSimilarityRatio(const Pose &other) const {
    float sum2 = 0;
    int count = this->m_pose.m_bones.size();

    for (auto& [bone, thisAngle] : this->m_pose.m_bones) {
        auto it = other.m_bones.find(bone);
        if (it != other.m_bones.end()) {
            float otherAngle = it->second;
            float d = angleDiff(thisAngle, otherAngle);
            d = d / std::numbers::pi; // normalize to 0..1
            sum2 += d * d;
        } else {
            sum2 += 0.5;
        }
    }

    return 1 - std::sqrt(sum2 / count);
}

PlayerAction PosePrototype::transitionToPose(PoseId prev) const {
    auto it = m_actions.find(prev);
    if (it != m_actions.end()) {
        return it->second;
    }
    it = m_actions.find(PoseId::Any);
    if (it != m_actions.end()) {
        return it->second;
    }
    return PlayerAction::None;
}

void Pose::debugPrint() {
    std::map<Pose::Bone, const char*> mappings = {
        {Pose::Bone::Bone_5_7, "Bone_5_7"},
        {Pose::Bone::Bone_6_8, "Bone_6_8"},
        {Pose::Bone::Bone_7_9, "Bone_7_9"},
        {Pose::Bone::Bone_8_10, "Bone_8_10"}
    };
    
    std::string s;
    
    for (auto [bone, angle] : m_bones) {
        s += mappings[bone];
        s += ": ";
        s += std::to_string(angle);
        s += ", ";
    }
    
    log::info("Pose: {}", s);
}