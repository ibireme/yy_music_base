//
//  ym_window_func.h
//  yy_music
//
//  Created by ibireme on 14/12/31.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __yy_music__ym_window_func__
#define __yy_music__ym_window_func__

#include "ym_base.h"

YM_EXTERN_C_BEGIN

/**
 Window function type
 see: http://en.wikipedia.org/wiki/Window_function
 see: http://www.rssd.esa.int/SP/LISAPATHFINDER/docs/Data_Analysis/GH_FFT.pdf
 */
typedef enum {
    YM_WINDOW_TYPE_None = 0,         ///< None (Rectangular)
    YM_WINDOW_TYPE_Bartlett,         ///< Bartlett (Triangular)
    YM_WINDOW_TYPE_Hamming,          ///< Hamming
    YM_WINDOW_TYPE_Hanning,          ///< Hanning
    YM_WINDOW_TYPE_Welch,            ///< Welch
    YM_WINDOW_TYPE_Blackman,         ///< Blackman
    YM_WINDOW_TYPE_Blackman_Harris,  ///< Blackman Harris
    YM_WINDOW_TYPE_Blackman_Nuttall, ///< Blackman Nuttall
    YM_WINDOW_TYPE_Nuttal,           ///< Nuttal
    YM_WINDOW_TYPE_Flat_top,         ///< Flat top
    YM_WINDOW_TYPE_Kaiser,           ///< Kaiser  (alpha = 2, 3, ...)
    YM_WINDOW_TYPE_Parzen,           ///< Parzen
    YM_WINDOW_TYPE_Lanczos,          ///< Lanczos
    YM_WINDOW_TYPE_Gaussian,         ///< Gaussian (alpha = 2.5, 3.5, 4.5, ...)
    YM_WINDOW_TYPE_Dolph_Chebyshev,  ///< Dolph Chebyshev (alpha = 5, 6, ...) (5 for -100dB, 6 for -120dB, ...)
} ym_window_type;


/**
 Calculate window function data (float).
 
 @param type   window function type
 @param data   data for output
 @param length data length (2^x)
 */
void ym_window_function_data_float(ym_window_type type, float *data, uint32_t length);


/**
 Calculate window function data (double).
 
 @param type   window function type
 @param data   data for output
 @param length data length (2^x)
 */
void ym_window_function_data_double(ym_window_type type, double *data, uint32_t length);

/**
 Calculate window function data (float).
 
 @param type         window function type
 @param data         data for output
 @param length       data length (2^x)
 @param is_symmetric true for normal situation such as analysis, false for FFT overlap
 @param alpha        window function coefficient, see ym_window_type. 
                     pass 0 if the window function do not need.
 */
void ym_window_function_data_float_with_option(ym_window_type type, float *data, uint32_t length, bool is_symmetric, float alpha);

/**
 Calculate window function data (double).
 
 @param type         window function type
 @param data         data for output
 @param length       data length (2^x)
 @param is_symmetric true for normal situation such as analysis, false for FFT overlap
 @param alpha        window function coefficient, see ym_window_type.
 pass 0 if the window function do not need.
 */
void ym_window_function_data_double_with_option(ym_window_type type, double *data, uint32_t length, bool is_symmetric, double alpha);


YM_EXTERN_C_END
#endif
