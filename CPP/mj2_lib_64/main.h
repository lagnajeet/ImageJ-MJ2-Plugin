#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT setup_decoder(char* pix_format, int num_threads);
uint8_t* DLL_EXPORT getFramesAsTiff(double frame_number,double* out_length);
//uint16_t* DLL_EXPORT getFramesAsShort(double* linewidth);
int DLL_EXPORT open_input_file(char* inputVideoFileName,double* nb_frames,double* img_height,double* img_width);
void DLL_EXPORT extract_frames();
void DLL_EXPORT free_memory();
void DLL_EXPORT free_frame(double frame_number);
int DLL_EXPORT number_of_frames_decoded();

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
