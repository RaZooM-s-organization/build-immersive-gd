#include "PoseResolver.hpp"

PoseResolver* PoseResolver::create(std::shared_ptr<PoseEstimator> worker, std::function<void(PlayerAction)> actionCallback){
    auto ret = new PoseResolver;
    if (ret && ret->init(worker, actionCallback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool PoseResolver::init(std::shared_ptr<PoseEstimator> worker, std::function<void(PlayerAction)> actionCallback) {
    if (!CCNode::init()) return false;
    m_poseEstimator = worker;
    m_actionCallback = actionCallback;
    m_mode = IconType::Cube;

    scheduleUpdate();
    
    return true;
}

void PoseResolver::update(float delta) {
    // get latest pose from estimator

    // check if it's changes

    // get poses for gamemode

    // get matching poseAction

    // return PlayerAction PoseActionPrototype::transitionToPose or None

}

void PoseResolver::setGameMode(IconType mode) {
    m_mode = mode;
}