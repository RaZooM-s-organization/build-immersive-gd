#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

// todo: seems that I don't use avfilter, so I can remove that 24MB dll from resources
// (and the same thing for other unused dlls)


#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>


using namespace geode::prelude;




// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC




// Frame format is strictly RGB
struct CameraFrame {
    uint32_t m_id = 0;
    uint32_t m_width;
    uint32_t m_height;
    std::shared_ptr<uint8_t> m_data;
};


/**
 * This class is responsible for capturing and decoding a videostream from the camera, 
 * and providing prepared RGB frame data of requested size.
 * 
 * CameraMan uses global CameraSettings
 */
class CameraMan {
protected:

    AVPacket* pkt{};
    AVFrame* frame{};
    AVFrame* rgb_frame{};
    AVFormatContext* fmt_ctx{};
    int video_stream_index{};
    AVCodecContext* codec_ctx{};
    SwsContext* sws_ctx{};


    std::string m_deviceName{};
    float m_qualityMultiplier{1.f};

    int m_outputW{};
    int m_outputH{};
    int m_rgbBufSz{};

    std::thread m_secondThread{};
    std::atomic<bool> m_secondThreadRunning{};
    std::mutex m_latestFrameOwnership;

    CameraFrame m_latestFrame;

    
public:
    
    static std::vector<std::string> getAvailableCameras();

    CameraMan() = delete;
    CameraMan(std::string deviceName, float qualityMultiplier = 1.f);
    ~CameraMan();

    CameraFrame getPendingFrame();
    void getFrameSizeInPix(int *pixW, int *pixH);

    Result<void, std::string> setupFFMPEG();

protected:

    bool processFrame();
    void processVideoStream();
};



class VideoFrame : public CCSprite {
protected:

    CCTexture2D* m_ccTexture{};

    std::shared_ptr<CameraMan> m_cameraMan;

    uint32_t m_currentFrameId = 0;

    bool m_initializedCorrectly{};


public:

    static VideoFrame* create(std::shared_ptr<CameraMan> cameraMan);
    ~VideoFrame();

    bool init(std::shared_ptr<CameraMan> cameraMan);
    void visit() override;

};


enum class ClippingMode {
    Crop, Fit
};


class VideoPlayer : public CCNode {
protected:

    ClippingMode m_mode;
    CCClippingNode* m_clippingNode;

public:

    static VideoPlayer* create(ClippingMode clippingMode = ClippingMode::Fit);

    bool init(ClippingMode clippingMode);
    void setContentSize(CCSize const& contentSize) override;

    void addFrame(CCSprite* frame);
    void updateFramesScalePos();

};