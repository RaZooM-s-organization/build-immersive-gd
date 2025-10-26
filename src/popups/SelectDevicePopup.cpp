#include "SelectDevicePopup.hpp"

#include "../videoTools/CameraMan.hpp"
#include "../utils/ChoiceNode.hpp"


SelectDevicePopup* SelectDevicePopup::create(SelectDevicePopupData data) {
    auto ret = new SelectDevicePopup();
    if (ret && ret->initAnchored(ret->m_width, ret->m_height, data)) {
        ret->autorelease();
        return ret; 
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool SelectDevicePopup::setup(SelectDevicePopupData data) {
    m_closeBtn->setVisible(false);
    setTitle("Select Video Device");
    setID("select-device-popup"_spr);

    // m_buttonMenu->addChildAtPosition(
    //     InfoAlertButton::create(
    //         "Info", 
    //         "todo", 
    //         0.75
    //     ), 
    //     Anchor::TopRight, ccp(-18, -18)
    // );

    m_buttonMenu->addChildAtPosition(
        CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ok", "goldFont.fnt", "GJ_button_01.png", 1),
            this, menu_selector(SelectDevicePopup::onClose)
        ),
        Anchor::Bottom
    );

    m_data = data;

    std::vector<std::string> cameras = CameraMan::getAvailableCameras();
    if (cameras.empty()) {
        auto errLbl = CCLabelBMFont::create("No devices found :(", "bigFont.fnt");
        m_mainLayer->addChildAtPosition(errLbl, Anchor::Center);
        errLbl->setScale(0.6);
    } else {
        cameras.insert(cameras.begin(), "(auto)"); // add 'auto' option
        m_mainLayer->addChildAtPosition(
            ChoiceNode::create(
                130, nullptr, cameras, 
                [this](std::string s){
                    m_data.m_current = s;
                }, 
                m_data.m_current, true
            ),
            Anchor::Center
        );
    }

    return true;
}


void SelectDevicePopup::onClose(CCObject* obj) {
    if (m_data.m_callback) {
        m_data.m_callback(m_data.m_current);
    }
    Popup::onClose(obj);
}
