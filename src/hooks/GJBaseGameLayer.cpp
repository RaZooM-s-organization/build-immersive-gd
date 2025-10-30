#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "../settings/Settings.hpp"


using namespace geode::prelude;


class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
    
    void handleButton(bool down, int button, bool isPlayer1) {
        // button: 1-up, 2-left, 3-right
        if (!ModGlobal::get().m_shouldBlockInputsInBaseGameLayer) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
    }
};