#pragma once


#include "../videoTools/VideoPlayer.hpp"
#include "../videoTools/VideoFrame.hpp"
#include "../pose/PoseFrame.hpp"
#include "../pose/PoseResolver.hpp"

class VideoPreviewPopup : public Popup<int> {
protected:
    const float m_width = 430.f;
    const float m_height = 290.f;
    VideoPlayer* m_videoplayer{};
    VideoFrame* m_videoFrame{};
    PoseFrame* m_poseFrame{};
    
    std::shared_ptr<CameraMan> m_cameraMan{};
    std::shared_ptr<PoseEstimator> m_poseEstimator{};

    CCLabelBMFont* m_poseInfoLbl;
    CCLabelBMFont* m_videoInfoLbl;
    CCLabelBMFont* m_clickLbl;
    CCNodeRGBA* m_dbgBase;

    std::string m_optionalErrors{};

    std::optional<IconType> m_selectedGameMode;
    PoseResolver m_poseResolver;

public:
    static VideoPreviewPopup* create();
    bool setup(int) override;
    void setupCameraPreview();

    void onClose(CCObject* obj) override;
    void onModSettings(CCObject*);
    void onModeButton(CCObject* obj);
    void onPlayButton(CCObject* obj);
    void onCapturePoseButton(CCObject* obj);
    void onOptionalErrorButton(CCObject* obj);

    void updateFpsLabels(float);
    void updatePoseResolver(float);
    void showPlayerClick(PlayerAction action = PlayerAction::Click);
    void debugPrintCurrentPose(float);
    void updateScoresLabels(Pose pose);
};


