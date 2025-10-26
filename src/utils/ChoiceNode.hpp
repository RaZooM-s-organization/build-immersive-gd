#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


class ChoiceNode : public CCMenu {
protected:
    CCMenuItemSpriteExtra* m_leftBtn{};
    CCMenuItemSpriteExtra* m_rightBtn{};
    Ref<CCArray> m_labels{};
    CCLabelBMFont* m_selectedLabel{};
    std::function<void(std::string)> m_callback{};
    std::map<int, std::string> m_tag2str{};

public:

    static ChoiceNode* create(
        float width, const char* title,
        const std::vector<std::string> &nonEmptyVec,
        std::function<void(std::string)> callback,
        std::string defaultChoice = "", 
        bool triggerCallback = false
    );

protected:

    bool init(
        float width, const char* title,
        const std::vector<std::string> &nonEmptyVec,
        std::function<void(std::string)> callback,
        std::string defaultChoice, 
        bool triggerCallback
    );

    void onClick(CCObject* btn);

    void updateArrows();

};
