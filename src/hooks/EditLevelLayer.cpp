#include "EditLevelLayer.hpp"
#include "../popups/VideoPreviewPopup.hpp"


bool MyEditLevelLayer::init(GJGameLevel* p0) {
    if (!EditLevelLayer::init(p0)) return false;

    if (!p0 || p0->isPlatformer()) return true; // no platformer support so far

    auto menu = getChildByID("level-edit-menu");

    auto myBtnSpr = CCSprite::create("playManBtn.png"_spr);
    // myBtnSpr->setScale(0.75);
    auto btn = CCMenuItemSpriteExtra::create(
        myBtnSpr, this, menu_selector(MyEditLevelLayer::onOpenPopup)
    );
    
    menu->addChild(btn);
    menu->updateLayout();
    
    return true;
}


void MyEditLevelLayer::onPlayRaZooM(std::shared_ptr<CameraMan> optionalCameraMan) {
    if (optionalCameraMan) {
        m_fields->m_cameraMan = optionalCameraMan;
    }
    m_fields->m_shouldEnableTheModOnPlayLayer = true;
    onPlay(nullptr);
}

void MyEditLevelLayer::onOpenPopup(CCObject*) {
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
