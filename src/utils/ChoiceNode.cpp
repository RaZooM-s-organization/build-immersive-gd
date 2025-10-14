#include "ChoiceNode.hpp"

ChoiceNode* ChoiceNode::create(
    float width, const char* title,
    const std::vector<std::pair<int, std::string>> &choice,
    std::function<void(int)> callback,
    std::optional<int> defaultChoiceTag, 
    bool triggerCallback
) {
    auto ret = new ChoiceNode;
    if (ret && ret->init(width, title, choice, callback, defaultChoiceTag, triggerCallback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ChoiceNode::updateArrows() {
    m_leftBtn->setOpacity(255);
    m_leftBtn->setEnabled(true);
    m_rightBtn->setOpacity(255);
    m_rightBtn->setEnabled(true);
    if (m_selectedLabel == m_labels->firstObject()) {
        m_leftBtn->setOpacity(90);
        m_leftBtn->setEnabled(false);
    }
    if (m_selectedLabel == m_labels->lastObject()) {
        m_rightBtn->setOpacity(90);
        m_rightBtn->setEnabled(false);
    }
}

bool ChoiceNode::init(
    float width, const char* title,
    const std::vector<std::pair<int, std::string>> &choice,
    std::function<void(int)> callback,
    std::optional<int> defaultChoiceTag, 
    bool triggerCallback = false
) {
    if (!CCMenu::init()) return false;

    setContentSize(CCSizeMake(width, 30));
    m_labels = CCArray::create();
    m_callback = callback;

    if (!title) title = "";
    auto titleLbl = CCLabelBMFont::create(title, "goldFont.fnt");
    titleLbl->setScale(0.6);
    addChildAtPosition(titleLbl, Anchor::Top, ccp(0, 15));

    for (auto& [tag, str] : choice) {
        auto lab = CCLabelBMFont::create(str.c_str(), "bigFont.fnt");
        m_labels->addObject(lab);
        lab->setTag(tag);
        lab->limitLabelWidth(width - 5, 0.6, 0.1);
        addChildAtPosition(lab, Anchor::Center);
        lab->setVisible(false);
        if (defaultChoiceTag && defaultChoiceTag.value() == tag) {
            m_selectedLabel = lab;
        }
    }

    if (!m_selectedLabel) {
        m_selectedLabel = static_cast<CCLabelBMFont*>(m_labels->firstObject());
    }
    m_selectedLabel->setVisible(true);

    auto leftSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    leftSprite->setScale(0.8);
    m_leftBtn = CCMenuItemSpriteExtra::create(leftSprite, this, menu_selector(ChoiceNode::onClick));

    auto rightSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    rightSprite->setScale(0.8);
    rightSprite->setFlipX(true);
    m_rightBtn = CCMenuItemSpriteExtra::create(rightSprite, this, menu_selector(ChoiceNode::onClick));

    addChildAtPosition(m_leftBtn, Anchor::Left, ccp(-20, 0));
    addChildAtPosition(m_rightBtn, Anchor::Right, ccp(20, 0));

    // auto bg = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
    // bg->setOpacity(90);
    // bg->setContentSize(getContentSize() * 2);
    // bg->setScale(0.5);
    // addChildAtPosition(bg, Anchor::Center);
    // bg->setZOrder(-1);

    updateArrows();
    if (callback && triggerCallback) callback(m_selectedLabel->getTag());

    return true;
}

void ChoiceNode::onClick(CCObject* btn) {
    int idx = m_labels->indexOfObject(m_selectedLabel) + (btn == m_leftBtn ? -1 : 1);
    idx = std::clamp(idx, 0, (int)m_labels->count() -1);
    m_selectedLabel->setVisible(false);
    m_selectedLabel = static_cast<CCLabelBMFont*>(m_labels->objectAtIndex(idx));
    m_selectedLabel->setVisible(true);
    updateArrows();
    if (m_callback) m_callback(m_selectedLabel->getTag());
}