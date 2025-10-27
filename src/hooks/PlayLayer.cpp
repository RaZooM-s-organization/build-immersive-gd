#include "../popups/VideoPreviewPopup.hpp"
#include "LevelInfoLayer.hpp"

#include <Geode/modify/PlayLayer.hpp>


class $modify(MyPlayLayer, PlayLayer) {

    struct Fields {
        std::shared_ptr<CameraMan> m_cameraMan{};
        bool m_enable{};
    };


    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto f = m_fields.self();

        if (auto levelInfo = CCScene::get()->getChildByType<LevelInfoLayer>(0)) {
            auto levelInfoFields = reinterpret_cast<MyLevelInfoLayer*>(levelInfo)->m_fields.self();
            f->m_cameraMan = levelInfoFields->m_cameraMan;
            f->m_enable = levelInfoFields->m_shouldEnableTheModOnPlayLayer;
        }

        if (!f->m_enable) {
            f->m_cameraMan = nullptr;
            return true;
        };

        if (!f->m_cameraMan) {
            f->m_cameraMan = std::make_shared<CameraMan>();
            auto res = f->m_cameraMan->setupFFMPEG();
            if (res.isErr()) {
                log::error("{}", res.unwrapErr());
                f->m_cameraMan = nullptr;
                f->m_enable = false;
                return true;
            }
        }

        // Here we have a working cameraMan


        return true;
    }

    // b is [0..5]
    void hideLayerAndAllBelow(int b) {
        switch (b) {
            case 0: hideB0();
            case 1: hideB1();
            case 2: hideB2();
            case 3: hideB3();
            case 4: hideB4();
            case 5: hideB5();
        }
    }

    void hideB0() {
        // wtf is B0 layer
        m_game2LayerB0->setVisible(false);
        m_gameBlendingLayerB0->setVisible(false);
        m_fireBlendingLayerB0->setVisible(false);
        m_pixelBlendingLayerB0->setVisible(false);
        m_particleBlendingLayerB0->setVisible(false);
        m_game2BlendingLayerB0->setVisible(false);
    }

    void hideB1() {
        m_gameLayerB1->setVisible(false);
        m_gameBlendingLayerB1->setVisible(false);
        m_glowLayerB1->setVisible(false);
        m_specialLayerB1->setVisible(false);
        m_textLayerB1->setVisible(false);
        m_textBlendingLayerB1->setVisible(false);
        m_fireLayerB1->setVisible(false);
        m_fireBlendingLayerB1->setVisible(false);
        m_pixelLayerB1->setVisible(false);
        m_pixelBlendingLayerB1->setVisible(false);
        m_particleLayerB1->setVisible(false);
        m_particleBlendingLayerB1->setVisible(false);
        m_game2LayerB1->setVisible(false);
        m_game2BlendingLayerB1->setVisible(false);
    }

    void hideB2() {
        m_gameLayerB2->setVisible(false);
        m_gameBlendingLayerB2->setVisible(false);
        m_glowLayerB2->setVisible(false);
        m_specialLayerB2->setVisible(false);
        m_textLayerB2->setVisible(false);
        m_textBlendingLayerB2->setVisible(false);
        m_fireLayerB2->setVisible(false);
        m_fireBlendingLayerB2->setVisible(false);
        m_pixelLayerB2->setVisible(false);
        m_pixelBlendingLayerB2->setVisible(false);
        m_particleLayerB2->setVisible(false);
        m_particleBlendingLayerB2->setVisible(false);
        m_game2LayerB2->setVisible(false);
        m_game2BlendingLayerB2->setVisible(false);
    }


    void hideB3() {
        m_gameLayerB3->setVisible(false);
        m_gameBlendingLayerB3->setVisible(false);
        m_glowLayerB3->setVisible(false);
        m_specialLayerB3->setVisible(false);
        m_textLayerB3->setVisible(false);
        m_textBlendingLayerB3->setVisible(false);
        m_fireLayerB3->setVisible(false);
        m_fireBlendingLayerB3->setVisible(false);
        m_pixelLayerB3->setVisible(false);
        m_pixelBlendingLayerB3->setVisible(false);
        m_particleLayerB3->setVisible(false);
        m_particleBlendingLayerB3->setVisible(false);
        m_game2LayerB3->setVisible(false);
        m_game2BlendingLayerB3->setVisible(false);
    }

    void hideB4() {
        m_gameLayerB4->setVisible(false);
        m_gameBlendingLayerB4->setVisible(false);
        m_glowLayerB4->setVisible(false);
        m_specialLayerB4->setVisible(false);
        m_textLayerB4->setVisible(false);
        m_textBlendingLayerB4->setVisible(false);
        m_fireLayerB4->setVisible(false);
        m_fireBlendingLayerB4->setVisible(false);
        m_pixelLayerB4->setVisible(false);
        m_pixelBlendingLayerB4->setVisible(false);
        m_particleLayerB4->setVisible(false);
        m_particleBlendingLayerB4->setVisible(false);
        m_game2LayerB4->setVisible(false);
        m_game2BlendingLayerB4->setVisible(false);
    }

    void hideB5() {
        m_gameLayerB5->setVisible(false);
        m_gameBlendingLayerB5->setVisible(false);
        m_glowLayerB5->setVisible(false);
        m_specialLayerB5->setVisible(false);
        m_textLayerB5->setVisible(false);
        m_textBlendingLayerB5->setVisible(false);
        m_fireLayerB5->setVisible(false);
        m_fireBlendingLayerB5->setVisible(false);
        m_pixelLayerB5->setVisible(false);
        m_pixelBlendingLayerB5->setVisible(false);
        m_particleLayerB5->setVisible(false);
        m_particleBlendingLayerB5->setVisible(false);
        m_game2LayerB5->setVisible(false);
        m_game2BlendingLayerB5->setVisible(false);
    }


};