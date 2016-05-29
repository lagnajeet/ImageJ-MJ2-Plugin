#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include <libavutil/motion_vector.h>
#include <libavutil/avconfig.h>
#ifdef __cplusplus
}
#endif
#include <stdio.h>
#include <string>
#include <map>
#include <pthread.h>


#include "main.h"
#define CHECK_ERR(ERR) {if ((ERR)<0) {is_error=1; return NULL; }}
#define CHECK_ERR_INT(ERR) {if ((ERR)<0) {is_error=1;return -1;} }
uint8_t is_error=0;
uint8_t MAX_THREADS=6;
int PIXEL_FORMAT=-1;
char* video_file_name=NULL;
uint32_t total_frames=0;
pthread_t *tid=NULL;
int *frames_decoded=NULL;
uint8_t** tiff_data_array=NULL;
int* tiff_data_size_array=NULL;

char *strtoupper(char *str)
{
    int i = 0;
    char *p = (char*) malloc(strlen(str) * sizeof(char));
    while(str[i])
    {
        if((str[i] >= 'a') && (str[i] <= 'z'))
            p[i] = str[i] + 'A' - 'a';
        else
            p[i] = str[i];
        ++i;
    }
    p[i] = '\0';
    return p;
}

int px_format_to_enum(char* px_format)
{

    if(strcmp(px_format,"NONE")==0) return AV_PIX_FMT_NONE;
    if(strcmp(px_format,"DEFAULT")==0) return AV_PIX_FMT_NONE;
    if(strcmp(px_format,"YUV420P")==0) return AV_PIX_FMT_YUV420P;
    if(strcmp(px_format,"YUYV422")==0) return AV_PIX_FMT_YUYV422;
    if(strcmp(px_format,"RGB24")==0) return AV_PIX_FMT_RGB24;
    if(strcmp(px_format,"BGR24")==0) return AV_PIX_FMT_BGR24;
    if(strcmp(px_format,"YUV422P")==0) return AV_PIX_FMT_YUV422P;
    if(strcmp(px_format,"YUV444P")==0) return AV_PIX_FMT_YUV444P;
    if(strcmp(px_format,"YUV410P")==0) return AV_PIX_FMT_YUV410P;
    if(strcmp(px_format,"YUV411P")==0) return AV_PIX_FMT_YUV411P;
    if(strcmp(px_format,"GRAY8")==0) return AV_PIX_FMT_GRAY8;
    if(strcmp(px_format,"MONOWHITE")==0) return AV_PIX_FMT_MONOWHITE;
    if(strcmp(px_format,"MONOBLACK")==0) return AV_PIX_FMT_MONOBLACK;
    if(strcmp(px_format,"PAL8")==0) return AV_PIX_FMT_PAL8;
    if(strcmp(px_format,"YUVJ420P")==0) return AV_PIX_FMT_YUVJ420P;
    if(strcmp(px_format,"YUVJ422P")==0) return AV_PIX_FMT_YUVJ422P;
    if(strcmp(px_format,"YUVJ444P")==0) return AV_PIX_FMT_YUVJ444P;
#if FF_API_XVMC
    if(strcmp(px_format,"XVMC_MPEG2_MC")==0) return AV_PIX_FMT_XVMC_MPEG2_MC;
    if(strcmp(px_format,"XVMC_MPEG2_IDCT")==0) return AV_PIX_FMT_XVMC_MPEG2_IDCT;
#define AV_PIX_FMT_XVMC AV_PIX_FMT_XVMC_MPEG2_IDCT
#endif
    if(strcmp(px_format,"UYVY422")==0) return AV_PIX_FMT_UYVY422;
    if(strcmp(px_format,"UYYVYY411")==0) return AV_PIX_FMT_UYYVYY411;
    if(strcmp(px_format,"BGR8")==0) return AV_PIX_FMT_BGR8;
    if(strcmp(px_format,"BGR4")==0) return AV_PIX_FMT_BGR4;
    if(strcmp(px_format,"BGR4_BYTE")==0) return AV_PIX_FMT_BGR4_BYTE;
    if(strcmp(px_format,"RGB8")==0) return AV_PIX_FMT_RGB8;
    if(strcmp(px_format,"RGB4")==0) return AV_PIX_FMT_RGB4;
    if(strcmp(px_format,"RGB4_BYTE")==0) return AV_PIX_FMT_RGB4_BYTE;
    if(strcmp(px_format,"NV12")==0) return AV_PIX_FMT_NV12;
    if(strcmp(px_format,"NV21")==0) return AV_PIX_FMT_NV21;

    if(strcmp(px_format,"ARGB")==0) return AV_PIX_FMT_ARGB;
    if(strcmp(px_format,"RGBA")==0) return AV_PIX_FMT_RGBA;
    if(strcmp(px_format,"ABGR")==0) return AV_PIX_FMT_ABGR;
    if(strcmp(px_format,"BGRA")==0) return AV_PIX_FMT_BGRA;

    if(strcmp(px_format,"GRAY16BE")==0) return AV_PIX_FMT_GRAY16BE;
    if(strcmp(px_format,"GRAY16LE")==0) return AV_PIX_FMT_GRAY16LE;
    if(strcmp(px_format,"YUV440P")==0) return AV_PIX_FMT_YUV440P;
    if(strcmp(px_format,"YUVJ440P")==0) return AV_PIX_FMT_YUVJ440P;
    if(strcmp(px_format,"YUVA420P")==0) return AV_PIX_FMT_YUVA420P;
#if FF_API_VDPAU
    if(strcmp(px_format,"VDPAU_H264")==0) return AV_PIX_FMT_VDPAU_H264;
    if(strcmp(px_format,"VDPAU_MPEG1")==0) return AV_PIX_FMT_VDPAU_MPEG1;
    if(strcmp(px_format,"VDPAU_MPEG2")==0) return AV_PIX_FMT_VDPAU_MPEG2;
    if(strcmp(px_format,"VDPAU_WMV3")==0) return AV_PIX_FMT_VDPAU_WMV3;
    if(strcmp(px_format,"VDPAU_VC1")==0) return AV_PIX_FMT_VDPAU_VC1;
#endif
    if(strcmp(px_format,"RGB48BE")==0) return AV_PIX_FMT_RGB48BE;
    if(strcmp(px_format,"RGB48LE")==0) return AV_PIX_FMT_RGB48LE;

    if(strcmp(px_format,"RGB565BE")==0) return AV_PIX_FMT_RGB565BE;
    if(strcmp(px_format,"RGB565LE")==0) return AV_PIX_FMT_RGB565LE;
    if(strcmp(px_format,"RGB555BE")==0) return AV_PIX_FMT_RGB555BE;
    if(strcmp(px_format,"RGB555LE")==0) return AV_PIX_FMT_RGB555LE;

    if(strcmp(px_format,"BGR565BE")==0) return AV_PIX_FMT_BGR565BE;
    if(strcmp(px_format,"BGR565LE")==0) return AV_PIX_FMT_BGR565LE;
    if(strcmp(px_format,"BGR555BE")==0) return AV_PIX_FMT_BGR555BE;
    if(strcmp(px_format,"BGR555LE")==0) return AV_PIX_FMT_BGR555LE;

#if FF_API_VAAPI
    /** @name Deprecated pixel formats */
    /**@{*/
    if(strcmp(px_format,"VAAPI_MOCO")==0) return AV_PIX_FMT_VAAPI_MOCO;
    if(strcmp(px_format,"VAAPI_IDCT")==0) return AV_PIX_FMT_VAAPI_IDCT;
    if(strcmp(px_format,"VAAPI_VLD")==0) return AV_PIX_FMT_VAAPI_VLD;
    /**@}*/
    if(strcmp(px_format,"VAAPI")==0) return AV_PIX_FMT_VAAPI;
#else

    if(strcmp(px_format,"VAAPI")==0) return AV_PIX_FMT_VAAPI;
#endif

    if(strcmp(px_format,"YUV420P16LE")==0) return AV_PIX_FMT_YUV420P16LE;
    if(strcmp(px_format,"YUV420P16BE")==0) return AV_PIX_FMT_YUV420P16BE;
    if(strcmp(px_format,"YUV422P16LE")==0) return AV_PIX_FMT_YUV422P16LE;
    if(strcmp(px_format,"YUV422P16BE")==0) return AV_PIX_FMT_YUV422P16BE;
    if(strcmp(px_format,"YUV444P16LE")==0) return AV_PIX_FMT_YUV444P16LE;
    if(strcmp(px_format,"YUV444P16BE")==0) return AV_PIX_FMT_YUV444P16BE;
#if FF_API_VDPAU
    if(strcmp(px_format,"VDPAU_MPEG4")==0) return AV_PIX_FMT_VDPAU_MPEG4;
#endif
    if(strcmp(px_format,"DXVA2_VLD")==0) return AV_PIX_FMT_DXVA2_VLD;

    if(strcmp(px_format,"RGB444LE")==0) return AV_PIX_FMT_RGB444LE;
    if(strcmp(px_format,"RGB444BE")==0) return AV_PIX_FMT_RGB444BE;
    if(strcmp(px_format,"BGR444LE")==0) return AV_PIX_FMT_BGR444LE;
    if(strcmp(px_format,"BGR444BE")==0) return AV_PIX_FMT_BGR444BE;
    if(strcmp(px_format,"YA8")==0) return AV_PIX_FMT_YA8;

    if(strcmp(px_format,"Y400A")==0) return AV_PIX_FMT_Y400A;
    if(strcmp(px_format,"GRAY8A")==0) return AV_PIX_FMT_GRAY8A;

    if(strcmp(px_format,"BGR48BE")==0) return AV_PIX_FMT_BGR48BE;
    if(strcmp(px_format,"BGR48LE")==0) return AV_PIX_FMT_BGR48LE;
    if(strcmp(px_format,"YUV420P9BE")==0) return AV_PIX_FMT_YUV420P9BE;
    if(strcmp(px_format,"YUV420P9LE")==0) return AV_PIX_FMT_YUV420P9LE;
    if(strcmp(px_format,"YUV420P10BE")==0) return AV_PIX_FMT_YUV420P10BE;
    if(strcmp(px_format,"YUV420P10LE")==0) return AV_PIX_FMT_YUV420P10LE;
    if(strcmp(px_format,"YUV422P10BE")==0) return AV_PIX_FMT_YUV422P10BE;
    if(strcmp(px_format,"YUV422P10LE")==0) return AV_PIX_FMT_YUV422P10LE;
    if(strcmp(px_format,"YUV444P9BE")==0) return AV_PIX_FMT_YUV444P9BE;
    if(strcmp(px_format,"YUV444P9LE")==0) return AV_PIX_FMT_YUV444P9LE;
    if(strcmp(px_format,"YUV444P10BE")==0) return AV_PIX_FMT_YUV444P10BE;
    if(strcmp(px_format,"YUV444P10LE")==0) return AV_PIX_FMT_YUV444P10LE;
    if(strcmp(px_format,"YUV422P9BE")==0) return AV_PIX_FMT_YUV422P9BE;
    if(strcmp(px_format,"YUV422P9LE")==0) return AV_PIX_FMT_YUV422P9LE;
    if(strcmp(px_format,"VDA_VLD")==0) return AV_PIX_FMT_VDA_VLD;
    if(strcmp(px_format,"GBRP")==0) return AV_PIX_FMT_GBRP;
    if(strcmp(px_format,"GBRP9BE")==0) return AV_PIX_FMT_GBRP9BE;
    if(strcmp(px_format,"GBRP9LE")==0) return AV_PIX_FMT_GBRP9LE;
    if(strcmp(px_format,"GBRP10BE")==0) return AV_PIX_FMT_GBRP10BE;
    if(strcmp(px_format,"GBRP10LE")==0) return AV_PIX_FMT_GBRP10LE;
    if(strcmp(px_format,"GBRP16BE")==0) return AV_PIX_FMT_GBRP16BE;
    if(strcmp(px_format,"GBRP16LE")==0) return AV_PIX_FMT_GBRP16LE;
    if(strcmp(px_format,"YUVA422P")==0) return AV_PIX_FMT_YUVA422P;
    if(strcmp(px_format,"YUVA444P")==0) return AV_PIX_FMT_YUVA444P;
    if(strcmp(px_format,"YUVA420P9BE")==0) return AV_PIX_FMT_YUVA420P9BE;
    if(strcmp(px_format,"YUVA420P9LE")==0) return AV_PIX_FMT_YUVA420P9LE;
    if(strcmp(px_format,"YUVA422P9BE")==0) return AV_PIX_FMT_YUVA422P9BE;
    if(strcmp(px_format,"YUVA422P9LE")==0) return AV_PIX_FMT_YUVA422P9LE;
    if(strcmp(px_format,"YUVA444P9BE")==0) return AV_PIX_FMT_YUVA444P9BE;
    if(strcmp(px_format,"YUVA444P9LE")==0) return AV_PIX_FMT_YUVA444P9LE;
    if(strcmp(px_format,"YUVA420P10BE")==0) return AV_PIX_FMT_YUVA420P10BE;
    if(strcmp(px_format,"YUVA420P10LE")==0) return AV_PIX_FMT_YUVA420P10LE;
    if(strcmp(px_format,"YUVA422P10BE")==0) return AV_PIX_FMT_YUVA422P10BE;
    if(strcmp(px_format,"YUVA422P10LE")==0) return AV_PIX_FMT_YUVA422P10LE;
    if(strcmp(px_format,"YUVA444P10BE")==0) return AV_PIX_FMT_YUVA444P10BE;
    if(strcmp(px_format,"YUVA444P10LE")==0) return AV_PIX_FMT_YUVA444P10LE;
    if(strcmp(px_format,"YUVA420P16BE")==0) return AV_PIX_FMT_YUVA420P16BE;
    if(strcmp(px_format,"YUVA420P16LE")==0) return AV_PIX_FMT_YUVA420P16LE;
    if(strcmp(px_format,"YUVA422P16BE")==0) return AV_PIX_FMT_YUVA422P16BE;
    if(strcmp(px_format,"YUVA422P16LE")==0) return AV_PIX_FMT_YUVA422P16LE;
    if(strcmp(px_format,"YUVA444P16BE")==0) return AV_PIX_FMT_YUVA444P16BE;
    if(strcmp(px_format,"YUVA444P16LE")==0) return AV_PIX_FMT_YUVA444P16LE;

    if(strcmp(px_format,"VDPAU")==0) return AV_PIX_FMT_VDPAU;

    if(strcmp(px_format,"XYZ12LE")==0) return AV_PIX_FMT_XYZ12LE;
    if(strcmp(px_format,"XYZ12BE")==0) return AV_PIX_FMT_XYZ12BE;
    if(strcmp(px_format,"NV16")==0) return AV_PIX_FMT_NV16;
    if(strcmp(px_format,"NV20LE")==0) return AV_PIX_FMT_NV20LE;
    if(strcmp(px_format,"NV20BE")==0) return AV_PIX_FMT_NV20BE;

    if(strcmp(px_format,"RGBA64BE")==0) return AV_PIX_FMT_RGBA64BE;
    if(strcmp(px_format,"RGBA64LE")==0) return AV_PIX_FMT_RGBA64LE;
    if(strcmp(px_format,"BGRA64BE")==0) return AV_PIX_FMT_BGRA64BE;
    if(strcmp(px_format,"BGRA64LE")==0) return AV_PIX_FMT_BGRA64LE;

    if(strcmp(px_format,"YVYU422")==0) return AV_PIX_FMT_YVYU422;

    if(strcmp(px_format,"VDA")==0) return AV_PIX_FMT_VDA;

    if(strcmp(px_format,"YA16BE")==0) return AV_PIX_FMT_YA16BE;
    if(strcmp(px_format,"YA16LE")==0) return AV_PIX_FMT_YA16LE;

    if(strcmp(px_format,"GBRAP")==0) return AV_PIX_FMT_GBRAP;
    if(strcmp(px_format,"GBRAP16BE")==0) return AV_PIX_FMT_GBRAP16BE;
    if(strcmp(px_format,"GBRAP16LE")==0) return AV_PIX_FMT_GBRAP16LE;
    if(strcmp(px_format,"QSV")==0) return AV_PIX_FMT_QSV;
    if(strcmp(px_format,"MMAL")==0) return AV_PIX_FMT_MMAL;

    if(strcmp(px_format,"D3D11VA_VLD")==0) return AV_PIX_FMT_D3D11VA_VLD;
    if(strcmp(px_format,"CUDA")==0) return AV_PIX_FMT_CUDA;

    if(strcmp(px_format,"0RGB")==0) return AV_PIX_FMT_0RGB;
    if(strcmp(px_format,"RGB0")==0) return AV_PIX_FMT_RGB0;
    if(strcmp(px_format,"0BGR")==0) return AV_PIX_FMT_0BGR;
    if(strcmp(px_format,"BGR0")==0) return AV_PIX_FMT_BGR0;

    if(strcmp(px_format,"YUV420P12BE")==0) return AV_PIX_FMT_YUV420P12BE;
    if(strcmp(px_format,"YUV420P12LE")==0) return AV_PIX_FMT_YUV420P12LE;
    if(strcmp(px_format,"YUV420P14BE")==0) return AV_PIX_FMT_YUV420P14BE;
    if(strcmp(px_format,"YUV420P14LE")==0) return AV_PIX_FMT_YUV420P14LE;
    if(strcmp(px_format,"YUV422P12BE")==0) return AV_PIX_FMT_YUV422P12BE;
    if(strcmp(px_format,"YUV422P12LE")==0) return AV_PIX_FMT_YUV422P12LE;
    if(strcmp(px_format,"YUV422P14BE")==0) return AV_PIX_FMT_YUV422P14BE;
    if(strcmp(px_format,"YUV422P14LE")==0) return AV_PIX_FMT_YUV422P14LE;
    if(strcmp(px_format,"YUV444P12BE")==0) return AV_PIX_FMT_YUV444P12BE;
    if(strcmp(px_format,"YUV444P12LE")==0) return AV_PIX_FMT_YUV444P12LE;
    if(strcmp(px_format,"YUV444P14BE")==0) return AV_PIX_FMT_YUV444P14BE;
    if(strcmp(px_format,"YUV444P14LE")==0) return AV_PIX_FMT_YUV444P14LE;
    if(strcmp(px_format,"GBRP12BE")==0) return AV_PIX_FMT_GBRP12BE;
    if(strcmp(px_format,"GBRP12LE")==0) return AV_PIX_FMT_GBRP12LE;
    if(strcmp(px_format,"GBRP14BE")==0) return AV_PIX_FMT_GBRP14BE;
    if(strcmp(px_format,"GBRP14LE")==0) return AV_PIX_FMT_GBRP14LE;
    if(strcmp(px_format,"YUVJ411P")==0) return AV_PIX_FMT_YUVJ411P;

    if(strcmp(px_format,"BAYER_BGGR8")==0) return AV_PIX_FMT_BAYER_BGGR8;
    if(strcmp(px_format,"BAYER_RGGB8")==0) return AV_PIX_FMT_BAYER_RGGB8;
    if(strcmp(px_format,"BAYER_GBRG8")==0) return AV_PIX_FMT_BAYER_GBRG8;
    if(strcmp(px_format,"BAYER_GRBG8")==0) return AV_PIX_FMT_BAYER_GRBG8;
    if(strcmp(px_format,"BAYER_BGGR16LE")==0) return AV_PIX_FMT_BAYER_BGGR16LE;
    if(strcmp(px_format,"BAYER_BGGR16BE")==0) return AV_PIX_FMT_BAYER_BGGR16BE;
    if(strcmp(px_format,"BAYER_RGGB16LE")==0) return AV_PIX_FMT_BAYER_RGGB16LE;
    if(strcmp(px_format,"BAYER_RGGB16BE")==0) return AV_PIX_FMT_BAYER_RGGB16BE;
    if(strcmp(px_format,"BAYER_GBRG16LE")==0) return AV_PIX_FMT_BAYER_GBRG16LE;
    if(strcmp(px_format,"BAYER_GBRG16BE")==0) return AV_PIX_FMT_BAYER_GBRG16BE;
    if(strcmp(px_format,"BAYER_GRBG16LE")==0) return AV_PIX_FMT_BAYER_GRBG16LE;
    if(strcmp(px_format,"BAYER_GRBG16BE")==0) return AV_PIX_FMT_BAYER_GRBG16BE;
#if !FF_API_XVMC
    if(strcmp(px_format,"XVMC")==0) return AV_PIX_FMT_XVMC;
#endif
    if(strcmp(px_format,"YUV440P10LE")==0) return AV_PIX_FMT_YUV440P10LE;
    if(strcmp(px_format,"YUV440P10BE")==0) return AV_PIX_FMT_YUV440P10BE;
    if(strcmp(px_format,"YUV440P12LE")==0) return AV_PIX_FMT_YUV440P12LE;
    if(strcmp(px_format,"YUV440P12BE")==0) return AV_PIX_FMT_YUV440P12BE;
    if(strcmp(px_format,"AYUV64LE")==0) return AV_PIX_FMT_AYUV64LE;
    if(strcmp(px_format,"AYUV64BE")==0) return AV_PIX_FMT_AYUV64BE;

    if(strcmp(px_format,"VIDEOTOOLBOX")==0) return AV_PIX_FMT_VIDEOTOOLBOX;

    if(strcmp(px_format,"P010LE")==0) return AV_PIX_FMT_P010LE;
    if(strcmp(px_format,"P010BE")==0) return AV_PIX_FMT_P010BE;

    if(strcmp(px_format,"GBRAP12BE")==0) return AV_PIX_FMT_GBRAP12BE;
    if(strcmp(px_format,"GBRAP12LE")==0) return AV_PIX_FMT_GBRAP12LE;

    if(strcmp(px_format,"NB")==0) return AV_PIX_FMT_NB;

    return AV_PIX_FMT_NONE;

}

void DLL_EXPORT setup_decoder(char* pix_format,int num_threads)
{
    MAX_THREADS=num_threads;
    tid = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    frames_decoded = (int*)malloc(sizeof(int)*num_threads);
    PIXEL_FORMAT=px_format_to_enum(strtoupper(pix_format));
    is_error=0;
    //printf("%d",PIXEL_FORMAT);
//    }
}

int DLL_EXPORT open_input_file(char* inputVideoFileName,double* nb_frames,double* img_height,double* img_width)
{
    //GC_INIT();
    //GC_allow_register_threads();
    is_error=0;
    av_register_all();
    avcodec_register_all();
    ::AVFormatContext * ctx = NULL;
    AVCodec * codec = NULL;
    int strm = -1;
    AVCodecContext * codecCtx = NULL;
    int err = avformat_open_input(&ctx, inputVideoFileName, NULL, NULL);
    CHECK_ERR_INT(err);
    err = avformat_find_stream_info(ctx,NULL);
    CHECK_ERR_INT(err);
    strm = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

    codecCtx = ctx->streams[strm]->codec;
    //printf("%d",ctx->streams[strm]->nb_frames);
    err = avcodec_open2(codecCtx, codec, NULL);
    CHECK_ERR_INT(err);
    *nb_frames=ctx->streams[strm]->nb_frames;
    *img_height=codecCtx->height;
    *img_width=codecCtx->width;
    video_file_name=(char*)malloc(strlen(inputVideoFileName)*(sizeof(char)+1));
    strcpy(video_file_name,inputVideoFileName);
    total_frames = ctx->streams[strm]->nb_frames;
    tiff_data_size_array = (int*) malloc((total_frames+1)*sizeof(int));
    tiff_data_array=(uint8_t**)malloc((total_frames+1)*sizeof(uint8_t*));
    avcodec_close(codecCtx);
    for(int i=0; i<MAX_THREADS; i++)
        frames_decoded[i]=0;
    avformat_close_input(&ctx);
    av_free(ctx);
    return 0;
}

// a sample exported function
void* get_all_frames_as_tiff(void*)
{
    int ready=0;
    int video_frame_count=0;
    int skip=0;
    int err=0;
    int strm = -1;
    int i = 0;
    int thread_number=0;
    uint32_t start_frame=0;
    uint32_t end_frame=0;
    uint32_t interval=(int)(total_frames/MAX_THREADS);
    pthread_t id = pthread_self();
    for(i=0; i<MAX_THREADS; i++)
    {
        if(pthread_equal(id,tid[i]))
        {
            start_frame=interval*i;
            if(i==MAX_THREADS-1)
                end_frame=total_frames;
            else
                end_frame=interval*(i+1);
            thread_number=i;
            break;
        }
    }
    ::AVFormatContext * ctx = NULL;
    AVCodec * codec = NULL;
    AVCodecContext * codecCtx = NULL;
    i=0;
    err = avformat_open_input(&ctx, video_file_name, NULL, NULL);
    CHECK_ERR(err);
    err = avformat_find_stream_info(ctx,NULL);
    CHECK_ERR(err);
    strm = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

    codecCtx = ctx->streams[strm]->codec;
    //printf("%d",ctx->streams[strm]->nb_frames);
    err = avcodec_open2(codecCtx, codec, NULL);
    CHECK_ERR(err);

    for(i=0; i<start_frame; i++)
    {
        AVPacket pkt;
        av_init_packet(&pkt);
        err = av_read_frame(ctx, &pkt);
        av_free_packet(&pkt);
        CHECK_ERR(err);
        video_frame_count++;
    }
    while (video_frame_count<end_frame)
    {
        AVPacket pkt;
        av_init_packet(&pkt);
        err = av_read_frame(ctx, &pkt);
        CHECK_ERR(err);

        if (pkt.stream_index == strm)
        {
            int got = 0;
            AVFrame * frame = av_frame_alloc();
            err = avcodec_decode_video2(codecCtx, frame, &got, &pkt);
            CHECK_ERR(err);
            if (got)
            {

                AVCodec *outCodec = avcodec_find_encoder(AV_CODEC_ID_TIFF);
                AVCodecContext *outCodecCtx = avcodec_alloc_context3(outCodec);
                if (!codecCtx)
                {
                    is_error=1;
                    return NULL;
                }


                outCodecCtx->width = codecCtx->width;
                outCodecCtx->height = codecCtx->height;
                if(PIXEL_FORMAT==-1)
                    outCodecCtx->pix_fmt = codecCtx->pix_fmt;
                else
                    outCodecCtx->pix_fmt = static_cast<AVPixelFormat>(PIXEL_FORMAT);
                outCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
                outCodecCtx->time_base.num = codecCtx->time_base.num;
                outCodecCtx->time_base.den = codecCtx->time_base.den;

                if (!outCodec || avcodec_open2(outCodecCtx, outCodec, NULL) < 0)
                {
                    is_error=1;
                    return NULL;
                }

                AVPacket outPacket;
                av_init_packet(&outPacket);
                outPacket.size = 0;
                outPacket.data = NULL;
                int gotFrame = 0;
                int ret = avcodec_encode_video2(outCodecCtx, &outPacket, frame, &gotFrame);
                //printf("here");
                if (ret >= 0 && gotFrame)
                {
                    tiff_data_size_array[video_frame_count]=outPacket.size;
                    tiff_data_array[video_frame_count]=(uint8_t*)malloc(outPacket.size*sizeof(uint8_t));
                    memcpy(tiff_data_array[video_frame_count],outPacket.data,outPacket.size);
                    video_frame_count++;
                    frames_decoded[thread_number]++;
                }
                else
                    is_error=1;
                av_free_packet(&outPacket);
                avcodec_close(outCodecCtx);
                av_free(outCodecCtx);
            }
            av_free_packet(&pkt);
            av_frame_unref(frame);
            av_frame_free(&frame);
            av_free(frame);
        }
    }

    //free(&pkt);
    avcodec_close(codecCtx);
    avformat_close_input(&ctx);
    av_free(ctx);
    av_free(codec);
    ctx = NULL;
    codec = NULL;
    codecCtx = NULL;
    pthread_exit(NULL);

}

int DLL_EXPORT number_of_frames_decoded()
{
    int sum=0;
    for(int i=0; i<MAX_THREADS; i++)
        sum=sum+frames_decoded[i];
    if(is_error==1)
        return -1;
    return sum;
}

uint8_t* DLL_EXPORT getFramesAsTiff(double frame_number,double* out_length)
{
    int temp=frame_number;
    //printf("%d\n",temp);
    //current_frame++;
    *out_length=tiff_data_size_array[temp];
    return tiff_data_array[temp];
}

void DLL_EXPORT free_frame(double frame_number)
{
    free(tiff_data_array[(int)frame_number]);
}
void DLL_EXPORT free_memory()
{
    int i=0;
    for(i=0; i<total_frames; i++)
        free(tiff_data_array[i]);
    free(tiff_data_size_array);
    free(tiff_data_array);
    free(video_file_name);
    free(frames_decoded);
    free(tid);
    is_error=0;
}

void DLL_EXPORT extract_frames()
{
    int i=0;
    //get_all_frames_as_tiff(NULL);
    for(i=0; i<MAX_THREADS; i++)
        pthread_create(&tid[i], NULL, get_all_frames_as_tiff, NULL);
    //for(i=0;i<MAX_THREADS;i++)
    //    pthread_join(tid[i],NULL);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        break;

    case DLL_THREAD_ATTACH:
        // attach to thread
        break;

    case DLL_THREAD_DETACH:
        // detach from thread
        break;
    }
    return TRUE; // succesful
}
