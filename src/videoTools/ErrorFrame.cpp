#include "ErrorFrame.hpp"

ErrorFrame* ErrorFrame::create(std::string errTxt) {
    auto ret = new ErrorFrame;
    if (ret && ret->init(std::move(errTxt))) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ErrorFrame::init(std::string errTxt) {
    if (!CCSprite::initWithSpriteFrameName("diffIcon_00_btn_001.png")) return false;
    m_errText = std::move(errTxt);
    setOpacity(90);
    auto menu = CCMenu::create();
    auto spr = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
    auto lbl = CCLabelBMFont::create("Error: ", "bigFont.fnt");
    lbl->setScale(0.3);
    spr->setScale(0.2);
    auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ErrorFrame::onBtn));
    menu->setContentSize({lbl->getScaledContentWidth() + spr->getScaledContentWidth(), 0.1});
    menu->addChild(lbl);
    menu->addChild(btn);
    lbl->setPosition({lbl->getScaledContentWidth() / 2, 1});
    btn->setPosition({lbl->getScaledContentWidth() + spr->getScaledContentWidth() / 2, 0});
    menu->setScale(0.4);
    addChild(menu);
    menu->setPosition(getContentSize()/2);
    menu->ignoreAnchorPointForPosition(false);
    return true;
}

void ErrorFrame::onBtn(CCObject* sender) {
    createQuickPopup("Error Info", m_errText, "ok", nullptr, 300, nullptr);
}
