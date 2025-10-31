#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "../settings/Settings.hpp"


using namespace geode::prelude;


class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {

    struct Fields {
        IconType m_currentMode = IconType::Cube;
    };

    void setupLevelStart(LevelSettingsObject* p0) {
        GJBaseGameLayer::setupLevelStart(p0);
        if (p0) {
            m_fields->m_currentMode = (IconType) p0->m_startMode;
        }
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        // button: 1-up, 2-left, 3-right
        if (!ModGlobal::get().m_shouldBlockInputsInBaseGameLayer) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
    }

    void playerWillSwitchMode(PlayerObject* p0, GameObject* p1) {
        if (p0 == m_player1) {
            switch (p1->m_objectID) {
                case 12: m_fields->m_currentMode = IconType::Cube; break;
                case 13: m_fields->m_currentMode = IconType::Ship; break;
                case 47: m_fields->m_currentMode = IconType::Ball; break;
                case 111: m_fields->m_currentMode = IconType::Ufo; break;
                case 660: m_fields->m_currentMode = IconType::Wave; break;
                case 745: m_fields->m_currentMode = IconType::Robot; break;
                case 1331: m_fields->m_currentMode = IconType::Spider; break;
                case 1933: m_fields->m_currentMode = IconType::Swing; break;
            }
        }
        GJBaseGameLayer::playerWillSwitchMode(p0, p1);
    }
};