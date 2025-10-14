#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class ChoiceNode : public CCMenu {
protected:
    CCMenuItemSpriteExtra* m_leftBtn{};
    CCMenuItemSpriteExtra* m_rightBtn{};
    Ref<CCArray> m_labels{};
    CCLabelBMFont* m_selectedLabel{};
    std::function<void(int)> m_callback{};

public:

    static ChoiceNode* create(
        float width, const char* title,
        const std::vector<std::pair<int, std::string>> &choice,
        std::function<void(int)> callback,
        std::optional<int> defaultChoiceTag = std::nullopt, 
        bool triggerCallback = false
    );

protected:

    bool init(
        float width, const char* title,
        const std::vector<std::pair<int, std::string>> &choice,
        std::function<void(int)> callback,
        std::optional<int> defaultChoiceTag, 
        bool triggerCallback
    );

    void onClick(CCObject* btn);

    void updateArrows();

};
