#pragma once

#include "../utils/Utils.hpp"

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
        bool m_disableInputs;
    } m_poseEstimation;

    // struct {
    //     bool m_enable;
    //     ccColor3B m_color;
    //     float m_tolerance;
    //     float m_edgeBlur;
    // } m_greenScreen;


    void pull() {
        auto mod = Mod::get();

        // Video Capture Settings
        if (auto sett = std::static_pointer_cast<SettingBaseValueV3<std::string>>(mod->getSetting("device-name"))) {
            m_videoCapture.m_deviceName = sett->getValue();
            if (m_videoCapture.m_deviceName == "(auto)") m_videoCapture.m_deviceName = "";
        }
        m_videoCapture.m_qualityMultiplier = mod->getSettingValue<double>("quality-multiplier");

        // Video Output Settings
        m_videoOutput.m_enable = mod->getSettingValue<bool>("video-output-enable");
        m_videoOutput.m_fpsLimit = mod->getSettingValue<int64_t>("video-output-fps-limit");
        m_videoOutput.m_layer = str2enum(mod->getSettingValue<std::string>("video-output-layer"));

        // Pose Estimation Settings
        m_poseEstimation.m_enable = mod->getSettingValue<bool>("pose-estimation-enable");
        m_poseEstimation.m_numOfThreads = mod->getSettingValue<int64_t>("pose-estimation-num-of-threads");
        m_poseEstimation.m_fpsLimit = mod->getSettingValue<int64_t>("pose-estimation-video-output-fps-limit");
        // m_poseEstimation.m_useGPU = mod->getSettingValue<bool>("pose-estimation-use-gpu");
        m_poseEstimation.m_useGPU = false;
        m_poseEstimation.m_debugDraw = mod->getSettingValue<bool>("pose-estimation-debug-draw");
        m_poseEstimation.m_disableInputs = mod->getSettingValue<bool>("pose-estimation-disable-inputs");

        // Green Screen Settings
        // m_greenScreen.m_enable = mod->getSettingValue<bool>("green-screen-enable");
        // m_greenScreen.m_color = mod->getSettingValue<ccColor3B>("green-screen-color");
        // m_greenScreen.m_tolerance = mod->getSettingValue<double>("green-screen-tolerance");
        // m_greenScreen.m_edgeBlur = mod->getSettingValue<double>("green-screen-edge-blur");

    }


    static ModSettings& get() {
        static ModSettings instance;
        return instance;
    }
};


struct ModGlobal {
    
    bool m_shouldBlockInputsInBaseGameLayer = false;

    static ModGlobal& get() {
        static ModGlobal instance;
        return instance;
    }
};


/*


"green-screen-title": {
	"type": "title",
	"name": "Green Screen"
},
"green-screen-enable":{
	"type": "bool",
	"name": "Enable",
	"description": "Use the green screen",
	"default": false
},
"green-screen-color": {
	"type": "rgb",
	"name": "Green Screen Color",
	"description": "Color that will be replaced. Online color picker: https://imagecolorpicker.com/",
	"default": [0, 255, 0],
	"enable-if": "green-screen-enable"
},
"green-screen-tolerance": {
	"type": "float",
	"name": "Tolerance",
	"description": "Green screen color +- %",
	"default": 5.0,
	"min": 0.0,
	"max": 100.0,
	"control": {
		"arrows": false,
		"big-arrows": false,
		"slider": true,
		"slider-step": 0.01
	},
	"enable-if": "green-screen-enable"
},
"green-screen-edge-blur": {
	"type": "float",
	"name": "Edge Blur",
	"description": "Edge blur %",
	"default": 5.0,
	"min": 0.0,
	"max": 100.0,
	"control": {
		"arrows": false,
		"big-arrows": false,
		"slider": true,
		"slider-step": 0.01
	},
	"enable-if": "green-screen-enable"
}


"pose-estimation-use-gpu": {
	"type": "bool",
	"name": "Use GPU (only NVIDIA supported)",
	"description": "Use GPU to speed up the neural network computations. \n <co>Don't enable if you don't have an NVIDIA GPU</c>\n<cg>But if you have it, I highly recommend enabling this option!</c>",
	"default": false,
	"enable-if": "pose-estimation-enable"
},


*/