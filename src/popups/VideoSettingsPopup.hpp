#pragma once


#include "../videoTools/VideoPlayer.hpp"


class VideoSettingsPopup : public Popup<int> {
protected:
    const float m_width = 430.f;
    const float m_height = 290.f;
    VideoPlayer* m_videoplayer{};

public:
    static VideoSettingsPopup* create();
    bool setup(int) override;
    void setupCameraPreview();
    void onClose(CCObject* obj) override;
};


