#pragma once


#include <Geode/Geode.hpp>


using namespace geode::prelude;


enum class VideoOutputLayer {
    BG, B5, B4, B3, B2, B1, B0, T1, T2, T3, T4
};



struct ModSettings {

    struct {
        std::string m_deviceName;
        float m_qualityMultiplier;
    } m_videoCapture;

    struct {
        bool m_enable;
        int m_fpsLimit;
        VideoOutputLayer m_layer;
    } m_videoOutput;

    struct {
        bool m_enable;
        int m_numOfThreads;
        int m_fpsLimit;
        bool m_useGPU;
        bool m_debugDraw;
    } m_poseEstimation;

    struct {
        bool m_enable;
        ccColor3B m_color;
        float m_softness;
        float m_edgeBlur;
    } m_greenScreen;


    void pull() {
        auto mod = Mod::get();

        // Video Capture Settings
        if (auto sett = std::static_pointer_cast<SettingBaseValueV3<std::string>>(mod->getSetting("device-name"))) {
            m_videoCapture.m_deviceName = sett->getValue();
            if (m_videoCapture.m_deviceName == "(auto)") m_videoCapture.m_deviceName = "";
        }
        m_videoCapture.m_qualityMultiplier = mod->getSettingValue<float>("quality-multiplier");

        // Video Output Settings
        m_videoOutput.m_enable = mod->getSettingValue<bool>("video-output-enable");
        m_videoOutput.m_fpsLimit = mod->getSettingValue<int>("video-output-fps-limit");
        std::map<std::string, VideoOutputLayer> layer2enum = {
            {"BG", VideoOutputLayer::BG},
            {"B5", VideoOutputLayer::B5},
            {"B4", VideoOutputLayer::B4},
            {"B3", VideoOutputLayer::B3},
            {"B2", VideoOutputLayer::B2},
            {"B1", VideoOutputLayer::B1},
            {"B0", VideoOutputLayer::B0},
            {"T1", VideoOutputLayer::T1},
            {"T2", VideoOutputLayer::T2},
            {"T3", VideoOutputLayer::T3},
            {"T4", VideoOutputLayer::T4}
        };
        auto it = layer2enum.find(mod->getSettingValue<std::string>("video-output-layer"));
        m_videoOutput.m_layer = (it == layer2enum.end()) ? VideoOutputLayer::BG : it->second;

        // Pose Estimation Settings
        m_poseEstimation.m_enable = mod->getSettingValue<bool>("pose-estimation-enable");
        m_poseEstimation.m_numOfThreads = mod->getSettingValue<int>("pose-estimation-num-of-threads");
        m_poseEstimation.m_fpsLimit = mod->getSettingValue<int>("pose-estimation-video-output-fps-limit");
        m_poseEstimation.m_useGPU = mod->getSettingValue<bool>("pose-estimation-use-gpu");
        m_poseEstimation.m_debugDraw = mod->getSettingValue<bool>("pose-estimation-debug-draw");

        // Green Screen Settings
        m_greenScreen.m_enable = mod->getSettingValue<bool>("green-screen-enable");
        m_greenScreen.m_color = mod->getSettingValue<ccColor3B>("green-screen-color");
        m_greenScreen.m_softness = mod->getSettingValue<float>("green-screen-tolerance");
        m_greenScreen.m_edgeBlur = mod->getSettingValue<float>("green-screen-edge-blur");
    }


    static ModSettings& get() {
        static ModSettings instance;
        return instance;
    }
};