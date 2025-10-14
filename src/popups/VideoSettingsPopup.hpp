#pragma once

#include "../utils/ChoiceNode.hpp"
#include "../videoTools/VideoTools.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class VideoSettingsPopup : public Popup<int> {
protected:

    const float m_width = 430.f;
    const float m_height = 290.f;
    
    VideoPlayer* m_videoplayer{};
    
public:

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
        setID("video-settings-popup"_spr);

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

        setupCameraPreview();


        // auto mc = ChoiceNode::create(100, "select the number", {
        //     {1, "1111111"},
        //     {2, "22222222222222"},
        //     {3, "3333333333333333333"},
        //     {4, "44"}
        // }, [](int i){log::debug("choice = {}", i);});
        
        // m_mainLayer->addChildAtPosition(mc, Anchor::Center);
        

        return true;
    }


    void setupCameraPreview() {
        m_videoplayer = VideoPlayer::create(1.f, ClippingMode::Crop);
        auto winSz = CCDirector::get()->getWinSize();
        float ratio = winSz.height / winSz.width;
        // width is fixed, height is adaptive
        float w = m_mainLayer->getContentWidth() / 2.5;
        float h = w * ratio;
        float posX = m_mainLayer->getContentWidth() * 0.75;
        float posY = m_mainLayer->getContentHeight() - h / 2 - 60;
        m_videoplayer->setContentSize({w,h});
        m_mainLayer->addChild(m_videoplayer);
        m_videoplayer->setPosition({posX, posY});

        // title
        auto lbl = CCLabelBMFont::create("Video Preview", "bigFont.fnt");
        m_mainLayer->addChild(lbl);
        lbl->setPosition({posX, posY + h / 2 + 15});
        lbl->setScale(0.5);
    }



    void onClose(CCObject* obj) override {
        Popup::onClose(obj);
    }

};


