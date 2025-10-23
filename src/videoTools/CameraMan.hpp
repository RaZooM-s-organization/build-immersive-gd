#pragma once

#include "VideoTools.hpp"

extern "C" {
// #include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
// #include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
// #include <libswresample/swresample.h>
#include <libswscale/swscale.h>
// #include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#include <thread>
#include <atomic>
#include <mutex>

// todo: seems that I don't use avfilter, so I can remove that 24MB dll from resources
// (and the same thing for other unused dlls)



/**
 * This class is responsible for capturing and decoding a videostream
 * from the camera, and providing prepared RGB frame data.
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
    std::mutex m_latestFrameOwnership{};

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

