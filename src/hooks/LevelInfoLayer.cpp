#include "../popups/VideoPreviewPopup.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;


class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_myBtn;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        
        auto playBtn = static_cast<CCMenuItemSpriteExtra*>(m_playBtnMenu->getChildByID("play-button"));
        playBtn->getNormalImage()->setScale(0.75);
        playBtn->updateSprite();
        playBtn->setPositionX(-35);

        auto myBtnSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        myBtnSpr->setScale(0.75);
        auto myBtn = CCMenuItemSpriteExtra::create(
            myBtnSpr, this, menu_selector(MyLevelInfoLayer::onOpenPopup)
        );
        myBtn->m_pfnSelector = playBtn->m_pfnSelector;
        myBtn->m_pListener = playBtn->m_pListener;

        m_playBtnMenu->addChild(myBtn);
        myBtn->setPosition({35, playBtn->getPositionY()});

        m_fields->m_myBtn = myBtn;

        return true;
    }

    void onPlay(CCObject* sender) {
        LevelInfoLayer::onPlay(sender);

        // todo: 
        if (sender != m_fields->m_myBtn) return;

        auto childrenTmp = m_playSprite->getChildrenExt();
        std::vector<CCNode*> children (childrenTmp.begin(), childrenTmp.end());

        for (auto ch : children) {
            ch->retain();
            ch->removeFromParent();
            m_fields->m_myBtn->getNormalImage()->addChild(ch);
            ch->release();
        }
        static_cast<CCSprite*>(m_fields->m_myBtn->getNormalImage())->setColor(m_playSprite->getColor());
        m_playSprite->setColor(ccc3(255,255,255));
    }

    void onOpenPopup(CCObject*) {
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
};