#pragma once

#include "../utils/ChoiceNode.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class VideoSettingsPopup : public Popup<int> {
public:
    const float m_width = 210.f;
    const float m_height = 200.f;


    static VideoSettingsPopup* create() {
        auto ret = new VideoSettingsPopup();
        if (ret && ret->initAnchored(ret->m_width, ret->m_height, 0)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }


    bool setup(int) override {
        m_closeBtn->setVisible(false);
        setTitle("Video Settings");

        m_buttonMenu->addChildAtPosition(
            InfoAlertButton::create(
                "Video Settings", 
                "todo", 
                0.75
            ), 
            Anchor::TopRight, ccp(-18, -18)
        );

        m_buttonMenu->addChildAtPosition(
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create("ok", "goldFont.fnt", "GJ_button_01.png", 1),
                this, menu_selector(VideoSettingsPopup::onClose)
            ),
            Anchor::Bottom
        );


        auto mc = ChoiceNode::create(100, {
            {1, "1111111"},
            {2, "22222222222222"},
            {3, "3333333333333333333"},
            {4, "44"}
        }, [](int i){log::debug("choice = {}", i);});
        m_mainLayer->addChildAtPosition(mc, Anchor::Center);
        

        setID("video-settings-popup"_spr);
        return true;
    }



    void onClose(CCObject* obj) override {
        Popup::onClose(obj);
    }

};


