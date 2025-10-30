#pragma once


#include "../videoTools/VideoPlayer.hpp"
#include "../videoTools/VideoFrame.hpp"
#include "../pose/PoseFrame.hpp"


class VideoPreviewPopup : public Popup<int> {
protected:
    const float m_width = 430.f;
    const float m_height = 290.f;
    VideoPlayer* m_videoplayer{};
    VideoFrame* m_videoFrame{};
    PoseFrame* m_poseFrame{};
    
    std::shared_ptr<CameraMan> m_cameraMan{};

    CCLabelBMFont* m_poseInfoLbl;
    CCLabelBMFont* m_videoInfoLbl;
    CCLabelBMFont* m_debugLabel;
    CCLabelBMFont* m_clickLbl;

    std::optional<IconType> m_selectedGameMode;

public:
    static VideoPreviewPopup* create();
    bool setup(int) override;
    void setupCameraPreview();

    void onClose(CCObject* obj) override;
    void onModSettings(CCObject*);
    void onModeButton(CCObject* obj);
    void onPlayButton(CCObject* obj);
    void onCapturePoseButton(CCObject* obj);

    void updateFpsLabels(float);
    void updateModeLabels(float);
    void flashClickLabel();
    void printCurrentPose(float);
};


