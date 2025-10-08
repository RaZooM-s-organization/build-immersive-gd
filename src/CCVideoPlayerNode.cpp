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




// std::string const vert_shader_source = 
// 	"#version 150\n"
// 	"in vec3 vertex;\n"
// 	"in vec2 texCoord0;\n"
// 	"uniform mat4 mvpMatrix;\n"
// 	"out vec2 texCoord;\n"
// 	"void main() {\n"
// 	"	gl_Position = mvpMatrix * vec4(vertex, 1.0);\n"
// 	"	texCoord = texCoord0;\n"
// 	"}\n";

// std::string const frag_shader_source =
// 	"#version 150\n"
// 	"uniform sampler2D frameTex;\n"
// 	"in vec2 texCoord;\n"
// 	"out vec4 fragColor;\n"
// 	"void main() {\n"
// 	"	fragColor = texture(frameTex, texCoord);\n"
// 	"}\n";

// #define BUFFER_OFFSET(i) ((char *)NULL + (i))

// // attribute indices
// enum {
// 	VERTICES = 0,
// 	TEX_COORDS	
// };

// // uniform indices
// enum {
// 	MVP_MATRIX = 0,
// 	FRAME_TEX
// };


// app data structure
// typedef struct {
// 	AVFormatContext *fmt_ctx;
// 	int stream_idx;
// 	AVStream *video_stream;
// 	AVCodecContext *codec_ctx;
// 	AVCodec *decoder;
// 	AVPacket *packet;
// 	AVFrame *av_frame;
// 	AVFrame *gl_frame;
// 	struct SwsContext *conv_ctx;
// 	GLuint vao;
// 	GLuint vert_buf;
// 	GLuint elem_buf;
// 	GLuint frame_tex;
// 	GLuint program;
// 	GLuint attribs[2];
// 	GLuint uniforms[2];
// } AppData;

// initialize the app data structure
// void initializeAppData(AppData *data) {
// 	data->fmt_ctx = NULL;
// 	data->stream_idx = -1;
// 	data->video_stream = NULL;
// 	data->codec_ctx = NULL;
// 	data->decoder = NULL;
// 	data->av_frame = NULL;
// 	data->gl_frame = NULL;
// 	data->conv_ctx = NULL;
// }

// clean up the app data structure
// void clearAppData(AppData *data) {
// 	if (data->av_frame) av_free(data->av_frame);
// 	if (data->gl_frame) av_free(data->gl_frame);
// 	if (data->packet) av_free(data->packet);
// 	if (data->codec_ctx) avcodec_free_context(&data->codec_ctx);
// 	if (data->fmt_ctx) avformat_free_context(data->fmt_ctx);
// 	glDeleteVertexArrays(1, &data->vao);
// 	glDeleteBuffers(1, &data->vert_buf);
// 	glDeleteBuffers(1, &data->elem_buf);
// 	glDeleteTextures(1, &data->frame_tex);
// 	initializeAppData(data);
// }

// read a video frame
// bool readFrame(AppData *data) {	
// 	do {
// 		if (av_read_frame(data->fmt_ctx, data->packet) < 0) {
//             av_packet_unref(data->packet);
// 			return false;
// 		}
	
// 		if (data->packet->stream_index == data->stream_idx) {
// 			int frame_finished = 0;

//             if (avcodec_send_packet(data->codec_ctx, data->packet)) {
//                 av_packet_unref(data->packet);
//                 return false;
//             }
//             if (avcodec_receive_frame(data->codec_ctx, data->av_frame)) {
//                 av_packet_unref(data->packet);
//                 return false;
//             }
		
// 			if (frame_finished) {
// 				if (!data->conv_ctx) {
// 					data->conv_ctx = sws_getContext(data->codec_ctx->width, 
// 						data->codec_ctx->height, data->codec_ctx->pix_fmt, 
// 						data->codec_ctx->width, data->codec_ctx->height, AV_PIX_FMT_RGB24,
// 						SWS_BICUBIC, NULL, NULL, NULL);
// 				}
			
// 				sws_scale(data->conv_ctx, data->av_frame->data, data->av_frame->linesize, 0, 
// 					data->codec_ctx->height, data->gl_frame->data, data->gl_frame->linesize);
					
// 				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->codec_ctx->width, 
// 					data->codec_ctx->height, GL_RGB, GL_UNSIGNED_BYTE, 
// 					data->gl_frame->data[0]);
// 			}
// 		}
		
// 		av_packet_unref(data->packet);
        
// 	} while (data->packet->stream_index != data->stream_idx);
	
// 	return true;
// }

// bool buildShader(std::string const &shader_source, GLuint &shader, GLenum type) {
// 	int size = shader_source.length();
	
// 	shader = glCreateShader(type);
// 	char const *c_shader_source = shader_source.c_str();
// 	glShaderSource(shader, 1, (GLchar const **)&c_shader_source, &size);
// 	glCompileShader(shader);
// 	GLint status;
// 	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
// 	if (status != GL_TRUE) {
// 		std::cout << "failed to compile shader" << std::endl;
// 		int length;
// 		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
// 		char *log = new char[length];
// 		glGetShaderInfoLog(shader, length, &length, log);
// 		std::cout << log << std::endl;
// 		delete[] log;
// 		return false;
// 	}
	
// 	return true;
// }

// initialize shaders
// bool buildProgram(AppData *data) {
// 	GLuint v_shader, f_shader;
// 	if (!buildShader(vert_shader_source, v_shader, GL_VERTEX_SHADER)) {
// 		std::cout << "failed to build vertex shader" << std::endl;
// 		return false;
// 	}
	
// 	if (!buildShader(frag_shader_source, f_shader, GL_FRAGMENT_SHADER)) {
// 		std::cout << "failed to build fragment shader" << std::endl;
// 		return false;
// 	} 
	
// 	data->program = glCreateProgram();
// 	glAttachShader(data->program, v_shader);
// 	glAttachShader(data->program, f_shader);
// 	glLinkProgram(data->program);
// 	GLint status;
// 	glGetProgramiv(data->program, GL_LINK_STATUS, &status);
// 	if (status != GL_TRUE) {
// 		std::cout << "failed to link program" << std::endl;
// 		int length;
// 		glGetProgramiv(data->program, GL_INFO_LOG_LENGTH, &length);
// 		char *log = new char[length];
// 		glGetShaderInfoLog(data->program, length, &length, log);
// 		std::cout << log << std::endl;
// 		delete[] log;
// 		return false;
// 	}
	
// 	data->uniforms[MVP_MATRIX] = glGetUniformLocation(data->program, "mvpMatrix");
// 	data->uniforms[FRAME_TEX] = glGetUniformLocation(data->program, "frameTex");
	
// 	data->attribs[VERTICES] = glGetAttribLocation(data->program, "vertex");
// 	data->attribs[TEX_COORDS] = glGetAttribLocation(data->program, "texCoord0");
		
// 	return true;
// }


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
    CCTexture2D* cc_texture{};

public:

    static CCVideoPlayerNode* create() {
        auto ret = new CCVideoPlayerNode;
        if (ret && ret->init()) {
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

        CC_SAFE_DELETE(cc_texture);

    }


    bool init() override {

        // const char* filename = "C:/Users/79215/Desktop/sample.mp4";

        // fmt_ctx = nullptr;
        // if (avformat_open_input(&fmt_ctx, filename, nullptr, nullptr) < 0) {
        //     std::cerr << "Could not open file\n";
        //     return true;
        // }
        // if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        //     std::cerr << "Could not find stream info\n";
        //     return true;
        // }
        // video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        // if (video_stream_index < 0) {
        //     std::cerr << "No video stream found\n";
        //     return true;
        // }
        // AVStream* video_stream = fmt_ctx->streams[video_stream_index];
        // const AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
        // if (!codec) {
        //     std::cerr << "Unsupported codec\n";
        //     return true;
        // }
        // codec_ctx = avcodec_alloc_context3(codec);
        // avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);
        // if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        //     std::cerr << "Could not open codec\n";
        //     return true;
        // }

        avdevice_register_all();

        const AVInputFormat* input_fmt = av_find_input_format("dshow");
        AVDictionary* options = nullptr;
        const char* device_name = "video=Live Streamer CAM 313";

        if (avformat_open_input(&fmt_ctx, device_name, input_fmt, &options) != 0) {
            std::cerr << "Не удалось открыть устройство!" << std::endl;
            return false;
        }

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            std::cerr << "Не удалось получить информацию о потоке!" << std::endl;
            return false;
        }


        video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (video_stream_index < 0) {
            std::cerr << "No video stream found\n";
            return true;
        }
        AVStream* video_stream = fmt_ctx->streams[video_stream_index];
        const AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
        if (!codec) {
            std::cerr << "Unsupported codec\n";
            return true;
        }
        codec_ctx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);
        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            std::cerr << "Could not open codec\n";
            return true;
        }

        fmt_ctx->flags |= AVFMT_FLAG_NONBLOCK; // dont block to 24 frames


        sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
            codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (!sws_ctx) {
            std::cerr << "Failed to create sws context\n";
            return true;
        }

        pkt = av_packet_alloc();
        frame = av_frame_alloc();
        rgb_frame = av_frame_alloc();

        int rgb_buf_sz = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
        rgb_buffer = (uint8_t*)av_malloc(rgb_buf_sz);

        av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, 
            rgb_buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);

        cc_texture = new CCTexture2D;
        if (cc_texture && cc_texture->initWithData(
            rgb_frame->data[0],
            kCCTexture2DPixelFormat_RGB888,
            codec_ctx->width,
            codec_ctx->height,
            CCSizeMake(codec_ctx->width, codec_ctx->height)
        )) {
            cc_texture->autorelease();
        } else {
            CC_SAFE_DELETE(cc_texture);
            return false;
        }

        

        
        return CCSprite::initWithTexture(cc_texture);
        // return CCSprite::init();
    }


    void visit() override {
        
        if (av_read_frame(fmt_ctx, pkt) >= 0) {
            if (pkt->stream_index == video_stream_index) {
                if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                    if (avcodec_receive_frame(codec_ctx, frame) == 0) {

                        if (auto curr_texture = getTexture()) {
                            if (auto glid = curr_texture->getName()) {
                                ccGLDeleteTexture(glid);
                            }
                        }

                        // Конвертируем
                        sws_scale(sws_ctx,
                                frame->data, frame->linesize,
                                0, codec_ctx->height,
                                rgb_frame->data, rgb_frame->linesize);

                        

                        bool success = cc_texture->initWithData(
                            rgb_frame->data[0],
                            kCCTexture2DPixelFormat_RGB888,
                            codec_ctx->width,
                            codec_ctx->height,
                            CCSizeMake(codec_ctx->width, codec_ctx->height)
                        );

                        
                        setTexture(cc_texture);
                        // setTextureRect(CCRect(0,0,480,270));
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
        vp->setContentSize({480, 270});
        vp->setPosition(CCDirector::get()->getWinSize() / 2);
        this->addChild(vp);

        return true;
    }
};