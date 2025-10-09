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


#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;




// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC



class VideoFrame : public CCSprite {
protected:

    AVPacket* pkt{};
    AVFrame* frame{};
    AVFrame* rgb_frame{};
    AVFormatContext* fmt_ctx{};
    int video_stream_index{};
    AVCodecContext* codec_ctx{};
    SwsContext* sws_ctx{};
    uint8_t* rgb_buffer{};

    CCTexture2D* cc_texture{};

    int output_w;
    int output_h;

    bool correctly_initialized = false;

public:

    static VideoFrame* create(float quality_multiplier = 1.f);
    ~VideoFrame();

    bool init(float quality_multiplier);
    void visit() override;
    bool isOk();


private:

    Result<void, std::string> setupFFMPEG(const char* device_name, float quality_multiplier);
    
};


enum class ClippingMode {
    Crop, Fit
};


class VideoPlayer : public CCNode {
protected:
    VideoFrame* m_videoFrame;
    ClippingMode m_mode;
    CCClippingNode* m_clippingNode;

public:

    static VideoPlayer* create(float qualityMultiplier = 1.f, ClippingMode clippingMode = ClippingMode::Fit);

    bool init(float qualityMultiplier, ClippingMode clippingMode);
    // void updateOutputVideoQuality(float qualityMultiplier);
    void setContentSize(CCSize const& contentSize) override;

private:

    void updateFrameScalePos();


};