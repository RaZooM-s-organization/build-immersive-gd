#pragma once


#include <Geode/Geode.hpp>


using namespace geode::prelude;


enum class VideoOutputLayer {
    BG = 1,
    B5, B4, B3, B2, B1, B0 // todo
};



struct Settings {

    struct {
        std::string m_deviceName;
        float m_qualityMultiplier;
    } m_videoCapture;

    struct {
        bool m_enable;
        int m_fpsLimit;
        VideoOutputLayer m_layer;
        bool m_noFace;
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
        float m_sharpness;
    } m_greenScreen;



    void pull() {

    }


    static Settings& get() {
        static Settings instance;
        return instance;
    }
};