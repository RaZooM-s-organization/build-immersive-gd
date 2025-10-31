#include "PoseResolver.hpp"

#define SIMILARITY_THRESHOLD 0.6f


std::optional<std::pair<PoseId, PlayerAction>> PoseResolver::nextPose(IconType mode, Pose pose) {

    if (!getPoseData().contains(mode)) {
        if (m_currentPoseId != PoseId::Any) {
            m_currentPoseId = PoseId::Any;
            return {{PoseId::Any, PlayerAction::None}};
        }
        return std::nullopt;
    }

    auto &variants = getPoseData().at(mode);

    float maxVal = -99999;
    int maxIdx = -1;

    for (int i = 0; i < variants.size(); i++) {
        PosePrototype const &proto = variants[i];
        float sim = proto.getSimilarityRatio(pose);
        if (sim > maxVal) {
            maxVal = sim;
            maxIdx = i;
        }
    }

    if (maxIdx == -1 || maxVal < SIMILARITY_THRESHOLD) {
        if (m_currentPoseId != PoseId::Any) {
            m_currentPoseId = PoseId::Any;
            return {{PoseId::Any, PlayerAction::None}};
        }
        return std::nullopt;
    }

    PosePrototype const &nextPose = variants[maxIdx];

    if (m_currentPoseId == nextPose.m_id) {
        return std::nullopt;
    }

    PlayerAction action = nextPose.transitionToPose(m_currentPoseId);
    m_currentPoseId = nextPose.m_id;

    return {{m_currentPoseId, action}};
}


std::vector<std::pair<const char*, float>> PoseResolver::getScores(IconType mode, Pose pose) {
    std::vector<std::pair<const char*, float>> ret;

    if (!getPoseData().contains(mode)) return ret;
    
    auto &variants = getPoseData().at(mode);
    
    for (int i = 0; i < variants.size(); i++) {
        PosePrototype const &proto = variants[i];
        float sim = proto.getSimilarityRatio(pose);
        ret.emplace_back(proto.m_name, sim);
    }

    return ret;
}


