//
//  ym_smid.h
//  yy_music
//
//  Created by ibireme on 14/12/2.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __YYMidiApp__ym_simd__
#define __YYMidiApp__ym_simd__

#include "ym_base.h"
#include "ym_complex.h"


YM_EXTERN_C_BEGIN



// data type

void ym_v_int16_to_int32(int16_t *input, int32_t *output, uint32_t length);
void ym_v_int32_to_int16(int32_t *input, int16_t *output, uint32_t length);

void ym_v_float_to_double(float *input, double *output, uint32_t length);
void ym_v_double_to_float(double *input, float *output, uint32_t length);

void ym_v_int16_to_float(int16_t *input, float *output, uint32_t length);
void ym_v_int16_to_double(int16_t *input, double *output, uint32_t length);

void ym_v_int32_to_float(int32_t *input, float *output, uint32_t length);
void ym_v_int32_to_double(int32_t *input, double *output, uint32_t length);

void ym_v_float_to_int16(float *input, int16_t *output, uint32_t length);
void ym_v_float_to_int32(float *input, int32_t *output, uint32_t length);
void ym_v_float_to_int16_round(float *input, int16_t *output, uint32_t length);
void ym_v_float_to_int32_round(float *input, int32_t *output, uint32_t length);

void ym_v_double_to_int16(double *input, int16_t *output, uint32_t length);
void ym_v_double_to_int32(double *input, int32_t *output, uint32_t length);
void ym_v_double_to_int16_round(double *input, int16_t *output, uint32_t length);
void ym_v_double_to_int32_round(double *input, int32_t *output, uint32_t length);



// math
void ym_v_sin_float(float *input, float *output, uint32_t length);
void ym_v_sin_double(double *input, double *output, uint32_t length);

void ym_v_cos_float(float *input, float *output, uint32_t length);
void ym_v_cos_double(double *input, double *output, uint32_t length);

void ym_v_tan_float(float *input, float *output, uint32_t length);
void ym_v_tan_double(double *input, double *output, uint32_t length);

void ym_v_asin_float(float *input, float *output, uint32_t length);
void ym_v_asin_double(double *input, double *output, uint32_t length);

void ym_v_acos_float(float *input, float *output, uint32_t length);
void ym_v_acos_double(double *input, double *output, uint32_t length);

void ym_v_atan_float(float *input, float *output, uint32_t length);
void ym_v_atan_double(double *input, double *output, uint32_t length);

void ym_v_log_float(float *input, float *output, uint32_t length);
void ym_v_log_double(double *input, double *output, uint32_t length);

void ym_v_log2_float(float *input, float *output, uint32_t length);
void ym_v_log2_double(double *input, double *output, uint32_t length);

void ym_v_log10_float(float *input, float *output, uint32_t length);
void ym_v_log10_double(double *input, double *output, uint32_t length);

void ym_v_exp_float(float *input, float *output, uint32_t length);
void ym_v_exp_double(double *input, double *output, uint32_t length);

void ym_v_exp2_float(float *input, float *output, uint32_t length);
void ym_v_exp2_double(double *input, double *output, uint32_t length);

void ym_v_pow_float(float *input, float exp, float *output, uint32_t length);
void ym_v_pow_double(double *input, double exp, double *output, uint32_t length);

void ym_v_sqrt_float(float *input, float *output, uint32_t length);
void ym_v_sqrt_double(double *input, double *output, uint32_t length);

void ym_v_abs_int16(int16_t *input, int16_t *output, uint32_t length);
void ym_v_abs_int32(int32_t *input, int32_t *output, uint32_t length);
void ym_v_abs_float(float *input, float *output, uint32_t length);
void ym_v_abs_double(double *input, double *output, uint32_t length);



// output[i] = input[i] * input[i]
void ym_v_square_float(float *input, float *output, uint32_t length);
void ym_v_square_double(double *input, double *output, uint32_t length);




// output[i] = input[i] + value
void ym_v_scalar_add_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length);
void ym_v_scalar_add_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length);
void ym_v_scalar_add_float(float *input, float *output, float value, uint32_t length);
void ym_v_scalar_add_double(double *input, double *output, double value, uint32_t length);

// output[i] = input[i] - value
void ym_v_scalar_sub_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length);
void ym_v_scalar_sub_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length);
void ym_v_scalar_sub_float(float *input, float *output, float value, uint32_t length);
void ym_v_scalar_sub_double(double *input, double *output, double value, uint32_t length);

// output[i] = input[i] * value
void ym_v_scalar_mul_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length);
void ym_v_scalar_mul_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length);
void ym_v_scalar_mul_float(float *input, float *output, float value, uint32_t length);
void ym_v_scalar_mul_double(double *input, double *output, double value, uint32_t length);

// output[i] = input[i] / value
void ym_v_scalar_div_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length);
void ym_v_scalar_div_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length);
void ym_v_scalar_div_float(float *input, float *output, float value, uint32_t length);
void ym_v_scalar_div_double(double *input, double *output, double value, uint32_t length);



// output[i] = input1[i] + input2[i]
void ym_v_add_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length);
void ym_v_add_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length);
void ym_v_add_float(float *input1, float *input2, float *output, uint32_t length);
void ym_v_add_double(double *input1, double *input2, double *output, uint32_t length);

// output[i] = input1[i] - input2[i]
void ym_v_sub_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length);
void ym_v_sub_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length);
void ym_v_sub_float(float *input1, float *input2, float *output, uint32_t length);
void ym_v_sub_double(double *input1, double *input2, double *output, uint32_t length);

// output[i] = input1[i] * input2[i]
void ym_v_mul_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length);
void ym_v_mul_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length);
void ym_v_mul_float(float *input1, float *input2, float *output, uint32_t length);
void ym_v_mul_double(double *input1, double *input2, double *output, uint32_t length);

// output[i] = input1[i] / input2[i]
void ym_v_div_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length);
void ym_v_div_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length);
void ym_v_div_float(float *input1, float *input2, float *output, uint32_t length);
void ym_v_div_double(double *input1, double *input2, double *output, uint32_t length);







// complex

// output = sqrt( input.real^2 + input.imag^2 )
void ym_v_abs_cpx_float(ym_cpx_float *input, float *output, uint32_t length);
void ym_v_abs_cpx_double(ym_cpx_double *input, double *output, uint32_t length);
void ym_v_abs_cpx_split_float(ym_cpx_split_float *input, float *output, uint32_t length);
void ym_v_abs_cpx_split_double(ym_cpx_split_double *input, double *output, uint32_t length);

// output = input.real^2 + input.imag^2
void ym_v_mag_cpx_float(ym_cpx_float *input, float *output, uint32_t length);
void ym_v_mag_cpx_double(ym_cpx_double *input, double *output, uint32_t length);
void ym_v_mag_cpx_split_float(ym_cpx_split_float *input, float *output, uint32_t length);
void ym_v_mag_cpx_split_double(ym_cpx_split_double *input, double *output, uint32_t length);

// output = input1 + input2
void ym_v_add_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length);
void ym_v_add_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length);
void ym_v_add_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length);
void ym_v_add_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length);

// output = input1 - input2
void ym_v_sub_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length);
void ym_v_sub_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length);
void ym_v_sub_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length);
void ym_v_sub_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length);

// output = input1 * input2
void ym_v_mul_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length);
void ym_v_mul_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length);
void ym_v_mul_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length);
void ym_v_mul_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length);

// output = input1 / input2
void ym_v_div_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length);
void ym_v_div_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length);
void ym_v_div_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length);
void ym_v_div_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length);



YM_EXTERN_C_END
#endif
