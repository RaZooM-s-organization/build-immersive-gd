#include "VideoPreviewPopup.hpp"

#include "../videoTools/ErrorFrame.hpp"
#include "../nnTools/PoseEstimator.hpp"
#include "../settings/Settings.hpp"
#include "../hooks/LevelInfoLayer.hpp"

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

    m_cameraMan = std::make_shared<CameraMan>();
    auto res = m_cameraMan->setupFFMPEG();

    if (res.isOk()) {
        // add video frame
        m_videoFrame = VideoFrame::create(m_cameraMan);
        m_videoplayer->addFrame(m_videoFrame);
        // add pose m_videoFrame
        auto worker = std::make_shared<PoseEstimator>(m_cameraMan);
        m_poseFrame = PoseFrame::create(worker);
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
    if (CCScene::get()->getChildByType<LevelInfoLayer>(0) == nullptr) {
        playBtn->setVisible(false);
    }


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


    // pose debug labels
    auto dbgBase = CCNodeRGBA::create();
    dbgBase->setPosition(m_videoplayer->getPosition());
    dbgBase->setContentSize(m_videoplayer->getScaledContentSize());
    dbgBase->setAnchorPoint({0.5,0.5});

    m_debugLabel = CCLabelBMFont::create("Cube:", "bigFont.fnt");
    m_debugLabel->setScale(0.3);
    dbgBase->setCascadeOpacityEnabled(true);
    dbgBase->addChild(m_debugLabel);
    dbgBase->setLayout(AxisLayout::create(Axis::Column)
        ->setGrowCrossAxis(true)
        ->setAutoScale(false)
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(AxisAlignment::End)
        ->setCrossAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisLineAlignment(AxisAlignment::Start)
        ->setAxisReverse(true)
        ->setCrossAxisReverse(true)
    );
    dbgBase->setOpacity(100);

    m_mainLayer->addChild(dbgBase, 5);

    schedule(schedule_selector(VideoPreviewPopup::updateLabels), 0.5);
    schedule(schedule_selector(VideoPreviewPopup::updateModeLabels), 0.2);
    
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


void VideoPreviewPopup::updateModeLabels(float) {

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
    }
}
