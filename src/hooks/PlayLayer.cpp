#include "LevelInfoLayer.hpp"
#include "EditLevelLayer.hpp"
#include "GJBaseGameLayer.hpp"

#include "../popups/VideoPreviewPopup.hpp"
#include "../settings/Settings.hpp"
#include "../pose/PoseResolver.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>


class $modify(MyPlayLayer, PlayLayer) {

    struct Fields {
        std::shared_ptr<CameraMan> m_cameraMan{};
        std::shared_ptr<PoseEstimator> m_poseEstimator{};
        VideoPlayer* m_videoplayer;
        PoseResolver m_poseResolver;
        bool m_enable{};
        bool m_isButtonDown{};
    };


    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        ModGlobal::get().m_shouldBlockInputsInBaseGameLayer = false;

        auto f = m_fields.self();

        // from LevelInfoLayer
        if (auto levelInfo = CCScene::get()->getChildByType<LevelInfoLayer>(0)) {
            auto levelInfoFields = reinterpret_cast<MyLevelInfoLayer*>(levelInfo)->m_fields.self();
            f->m_cameraMan = levelInfoFields->m_cameraMan;
            f->m_enable = levelInfoFields->m_shouldEnableTheModOnPlayLayer;
        }

        // from EditLevelLayer
        if (auto editLevel = CCScene::get()->getChildByType<EditLevelLayer>(0)) {
            auto editLevelFields = reinterpret_cast<MyEditLevelLayer*>(editLevel)->m_fields.self();
            f->m_cameraMan = editLevelFields->m_cameraMan;
            f->m_enable = editLevelFields->m_shouldEnableTheModOnPlayLayer;
        }

        if (!f->m_enable) {
            f->m_cameraMan = nullptr;
            return true;
        };

        if (!f->m_cameraMan) {
            f->m_cameraMan = std::make_shared<CameraMan>();
            auto res = f->m_cameraMan->setupFFMPEG();
            if (res.isErr()) {
                log::error("{}", res.unwrapErr());
                f->m_cameraMan = nullptr;
                f->m_enable = false;
                return true;
            }
        }

        // Here we have a working cameraMan

        // todo: GPU, OBS greenscreen implementation

        f->m_videoplayer = VideoPlayer::create(ClippingMode::Crop);
        f->m_videoplayer->setContentSize(CCDirector::get()->getWinSize());
        m_objectLayer->addChild(f->m_videoplayer, getTargetZOrder(ModSettings::get().m_videoOutput.m_layer));
        

        if (ModSettings::get().m_videoOutput.m_enable) { // show video on screen
            auto videoFrame = VideoFrame::create(f->m_cameraMan);
            f->m_videoplayer->addFrame(videoFrame);

            schedule(schedule_selector(MyPlayLayer::updateVideoPosition));
        }

        if (ModSettings::get().m_poseEstimation.m_enable) { // control the game by pose
            f->m_poseEstimator = std::make_shared<PoseEstimator>(f->m_cameraMan);

            if (ModSettings::get().m_poseEstimation.m_debugDraw) { // show pose debug draw
                auto poseFrame = PoseFrame::create(f->m_poseEstimator);
                f->m_videoplayer->addFrame(poseFrame);
            }

            if (ModSettings::get().m_poseEstimation.m_disableInputs) {
                ModGlobal::get().m_shouldBlockInputsInBaseGameLayer = true;
            }

            schedule(schedule_selector(MyPlayLayer::updatePoseResolve));
        }

        return true;
    }


    void handleButtonForce(bool down, int button = 1, bool isPlayer1 = true) {
        auto f = m_fields.self();
        if (down == f->m_isButtonDown) return;

        bool tmp = ModGlobal::get().m_shouldBlockInputsInBaseGameLayer;
        ModGlobal::get().m_shouldBlockInputsInBaseGameLayer = false;
        handleButton(down, button, isPlayer1);
        ModGlobal::get().m_shouldBlockInputsInBaseGameLayer = tmp;

        f->m_isButtonDown = down;
    }

    void resetLevel() {
        m_fields->m_isButtonDown = false;
        m_fields->m_poseResolver.reset();
        PlayLayer::resetLevel();
    }


    // it's scheduled only if the video is enabled
    void updateVideoPosition(float) {
        auto hws = CCDirector::get()->getWinSize() / 2;
        auto scaledSz = hws / m_gameState.m_cameraZoom;
        auto camCenterInEditor = m_gameState.m_cameraPosition + scaledSz;
        auto videoPlayer = m_fields->m_videoplayer;
        videoPlayer->setPosition(camCenterInEditor);
        videoPlayer->setRotation(-m_gameState.m_cameraAngle);
        videoPlayer->setScale(1 / m_gameState.m_cameraZoom);
    }

    // it's scheduled only if the pose estimation is enabled
    void updatePoseResolve(float) {

        auto f = m_fields.self();

        auto currPose = Pose(f->m_poseEstimator->getPendingPoseResult().m_points);
        
        auto res = f->m_poseResolver.nextPose(getActiveModeAsIconType(), currPose);

        if (res) {
            PlayerAction action = res.value().second;
            switch (action) {
                case PlayerAction::Click: {
                    handleButtonForce(true);
                    scheduleOnce(schedule_selector(MyPlayLayer::delayedButtonUp), 0);
                    break;
                }
                case PlayerAction::Hold: {
                    handleButtonForce(true);
                    break;
                }
                default: { // none
                    handleButtonForce(false);
                    break;
                }
            }
        }
    }


    void delayedButtonUp(float) {
        handleButtonForce(false);
    }

    void buttonSwitch(float) {
        handleButtonForce(!m_fields->m_isButtonDown);
    }


    IconType getActiveModeAsIconType() {
        return reinterpret_cast<MyGJBaseGameLayer*>(this)->m_fields->m_currentMode;
    }


    // слой НАД которым будет картинка
    int getTargetZOrder(VideoOutputLayer layer) {
        switch (layer) {
            case VideoOutputLayer::T4: return 1380;
            case VideoOutputLayer::T3: return 970;
            case VideoOutputLayer::T2: return 670;
            case VideoOutputLayer::T1: return 370;
            case VideoOutputLayer::B0: return 10;
            case VideoOutputLayer::B1: return -10;
            case VideoOutputLayer::B2: return -310;
            case VideoOutputLayer::B3: return -610;
            case VideoOutputLayer::B4: return -910;
            case VideoOutputLayer::B5: return -1210;
            default: return -1490; // bg
        }
    }


};

/*

T5, min: 1074, max: 1360
T4, min: 1010, max: 1340
T3, min: 710, max: 960
T2, min: 410, max: 660
T1, min: 120, max: 360
B0, min: -190, max: -5
B1, min: -280, max: -20
B2, min: -580, max: -320
B3, min: -880, max: -620
B4, min: -1180, max: -920
B5, min: -1480, max: -1220

*/