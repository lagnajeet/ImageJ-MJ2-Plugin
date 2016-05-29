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
#include <pthread.h>


#include "main.h"
#define CHECK_ERR(ERR) {if ((ERR)<0) return NULL; }
#define CHECK_ERR_INT(ERR) {if ((ERR)<0) return -1; }
static uint8_t MAX_THREADS=6;
char* video_file_name=NULL;
uint32_t total_frames=0;
pthread_t *tid;
int *frames_decoded;
uint8_t** tiff_data_array=NULL;
int* tiff_data_size_array=NULL;

void DLL_EXPORT setup_decoder(double num_threads,double pix_format)
{
    tid = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    frames_decoded = (int*)malloc(sizeof(int)*num_threads);
}

int DLL_EXPORT open_input_file(char* inputVideoFileName,double* nb_frames,double* img_height,double* img_width)
{
    //GC_INIT();
    //GC_allow_register_threads();
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
    for(int i=0;i<MAX_THREADS;i++)
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
    for(i=0;i<MAX_THREADS;i++)
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

    for(i=0;i<start_frame;i++)
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
                    return NULL;

                outCodecCtx->width = codecCtx->width;
                outCodecCtx->height = codecCtx->height;
                outCodecCtx->pix_fmt = codecCtx->pix_fmt;
                outCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
                outCodecCtx->time_base.num = codecCtx->time_base.num;
                outCodecCtx->time_base.den = codecCtx->time_base.den;

                if (!outCodec || avcodec_open2(outCodecCtx, outCodec, NULL) < 0)
                    return NULL;

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
    for(int i=0;i<MAX_THREADS;i++)
        sum=sum+frames_decoded[i];
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
    for(i=0;i<total_frames;i++)
        free(tiff_data_array[i]);
    free(tiff_data_size_array);
    free(tiff_data_array);
    free(video_file_name);
    free(frames_decoded);
    free(tid);
}

void DLL_EXPORT extract_frames()
{
    int i=0;
    //get_all_frames_as_tiff(NULL);
    for(i=0;i<MAX_THREADS;i++)
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
