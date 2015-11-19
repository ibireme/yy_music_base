//
//  ym_fft.h
//  yy_music
//
//  Created by ibireme on 14/12/2.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_fft__
#define __ym_fft__

#include "ym_base.h"
#include "ym_complex.h"

YM_EXTERN_C_BEGIN

typedef enum {
    YM_FFT_FORWARD = -1, ///< fft
    YM_FFT_INVERSE =  1, ///< ifft
} ym_fft_direction;


typedef struct _ym_fft_float *ym_fft_float_ref;
typedef struct _ym_fft_double *ym_fft_double_ref;

ym_fft_float_ref ym_fft_create_float(uint32_t length);
ym_fft_double_ref ym_fft_create_double(uint32_t length);


void ym_fft_cpx_float(ym_fft_float_ref setup,
                      ym_cpx_float *   input,
                      ym_cpx_float *   output,
                      ym_fft_direction direction,
                      uint32_t         length);

void ym_fft_cpx_double(ym_fft_double_ref setup,
                       ym_cpx_double *   input,
                       ym_cpx_double *   output,
                       ym_fft_direction  direction,
                       uint32_t          length);

void ym_fft_cpx_split_float(ym_fft_float_ref    setup,
                            ym_cpx_split_float *input,
                            ym_cpx_split_float *output,
                            ym_fft_direction    direction,
                            uint32_t            length);

void ym_fft_cpx_split_double(ym_fft_double_ref    setup,
                             ym_cpx_split_double *input,
                             ym_cpx_split_double *output,
                             ym_fft_direction     direction,
                             uint32_t             length);

void ym_fft_real_float(ym_fft_float_ref setup,
                       ym_cpx_float *   input,
                       ym_cpx_float *   output,
                       ym_fft_direction direction,
                       uint32_t         length);

void ym_fft_real_double(ym_fft_double_ref setup,
                        ym_cpx_double *   input,
                        ym_cpx_double *   output,
                        ym_fft_direction  direction,
                        uint32_t          length);


YM_EXTERN_C_END
#endif
