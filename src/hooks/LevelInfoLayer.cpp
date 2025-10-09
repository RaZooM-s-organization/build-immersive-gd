#include "../popups/VideoSettingsPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;


class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        
        auto menu = getChildByID("play-menu");
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
            this, menu_selector(MyLevelInfoLayer::onOpenPopup)
        );
        // todo: 4x3 screen
        menu->addChild(btn);
        btn->setPosition({-175, 0});
        return true;
    }

    void onOpenPopup(CCObject*) {
        VideoSettingsPopup::create()->show();
    }
};