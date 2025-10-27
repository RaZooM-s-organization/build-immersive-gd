#include "LevelInfoLayer.hpp"
#include "../popups/VideoPreviewPopup.hpp"


bool MyLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
    if (!LevelInfoLayer::init(level, challenge)) return false;

    if (!level || level->isPlatformer()) return true; // no platformer support so far
    
    auto playBtn = static_cast<CCMenuItemSpriteExtra*>(m_playBtnMenu->getChildByID("play-button"));
    playBtn->getNormalImage()->setScale(0.75);
    playBtn->updateSprite();
    playBtn->setPositionX(-35);

    auto myBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    myBtnSpr->setScale(0.75);
    m_fields->m_myPlayBtn = CCMenuItemSpriteExtra::create(
        myBtnSpr, this, menu_selector(MyLevelInfoLayer::onOpenPopup)
    );

    m_playBtnMenu->addChild(m_fields->m_myPlayBtn);
    m_fields->m_myPlayBtn->setPosition({35, playBtn->getPositionY()});

    return true;
}

void MyLevelInfoLayer::onPlay(CCObject* sender) {
    LevelInfoLayer::onPlay(sender);

    if (sender != m_fields->m_myPlayBtn) return;

    auto childrenTmp = m_playSprite->getChildrenExt();
    std::vector<CCNode*> children (childrenTmp.begin(), childrenTmp.end());

    for (auto ch : children) {
        ch->retain();
        ch->removeFromParent();
        m_fields->m_myPlayBtn->getNormalImage()->addChild(ch);
        ch->release();
    }
    static_cast<CCSprite*>(m_fields->m_myPlayBtn->getNormalImage())->setColor(m_playSprite->getColor());
    m_playSprite->setColor(ccc3(255,255,255));
}

void MyLevelInfoLayer::onOpenPopup(CCObject*) {
    if (Mod::get()->getSavedValue("use-camera-agreed", false)) {
        VideoPreviewPopup::create()->show();
        return;
    }
    createQuickPopup("Face Reveal!", 
        "<co>Your camera will now be enabled!</c>\nAre you sure you want to proceed?",
        "Yes", "No",
        [](auto, bool isBtn2) {
            if (!isBtn2) {
                Mod::get()->setSavedValue("use-camera-agreed", true);
                VideoPreviewPopup::create()->show();
            }
        }, 
        true, true
    );
}

void MyLevelInfoLayer::onPlayRaZooM(std::shared_ptr<CameraMan> optionalCameraMan) {
    if (optionalCameraMan) {
        m_fields->m_cameraMan = optionalCameraMan;
    }
    m_fields->m_shouldEnableTheModOnPlayLayer = true;
    onPlay(m_fields->m_myPlayBtn);
}