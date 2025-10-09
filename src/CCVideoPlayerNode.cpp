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



class CCVideoPlayerNode : public CCSprite {
protected:

    AVPacket* pkt{};
    AVFrame* frame{};
    AVFrame* rgb_frame{};
    AVFormatContext* fmt_ctx{};
    int video_stream_index{};
    AVCodecContext* codec_ctx{};
    SwsContext* sws_ctx{};
    uint8_t* rgb_buffer{};

    CCTexture2D cc_texture;

    int output_w;
    int output_h;

public:

    static CCVideoPlayerNode* create(float quality_multiplier = 1.f) {
        auto ret = new CCVideoPlayerNode;
        if (ret && ret->init(quality_multiplier)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    
    ~CCVideoPlayerNode() {
        av_frame_free(&frame);
        av_frame_free(&rgb_frame);
        av_packet_free(&pkt);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        sws_free_context(&sws_ctx);
        av_freep(&rgb_buffer);
    }


    bool init(float quality_multiplier) {

        const char* device_name = "video=Live Streamer CAM 313";
        
        avdevice_register_all();

        const AVInputFormat* input_fmt = av_find_input_format("dshow");
        if (input_fmt == nullptr) {
            log::error("'dshow' format not found");
            return true;
        }

        if (avformat_open_input(&fmt_ctx, device_name, input_fmt, nullptr) != 0) {
            log::error("Not able to open the device '{}'", device_name);
            return true;
        }

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            log::error("Stream info not found");
            return true;
        }

        video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (video_stream_index < 0) {
            log::error("No video stream found");
            return true;
        }

        AVStream* video_stream = fmt_ctx->streams[video_stream_index];
        const AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
        if (!codec) {
            log::error("Unsupported codec");
            return true;
        }

        fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK;

        codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            log::error("Can't allocate codec context");
            return true;
        }

        if (avcodec_parameters_to_context(codec_ctx, video_stream->codecpar) < 0) {
            log::error("Can't initialize codec context");
            return true;
        }

        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            log::error("Could not open codec");
            return true;
        }

        output_w = int(codec_ctx->width * quality_multiplier);
        output_h = int(codec_ctx->height * quality_multiplier);

        sws_ctx = sws_getContext(
            codec_ctx->width, codec_ctx->height, 
            codec_ctx->pix_fmt,
            output_w, output_h, 
            AV_PIX_FMT_RGB24, SWS_BILINEAR, 
            nullptr, nullptr, nullptr
        );
        if (!sws_ctx) {
            log::error("Failed to create sws context");
            return true;
        }

        pkt = av_packet_alloc();
        frame = av_frame_alloc();
        rgb_frame = av_frame_alloc();

        int rgb_buf_sz = av_image_get_buffer_size(AV_PIX_FMT_RGB24, output_w, output_h, 1);
        rgb_buffer = (uint8_t*)av_malloc(rgb_buf_sz);

        av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, 
            rgb_buffer, AV_PIX_FMT_RGB24, output_w, output_h, 1);


        cc_texture.initWithData(
            rgb_frame->data[0],
            kCCTexture2DPixelFormat_RGB888,
            output_w,
            output_h,
            CCSizeMake(output_w, output_h)
        );
        cc_texture.retain();
        
        if (!CCSprite::initWithTexture(&cc_texture)) return false;

        float size_multiplier = 1;
        switch (CCDirector::get()->getLoadedTextureQuality()) {
            case TextureQuality::kTextureQualityLow: size_multiplier = 1; break;
            case TextureQuality::kTextureQualityMedium: size_multiplier = 0.5; break;
            case TextureQuality::kTextureQualityHigh: size_multiplier = 0.25; break;
        }

        setContentSize(CCSizeMake(output_w, output_h) * size_multiplier);

        return true;
    }


    void visit() override {
        if (av_read_frame(fmt_ctx, pkt) == 0) {
            if (pkt->stream_index == video_stream_index) {
                if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                    if (avcodec_receive_frame(codec_ctx, frame) == 0) {

                        if (auto curr_texture = getTexture()) {
                            if (auto glid = curr_texture->getName()) {
                                ccGLDeleteTexture(glid);
                            }
                        }

                        // Convert
                        sws_scale(sws_ctx, frame->data, frame->linesize, 0,
                            codec_ctx->height, rgb_frame->data, rgb_frame->linesize);

                        bool success = cc_texture.initWithData(
                            rgb_frame->data[0],
                            kCCTexture2DPixelFormat_RGB888,
                            output_w,
                            output_h,
                            CCSizeMake(output_w, output_h)
                        );

                        updateBlendFunc();
                    }
                }
            }
            av_packet_unref(pkt);
        }

        CCSprite::visit();
    }

};


class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto vp = CCVideoPlayerNode::create();
        // vp->setContentSize({480, 270});
        vp->setPosition(CCDirector::get()->getWinSize() / 2);
        this->addChild(vp);

        return true;
    }
};