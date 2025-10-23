#include "CameraMan.hpp"


CameraMan::CameraMan(std::string deviceName, float qualityMultiplier) :
    m_deviceName(deviceName),
    m_qualityMultiplier(qualityMultiplier)
{}


CameraMan::~CameraMan() {
    if (m_secondThread.joinable()) {
        m_secondThreadRunning = false;
        m_secondThread.join();
    }
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    sws_free_context(&sws_ctx);
}


std::shared_ptr<uint8_t> make_shared_av_buffer(size_t sz) {
    auto ptr = (uint8_t*) av_malloc(sz);
    auto deleter = [](uint8_t* p){av_free(p);};
    return std::shared_ptr<uint8_t>(ptr, deleter);
}


Result<void, std::string> CameraMan::setupFFMPEG() {

    if (m_deviceName.empty()) {
        return Err("Device name can't be empty");
    }
    const char* device_name = ("video=" + m_deviceName).c_str();

    avdevice_register_all();
    
    const AVInputFormat* input_fmt = av_find_input_format("dshow");
    if (input_fmt == nullptr) {
        return Err("'dshow' format not found");
    }

    if (avformat_open_input(&fmt_ctx, device_name, input_fmt, nullptr) != 0) {
        return Err("Not able to open the device " + m_deviceName);
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
    
    m_outputW = int(codec_ctx->width * m_qualityMultiplier);
    m_outputH = int(codec_ctx->height * m_qualityMultiplier);
    
    sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, 
        codec_ctx->pix_fmt,
        m_outputW, m_outputH, 
        AV_PIX_FMT_RGB24, SWS_BILINEAR, 
        nullptr, nullptr, nullptr
    );
    if (!sws_ctx) {
        return Err("Failed to create sws context");
    }
    
    pkt = av_packet_alloc();
    frame = av_frame_alloc();
    rgb_frame = av_frame_alloc();
    
    m_rgbBufSz = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_outputW, m_outputH, 1);

    m_secondThread = std::thread(&CameraMan::processVideoStream, this);
    
    return Ok();
}



void CameraMan::processVideoStream() {
    m_secondThreadRunning = true;
    while(m_secondThreadRunning) {
        // avg is ~1.8 ms on 640x480, x1 multiplier
        bool success = processFrame();
        std::this_thread::yield();
    }
}


CameraFrame CameraMan::getPendingFrame() {
    m_latestFrameOwnership.lock();
    auto copy = m_latestFrame;
    m_latestFrameOwnership.unlock();
    return std::move(copy);
}


void CameraMan::getFrameSizeInPix(int *pixW, int *pixH) {
    *pixW = m_outputW;
    *pixH = m_outputH;
}


bool CameraMan::processFrame() {
    bool success = false;
    if (av_read_frame(fmt_ctx, pkt) == 0) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                if (avcodec_receive_frame(codec_ctx, frame) == 0) {
    
                    std::shared_ptr<uint8_t> rgb_buffer_tmp = make_shared_av_buffer(m_rgbBufSz);
                    
                    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, 
                        rgb_buffer_tmp.get(), AV_PIX_FMT_RGB24, m_outputW, m_outputH, 1);
    
                    // Convert
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0,
                        codec_ctx->height, rgb_frame->data, rgb_frame->linesize);

                    // update latest frame
                    m_latestFrameOwnership.lock();
                    m_latestFrame.m_width = m_outputW;
                    m_latestFrame.m_height = m_outputH;
                    m_latestFrame.m_data = rgb_buffer_tmp;
                    m_latestFrame.m_id++;
                    m_latestFrameOwnership.unlock();

                    success = true;
                }
            }
        }
        av_packet_unref(pkt);
    }
    return success;
}