#include "../popups/VideoSettingsPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;



// todo: rm this

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // auto vp = VideoPlayer::create();
        // vp->setPosition(CCDirector::get()->getWinSize() / 2);
        // this->addChild(vp);

        


        return true;
    }

    void onMoreGames(CCObject* sender) {
        
        VideoSettingsPopup::create()->show();
    }

};