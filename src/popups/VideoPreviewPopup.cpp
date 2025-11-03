#include "VideoPreviewPopup.hpp"

#include "../videoTools/ErrorFrame.hpp"
#include "../nnTools/PoseEstimator.hpp"
#include "../settings/Settings.hpp"
#include "../hooks/LevelInfoLayer.hpp"
#include "../hooks/EditLevelLayer.hpp"

#include <Geode/ui/GeodeUI.hpp>


#define DEBUG_LABELS_OPACITY 150


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
            "What the heck!?", 
            "In the top left corner of the video you can see the <cj>similarity coefficients</c> "
            "of your pose and target pose. If coefficient is > <cr>0.7</c>, then your pose is "
            "accepted. Target poses depend on the game mode\n"
            "- Use game mode buttons to train a certain game mode\n"
            "- Use settings button to open mod settings\n"
            "<co>You can find more information and a list of target poses in the mod tutorial</c>",
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

    m_cameraMan = std::make_shared<CameraMan>();
    auto res = m_cameraMan->setupFFMPEG();

    if (res.isOk()) {
        // add video frame
        m_videoFrame = VideoFrame::create(m_cameraMan);
        // if (ModSettings::get().m_greenScreen.m_enable) {
        //     m_videoFrame = VideoFrameAlpha::create(m_cameraMan);
        // } else {
        //     m_videoFrame = VideoFrame::create(m_cameraMan);
        // }
        m_videoplayer->addFrame(m_videoFrame);
        // add pose m_videoFrame
        m_poseEstimator = std::make_shared<PoseEstimator>(m_cameraMan);
        m_poseFrame = PoseFrame::create(m_poseEstimator);
        m_videoplayer->addFrame(m_poseFrame);
    } else {
        m_videoplayer->addFrame(ErrorFrame::create(res.unwrapErr()));
        m_cameraMan = nullptr;
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
    

    // gameMode buttons
    auto gmButtonsMenu = CCMenu::create();
    const char* gmButtonNames[] = {
        "gj_iconBtn_off_001.png", "gj_iconBtn_on_001.png",
        "gj_shipBtn_off_001.png", "gj_shipBtn_on_001.png",
        "gj_ballBtn_off_001.png", "gj_ballBtn_on_001.png",
        "gj_birdBtn_off_001.png", "gj_birdBtn_on_001.png",
        "gj_dartBtn_off_001.png", "gj_dartBtn_on_001.png",
        "gj_robotBtn_off_001.png", "gj_robotBtn_on_001.png",
        "gj_spiderBtn_off_001.png", "gj_spiderBtn_on_001.png",
        "gj_swingBtn_off_001.png", "gj_swingBtn_on_001.png"
    };
    for (int i = 0; i < 8; i++) {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName(gmButtonNames[2*i]),
            CCSprite::createWithSpriteFrameName(gmButtonNames[2*i+1]),
            this, menu_selector(VideoPreviewPopup::onModeButton)
        );
        toggler->setTag(i); // equal to IconType::
        gmButtonsMenu->addChild(toggler);
    }
    m_mainLayer->addChildAtPosition(gmButtonsMenu, Anchor::Bottom, ccp(0, 75));
    gmButtonsMenu->setContentSize({215, 20});
    gmButtonsMenu->setLayout(RowLayout::create());
    static_cast<CCMenuItemToggler*>(gmButtonsMenu->getChildByTag((int)IconType::Cube))->activate();


    // settings button
    auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    spr->setScale(0.6f);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(VideoPreviewPopup::onModSettings));
    m_buttonMenu->addChildAtPosition(btn, Anchor::Left, ccp(30, 0));

    // play button
    auto playBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    playBtnSpr->setScale(0.75);
    auto playBtn = CCMenuItemSpriteExtra::create(
        playBtnSpr, this, menu_selector(VideoPreviewPopup::onPlayButton)
    );
    m_buttonMenu->addChildAtPosition(playBtn, Anchor::BottomRight, ccp(-47,47));
    if (CCScene::get()->getChildByType<LevelInfoLayer>(0) == nullptr && CCScene::get()->getChildByType<EditLevelLayer>(0) == nullptr) {
        playBtn->setVisible(false);
    }

    // Capture pose button
    auto capturePoseButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Cap.", "goldFont.fnt", "GJ_button_01.png", 1),
        this, menu_selector(VideoPreviewPopup::onCapturePoseButton)
    );
    m_buttonMenu->addChildAtPosition(capturePoseButton, Anchor::Left, ccp(30, -30));
    capturePoseButton->setVisible(false);


    // FPS labels
    m_videoInfoLbl = CCLabelBMFont::create("", "goldFont.fnt");
    m_videoInfoLbl->setScale(0.5);
    m_videoInfoLbl->setAnchorPoint({0,0});
    m_mainLayer->addChildAtPosition(m_videoInfoLbl, Anchor::BottomLeft, ccp(60, 40));

    m_poseInfoLbl = CCLabelBMFont::create("", "goldFont.fnt");
    m_poseInfoLbl->setScale(0.5);
    m_poseInfoLbl->setAnchorPoint({0,0});
    m_mainLayer->addChild(m_poseInfoLbl);
    m_poseInfoLbl->setPosition(m_videoInfoLbl->getPosition() - ccp(0, 20));

    // click label
    m_clickLbl = CCLabelBMFont::create("Click!", "bigFont.fnt");
    m_clickLbl->setScale(0.5);
    m_clickLbl->setColor(ccc3(0, 255, 0));
    m_clickLbl->setOpacity(0);
    m_mainLayer->addChildAtPosition(m_clickLbl, Anchor::TopRight, ccp(-85, -18));
    

    // pose debug labels
    m_dbgBase = CCNodeRGBA::create();
    m_dbgBase->setPosition(m_videoplayer->getPosition());
    m_dbgBase->setContentSize(m_videoplayer->getScaledContentSize());
    m_dbgBase->setAnchorPoint({0.5,0.5});

    
    m_dbgBase->setCascadeOpacityEnabled(true);
    m_dbgBase->setLayout(AxisLayout::create(Axis::Column)
        ->setGrowCrossAxis(true)
        ->setAutoScale(false)
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(AxisAlignment::End)
        ->setCrossAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::Start)
        ->setAxisReverse(true)
        ->setCrossAxisReverse(true)
    );
    m_dbgBase->setOpacity(DEBUG_LABELS_OPACITY);

    m_mainLayer->addChild(m_dbgBase, 5);

    schedule(schedule_selector(VideoPreviewPopup::updateFpsLabels), 0.5);
    schedule(schedule_selector(VideoPreviewPopup::updatePoseResolver), 0.1);
    
}


void VideoPreviewPopup::updateFpsLabels(float) {
    bool videoEn = ModSettings::get().m_videoOutput.m_enable;
    auto videoLayer = ModSettings::get().m_videoOutput.m_layer;
    m_videoInfoLbl->setString(fmt::format("Video: {} | Layer: {} | FPS: {:.1f}", 
        videoEn ? "enabled" : "disabled",
        videoEn ? enum2str(videoLayer) : "-",
        m_videoFrame ? m_videoFrame->getFps() : 0.f
    ).c_str());

    bool poseEn = ModSettings::get().m_poseEstimation.m_enable;
    m_poseInfoLbl->setString(fmt::format("Pose control: {} | FPS: {:.1f}", 
        poseEn ? "enabled" : "disabled",
        m_poseFrame ? m_poseFrame->getFps() : 0.f
    ).c_str());
}


void VideoPreviewPopup::updatePoseResolver(float) {
    if (!m_poseEstimator || !m_selectedGameMode) return;

    auto currPose = Pose(m_poseEstimator->getPendingPoseResult().m_points);

    updateScoresLabels(currPose);

    auto res = m_poseResolver.nextPose(m_selectedGameMode.value(), currPose);

    if (res) {
        showPlayerClick(res.value().second);
    }
}


void VideoPreviewPopup::updateScoresLabels(Pose pose) {
    if (!m_poseEstimator || !m_selectedGameMode) return;
    auto scores = m_poseResolver.getScores(m_selectedGameMode.value(), pose);

    m_dbgBase->removeAllChildren();

    for (auto s : scores) {
        auto lbl = CCLabelBMFont::create(
            fmt::format("{}: {}", s.first, std::round(s.second * 100) / 100.f).c_str(),
            "bigFont.fnt"
        );
        lbl->setScale(0.5);
        m_dbgBase->addChild(lbl);
    }

    m_dbgBase->updateLayout();
    m_dbgBase->setOpacity(DEBUG_LABELS_OPACITY);
}




void VideoPreviewPopup::onCapturePoseButton(CCObject* obj) {
    showPlayerClick();
    scheduleOnce(schedule_selector(VideoPreviewPopup::debugPrintCurrentPose), 5);
}


void VideoPreviewPopup::showPlayerClick(PlayerAction action) {
    switch (action) {
        case PlayerAction::Click: {
            m_clickLbl->stopActionByTag(1111);
            m_clickLbl->setOpacity(255);
            auto a = CCFadeOut::create(0.75);
            a->setTag(1111);
            m_clickLbl->runAction(a);
            break;
        }
        case PlayerAction::Hold: {
            m_clickLbl->stopActionByTag(1111);
            m_clickLbl->setOpacity(255);
            break;
        }
        default: { // none
            if (m_clickLbl->getActionByTag(1111) == nullptr) {
                m_clickLbl->setOpacity(0);
            }
            break;
        }
    }
}


void VideoPreviewPopup::debugPrintCurrentPose(float) {
    showPlayerClick();
    if (!m_poseEstimator || !m_selectedGameMode) return;
    auto currPose = Pose(m_poseEstimator->getPendingPoseResult().m_points);
    currPose.debugPrint();
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


void VideoPreviewPopup::onModeButton(CCObject* btn) {
    auto toggler = static_cast<CCMenuItemToggler*>(btn);
    bool newVal = !toggler->isToggled();
    if (newVal) { // this toggler will be selected
        if (auto sel = toggler->getParent()->getChildByID("X")) {
            static_cast<CCMenuItemToggler*>(sel)->toggle(false);
            sel->setID("");
        }
        m_selectedGameMode = (IconType) toggler->getTag();
        toggler->setID("X");
    } else { // all togglers will be deselected
        m_selectedGameMode = std::nullopt;
    }
}


void VideoPreviewPopup::onPlayButton(CCObject* btn) {
    if (auto infoLayer = CCScene::get()->getChildByType<LevelInfoLayer>(0)) {
        onClose(nullptr);
        reinterpret_cast<MyLevelInfoLayer*>(infoLayer)->onPlayRaZooM(m_cameraMan);
    } else if (auto infoLayer = CCScene::get()->getChildByType<EditLevelLayer>(0)) {
        onClose(nullptr);
        reinterpret_cast<MyEditLevelLayer*>(infoLayer)->onPlayRaZooM(m_cameraMan);
    }
}
