#pragma once


#include "../videoTools/VideoPlayer.hpp"
#include "../videoTools/VideoFrame.hpp"
#include "../nnTools/PoseFrame.hpp"


class VideoPreviewPopup : public Popup<int> {
protected:
    const float m_width = 430.f;
    const float m_height = 290.f;
    VideoPlayer* m_videoplayer{};
    VideoFrame* m_videoFrame{};
    PoseFrame* m_poseFrame{};

    CCLabelBMFont* m_poseInfoLbl;
    CCLabelBMFont* m_videoInfoLbl;

public:
    static VideoPreviewPopup* create();
    bool setup(int) override;
    void setupCameraPreview();
    void onClose(CCObject* obj) override;
    void onModSettings(CCObject*);

    void updateLabels(float);
};


