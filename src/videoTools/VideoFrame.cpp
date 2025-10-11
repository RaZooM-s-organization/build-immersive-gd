#include "VideoTools.hpp"


// TODO: ffmpeg dlls in Geometry Dash folder
// https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC

// todo: memory check



VideoFrame* VideoFrame::create(float quality_multiplier) {
    auto ret = new VideoFrame;
    if (ret && ret->init(quality_multiplier)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


VideoFrame::~VideoFrame() {
    // all functions are null-safe
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    sws_free_context(&sws_ctx);
    av_freep(&rgb_buffer);

    CC_SAFE_RELEASE(cc_texture);

    log::info("Frame cleared");
}


bool VideoFrame::init(float quality_multiplier) {

    const char* device_name = "video=Live Streamer CAM 313"; // todo:

    auto res = setupFFMPEG(device_name, quality_multiplier);
    if (!res) {
        log::error("Setup ffmpeg error: {}", res.err().value_or("(unk)"));
        if (!CCSprite::initWithSpriteFrameName("diffIcon_00_btn_001.png")) return false;
        setOpacity(90);
        return true;
    }
    
    if (!CCSprite::initWithTexture(cc_texture)) return false;

    float size_multiplier = 1;
    switch (CCDirector::get()->getLoadedTextureQuality()) {
        case TextureQuality::kTextureQualityLow: size_multiplier = 1; break;
        case TextureQuality::kTextureQualityMedium: size_multiplier = 0.5; break;
        case TextureQuality::kTextureQualityHigh: size_multiplier = 0.25; break;
    }

    setContentSize(CCSizeMake(output_w, output_h) * size_multiplier);

    correctly_initialized = true;

    return true;
}


Result<void, std::string> VideoFrame::setupFFMPEG(const char* device_name, float quality_multiplier) {

    avdevice_register_all();
    
    const AVInputFormat* input_fmt = av_find_input_format("dshow");
    if (input_fmt == nullptr) {
        return Err("'dshow' format not found");
    }
    
    if (avformat_open_input(&fmt_ctx, device_name, input_fmt, nullptr) != 0) {
        return Err(fmt::format("Not able to open the device '{}'", device_name));
    }
    
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        return Err("Stream info not found");
    }
    
    video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_stream_index < 0) {
        return Err("No video stream found");
    }
    
    AVStream* video_stream = fmt_ctx->streams[video_stream_index];
    const AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (!codec) {
        return Err("Unsupported codec");
    }
    
    fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK;
    
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        return Err("Could not allocate codec context");
    }
    
    if (avcodec_parameters_to_context(codec_ctx, video_stream->codecpar) < 0) {
        return Err("Could not initialize codec context");
    }
    
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        return Err("Could not open codec");
    }

    // todo: все что ниже в отдельую функцию
    
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
        return Err("Failed to create sws context");
    }
    
    pkt = av_packet_alloc();
    frame = av_frame_alloc();
    rgb_frame = av_frame_alloc();
    
    int rgb_buf_sz = av_image_get_buffer_size(AV_PIX_FMT_RGB24, output_w, output_h, 1);
    rgb_buffer = (uint8_t*)av_malloc(rgb_buf_sz);
    
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, 
        rgb_buffer, AV_PIX_FMT_RGB24, output_w, output_h, 1);
    
    
    cc_texture = new CCTexture2D();
    if (!cc_texture || !cc_texture->initWithData(
        rgb_frame->data[0],
        kCCTexture2DPixelFormat_RGB888,
        output_w, output_h,
        CCSizeMake(output_w, output_h)
    )) {
        CC_SAFE_DELETE(cc_texture);
        return Err("Could not init the texture");
    }

    cc_texture->autorelease();
    cc_texture->retain();
    
    return Ok();
}


void VideoFrame::visit() {
    if (correctly_initialized) {
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

                        cc_texture->initWithData(
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
    }
    CCSprite::visit();
}


bool VideoFrame::isOk() {
    return correctly_initialized;
}
