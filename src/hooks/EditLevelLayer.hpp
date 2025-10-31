#pragma once

#include "../videoTools/CameraMan.hpp"

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/utils/cocos.hpp>


class $modify(MyEditLevelLayer, EditLevelLayer) {
    struct Fields {
        std::shared_ptr<CameraMan> m_cameraMan{};
        bool m_shouldEnableTheModOnPlayLayer{};
    };

    bool init(GJGameLevel* p0);

    void onOpenPopup(CCObject*);
    
    // dont destroy cameraman to skip his long initialization
    void onPlayRaZooM(std::shared_ptr<CameraMan> optionalCameraMan);

    
};