#include "VideoSettingsPopup.hpp"

#include "../utils/ChoiceNode.hpp"
#include "../videoTools/VideoFrame.hpp"
#include "../videoTools/ErrorFrame.hpp"
#include "../nnTools/PoseEstimator.hpp"
#include "../nnTools/PoseFrame.hpp"

VideoSettingsPopup* VideoSettingsPopup::create() {
    auto ret = new VideoSettingsPopup();
    if (ret && ret->initAnchored(ret->m_width, ret->m_height, 0)) {
        ret->autorelease();
        return ret; 
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool VideoSettingsPopup::setup(int) {
    // m_closeBtn->setVisible(false);
    setTitle("Video Preview");
    setID("video-settings-popup"_spr);

    m_buttonMenu->addChildAtPosition(
        InfoAlertButton::create(
            "Info", 
            "todo", 
            0.75
        ), 
        Anchor::TopRight, ccp(-18, -18)
    );

    // m_buttonMenu->addChildAtPosition(
    //     CCMenuItemSpriteExtra::create(
    //         ButtonSprite::create("ok", "goldFont.fnt", "GJ_button_01.png", 1),
    //         this, menu_selector(VideoSettingsPopup::onClose)
    //     ),
    //     Anchor::Bottom
    // );

    setupCameraPreview();

    return true;
}

void VideoSettingsPopup::setupCameraPreview() {
    m_videoplayer = VideoPlayer::create(ClippingMode::Fit);

    auto cameraMan = std::make_shared<CameraMan>("Live Streamer CAM 313");
    auto res = cameraMan->setupFFMPEG();

    if (res.isOk()) {
        // add video frame
        auto frame = VideoFrame::create(cameraMan);
        m_videoplayer->addFrame(frame);
        // add pose frame
        auto worker = std::make_shared<PoseEstimator>(cameraMan);
        auto poseFrame = PoseFrame::create(worker);
        m_videoplayer->addFrame(poseFrame);
    } else {
        auto frame = ErrorFrame::create(res.unwrapErr());
        m_videoplayer->addFrame(frame);
    }

    auto winSz = CCDirector::get()->getWinSize();

    // height is fixed, height is adaptive
    float h = 150;
    float w = h * winSz.width / winSz.height;

    float posX = m_mainLayer->getContentWidth() / 2;
    float posY = m_mainLayer->getContentHeight() - h / 2 - 40;
    m_videoplayer->setContentSize({w,h});
    m_mainLayer->addChild(m_videoplayer);
    m_videoplayer->setPosition({posX, posY});
    m_videoplayer->setZOrder(2);

    // video player bg
    auto bg = CCLayerColor::create();
    bg->setColor(ccc3(0,0,0));
    bg->setOpacity(100);
    bg->ignoreAnchorPointForPosition(false);
    bg->setContentSize(m_videoplayer->getContentSize() + ccp(10,10));
    m_mainLayer->addChild(bg);
    bg->setPosition(m_videoplayer->getPosition());
    bg->setZOrder(0);
    

    // title
    // auto lbl = CCLabelBMFont::create("Video Preview", "bigFont.fnt");
    // m_mainLayer->addChild(lbl);
    // lbl->setPosition({posX, posY + h / 2 + 15});
    // lbl->setScale(0.5);
}

void VideoSettingsPopup::onClose(CCObject* obj) {
    Popup::onClose(obj);
}
