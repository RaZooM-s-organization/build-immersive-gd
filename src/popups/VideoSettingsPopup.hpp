#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/ui/GeodeUI.hpp>


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
        

        setID("video-settings-popup"_spr);
        return true;
    }



    void onClose(CCObject* obj) override {
        Popup::onClose(obj);
    }

};


