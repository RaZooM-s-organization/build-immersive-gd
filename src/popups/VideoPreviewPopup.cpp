#include "VideoPreviewPopup.hpp"

#include "../videoTools/ErrorFrame.hpp"
#include "../nnTools/PoseEstimator.hpp"
#include "../settings/Settings.hpp"

#include <Geode/ui/GeodeUI.hpp>


VideoPreviewPopup* VideoPreviewPopup::create() {
    auto ret = new VideoPreviewPopup();
    if (ret && ret->initAnchored(ret->m_width, ret->m_height, 0)) {
        ret->autorelease();
        return ret; 
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool VideoPreviewPopup::setup(int) {
    // m_closeBtn->setVisible(false);
    setTitle("Video Preview");
    setID("video-preview-popup"_spr);

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
    //         this, menu_selector(VideoPreviewPopup::onClose)
    //     ),
    //     Anchor::Bottom
    // );

    setupCameraPreview();

    return true;
}


void VideoPreviewPopup::setupCameraPreview() {
    m_videoplayer = VideoPlayer::create(ClippingMode::Fit);

    auto cameraMan = std::make_shared<CameraMan>();
    auto res = cameraMan->setupFFMPEG();

    if (res.isOk()) {
        // add video frame
        m_videoFrame = VideoFrame::create(cameraMan);
        m_videoplayer->addFrame(m_videoFrame);
        // add pose m_videoFrame
        auto worker = std::make_shared<PoseEstimator>(cameraMan);
        m_poseFrame = PoseFrame::create(worker);
        m_videoplayer->addFrame(m_poseFrame);
    } else {
        m_videoplayer->addFrame(ErrorFrame::create(res.unwrapErr()));
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

    // settings button
    auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    spr->setScale(0.6f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(VideoPreviewPopup::onModSettings));
    m_buttonMenu->addChildAtPosition(btn, Anchor::Left, ccp(30, 0));

    // FPS labels
    m_videoInfoLbl = CCLabelBMFont::create("", "goldFont.fnt");
    m_videoInfoLbl->setScale(0.5);
    m_videoInfoLbl->setAnchorPoint({0,0});
    m_mainLayer->addChildAtPosition(m_videoInfoLbl, Anchor::BottomLeft, ccp(60, 60));

    m_poseInfoLbl = CCLabelBMFont::create("", "goldFont.fnt");
    m_poseInfoLbl->setScale(0.5);
    m_poseInfoLbl->setAnchorPoint({0,0});
    m_mainLayer->addChild(m_poseInfoLbl);
    m_poseInfoLbl->setPosition(m_videoInfoLbl->getPosition() - ccp(0, 20));

    schedule(schedule_selector(VideoPreviewPopup::updateLabels), 0.5);
    
}


void VideoPreviewPopup::updateLabels(float) {
    bool videoEn = ModSettings::get().m_videoOutput.m_enable;
    auto videoLayer = ModSettings::get().m_videoOutput.m_layer;
    m_videoInfoLbl->setString(fmt::format("Video: {} | Layer: {} | FPS: {}", 
        videoEn ? "enabled" : "disabled",
        videoEn ? enum2str(videoLayer) : "-",
        m_videoFrame ? m_videoFrame->getFps() : 0
    ).c_str());

    bool poseEn = ModSettings::get().m_poseEstimation.m_enable;
    m_poseInfoLbl->setString(fmt::format("Pose control: {} | FPS: {}", 
        poseEn ? "enabled" : "disabled",
        m_poseFrame ? m_poseFrame->getFps() : 0
    ).c_str());
}


void VideoPreviewPopup::onClose(CCObject* obj) {
    Popup::onClose(obj);
}


// geode events suck
class ControlSettingCloseNode : public CCNode {
public:
    static ControlSettingCloseNode* create() {
        auto ret = new ControlSettingCloseNode;
        if (ret && ret->init()) {
            ret->autorelease();
            ret->scheduleUpdate();
            ret->setID("controls-when-the-settings-are-closed"_spr);
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    void update(float) override {
        if (!CCScene::get()->getChildByID("mod-settings"_spr)) {
            VideoPreviewPopup::create()->show();
            removeFromParent();
        }
    }
};


void VideoPreviewPopup::onModSettings(CCObject*) {
    // close this popup and re-open it after the settings change
    onClose(nullptr);
    auto popup = openSettingsPopup(Mod::get(), true);
    popup->setID("mod-settings"_spr);
    CCScene::get()->addChild(ControlSettingCloseNode::create());
}
