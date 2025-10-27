#include "../videoTools/CameraMan.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/cocos.hpp>


class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_myPlayBtn{};
        std::shared_ptr<CameraMan> m_cameraMan{};
        bool m_shouldEnableTheModOnPlayLayer{};
    };

    bool init(GJGameLevel* level, bool challenge);

    void onPlay(CCObject* sender);

    void onOpenPopup(CCObject*);

    // dont destroy cameraman to skip his long initialization
    void onPlayRaZooM(std::shared_ptr<CameraMan> optionalCameraMan);
};