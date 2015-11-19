//
//  ym_smid.c
//  yy_music
//
//  Created by ibireme on 14/12/2.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_simd.h"

#if YM_OS_APPLE
#include <Accelerate/Accelerate.h>
#endif

#if YM_HAVE_NEON
#include <arm_neon.h>
#endif


//#undef YM_OS_APPLE
//#define YM_OS_APPLE 0




void ym_v_int16_to_int32(int16_t *input, int32_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
}

void ym_v_int32_to_int16(int32_t *input, int16_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
}

void ym_v_float_to_double(float *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vspdp(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_double_to_float(double *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vdpsp(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_int16_to_float(int16_t *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vflt16(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_int16_to_double(int16_t *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vflt16D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_int32_to_float(int32_t *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vflt32(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_int32_to_double(int32_t *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vflt32D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_float_to_int16(float *input, int16_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfix16(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_float_to_int32(float *input, int32_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfix32(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_float_to_int16_round(float *input, int16_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfixr16(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] + 0.5;
    }
#endif
}

void ym_v_float_to_int32_round(float *input, int32_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfixr32(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] + 0.5;
    }
#endif
}

void ym_v_double_to_int16(double *input, int16_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfix16D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_double_to_int32(double *input, int32_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfix32D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i];
    }
#endif
}

void ym_v_double_to_int16_round(double *input, int16_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfixr16D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] + 0.5;
    }
#endif
}

void ym_v_double_to_int32_round(double *input, int32_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vfixr32D(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] + 0.5;
    }
#endif
}

void ym_v_sin_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvsinf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sinf(input[i]);
    }
#endif
}

void ym_v_sin_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvsin(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sin(input[i]);
    }
#endif
}

void ym_v_cos_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvcosf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = cosf(input[i]);
    }
#endif
}

void ym_v_cos_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvcos(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = cos(input[i]);
    }
#endif
}

void ym_v_tan_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvtanf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = tanf(input[i]);
    }
#endif
}

void ym_v_tan_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvtan(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = tan(input[i]);
    }
#endif
}

void ym_v_asin_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvasinf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = asinf(input[i]);
    }
#endif
}

void ym_v_asin_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvasin(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = asin(input[i]);
    }
#endif
}

void ym_v_acos_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvacosf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = acosf(input[i]);
    }
#endif
}

void ym_v_acos_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvacos(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = acos(input[i]);
    }
#endif
}

void ym_v_atan_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvatanf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = atanf(input[i]);
    }
#endif
}

void ym_v_atan_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvatan(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = atan(input[i]);
    }
#endif
}

void ym_v_log_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlogf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = logf(input[i]);
    }
#endif
}

void ym_v_log_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlog(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = log(input[i]);
    }
#endif
}

void ym_v_log2_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlog2f(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = log2f(input[i]);
    }
#endif
}

void ym_v_log2_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlog2(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = log2(input[i]);
    }
#endif
}

void ym_v_log10_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlog10f(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = log10f(input[i]);
    }
#endif
}

void ym_v_log10_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvlog10(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = log10(input[i]);
    }
#endif
}

void ym_v_exp_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvexpf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = expf(input[i]);
    }
#endif
}

void ym_v_exp_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvexp(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = exp(input[i]);
    }
#endif
}

void ym_v_exp2_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvexp2f(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = exp2f(input[i]);
    }
#endif
}

void ym_v_exp2_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvexp2(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = exp2(input[i]);
    }
#endif
}

void ym_v_pow_float(float *input, float exp, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvpowf(output, input, &exp, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = powf(input[i], exp);
    }
#endif
}

void ym_v_pow_double(double *input, double exp, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvpow(output, input, &exp, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = pow(input[i], exp);
    }
#endif
}

void ym_v_sqrt_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvsqrtf(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrtf(input[i]);
    }
#endif
}

void ym_v_sqrt_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    int len = length;
    vvsqrt(output, input, &len);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrt(input[i]);
    }
#endif
}

void ym_v_abs_int16(int16_t *input, int16_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] < 0 ? -input[i] : input[i];
    }
}

void ym_v_abs_int32(int32_t *input, int32_t *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vabsi(input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i] < 0 ? -input[i] : input[i];
    }
#endif
}

void ym_v_abs_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vabs(input, 1, output, 1, length);
#else
    uint32_t *in_data = (uint32_t *)input;
    uint32_t *out_data = (uint32_t *)output;
    for (uint32_t i = 0; i < length; i++) {
        out_data[i] = in_data[i] & (uint32_t)0x7FFFFFFFUL;
    }
#endif
}

void ym_v_abs_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vabsD(input, 1, output, 1, length);
#else
    uint64_t *in_data = (uint64_t *)input;
    uint64_t *out_data = (uint64_t *)output;
    for (uint32_t i = 0; i < length; i++) {
        out_data[i] = in_data[i] & (uint64_t)0x7FFFFFFFFFFFFFFFULL;
    }
#endif
}

void ym_v_scalar_add_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length) {
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] += value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] + value;
        }
    }
}

void ym_v_scalar_add_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsaddi(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] += value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] + value;
        }
    }
#endif
}

void ym_v_scalar_add_float(float *input, float *output, float value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsadd(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] += value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] + value;
        }
    }
#endif
}

void ym_v_scalar_add_double(double *input, double *output, double value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsaddD(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] += value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] + value;
        }
    }
#endif
}

void ym_v_scalar_sub_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length) {
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] -= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] - value;
        }
    }
}

void ym_v_scalar_sub_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length) {
#if YM_OS_APPLE
    value = -value;
    vDSP_vsaddi(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] -= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] - value;
        }
    }
#endif
}

void ym_v_scalar_sub_float(float *input, float *output, float value, uint32_t length) {
#if YM_OS_APPLE
    value = -value;
    vDSP_vsadd(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] -= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] - value;
        }
    }
#endif
}

void ym_v_scalar_sub_double(double *input, double *output, double value, uint32_t length) {
#if YM_OS_APPLE
    value = -value;
    vDSP_vsaddD(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] -= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] - value;
        }
    }
#endif
}

void ym_v_scalar_mul_int16(int16_t *input,  int16_t *output, int16_t value, uint32_t length) {
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * value;
        }
    }
}

void ym_v_scalar_mul_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length) {
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * value;
        }
    }
}

void ym_v_scalar_mul_float(float *input, float *output, float value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsmul(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * value;
        }
    }
#endif
}

void ym_v_scalar_mul_double(double *input, double *output, double value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsmulD(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * value;
        }
    }
#endif
}

void ym_v_scalar_div_int16(int16_t *input, int16_t *output, int16_t value, uint32_t length) {
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] /= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] / value;
        }
    }
}

void ym_v_scalar_div_int32(int32_t *input, int32_t *output, int32_t value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsdivi(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] /= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] / value;
        }
    }
#endif
}

void ym_v_scalar_div_float(float *input, float *output, float value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsdiv(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] /= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] / value;
        }
    }
#endif
}

void ym_v_scalar_div_double(double *input, double *output, double value, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsdivD(input, 1, &value, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] /= value;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] / value;
        }
    }
#endif
}

void ym_v_add_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] + input2[i];
    }
}

void ym_v_add_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length) {
#if YM_HAVE_NEON
    int32x4_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 4) {
        _in1 = vld1q_s32(&input1[i]);
        _in2 = vld1q_s32(&input2[i]);
        _out = vaddq_s32(_in1, _in2);
        vst1q_s32(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] + input2[i];
    }
#endif
}

void ym_v_add_float(float *input1, float *input2, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vadd(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] + input2[i];
    }
#endif
}

void ym_v_add_double(double *input1, double *input2, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vaddD(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] + input2[i];
    }
#endif
}

void ym_v_sub_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length) {
#if YM_HAVE_NEON
    int16x8_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 8) {
        _in1 = vld1q_s16(&input1[i]);
        _in2 = vld1q_s16(&input2[i]);
        _out = vsubq_s16(_in1, _in2);
        vst1q_s16(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] - input2[i];
    }
#endif
}

void ym_v_sub_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length) {
#if YM_HAVE_NEON
    int32x4_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 4) {
        _in1 = vld1q_s32(&input1[i]);
        _in2 = vld1q_s32(&input2[i]);
        _out = vmulq_s32(_in1, _in2);
        vst1q_s32(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] - input2[i];
    }
#endif
}

void ym_v_sub_float(float *input1, float *input2, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsub(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] - input2[i];
    }
#endif
}

void ym_v_sub_double(double *input1, double *input2, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsubD(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] - input2[i];
    }
#endif
}

void ym_v_mul_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length) {
#if YM_HAVE_NEON
    int16x8_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 8) {
        _in1 = vld1q_s16(&input1[i]);
        _in2 = vld1q_s16(&input2[i]);
        _out = vmulq_s16(_in1, _in2);
        vst1q_s16(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#endif
}

void ym_v_mul_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length) {
#if YM_HAVE_NEON
    int32x4_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 4) {
        _in1 = vld1q_s32(&input1[i]);
        _in2 = vld1q_s32(&input2[i]);
        _out = vmulq_s32(_in1, _in2);
        vst1q_s32(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#endif
}

void ym_v_mul_float(float *input1, float *input2, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vmul(input1, 1, input2, 1, output, 1, length);
#elif YM_HAVE_NEON
    float32x4_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 4) {
        _in1 = vld1q_f32(&input1[i]);
        _in2 = vld1q_f32(&input2[i]);
        _out = vmulq_f32(_in1, _in2);
        vst1q_f32(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#endif
}

void ym_v_mul_double(double *input1, double *input2, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vmulD(input1, 1, input2, 1, output, 1, length);
#elif YM_HAVE_NEON
    float64x2_t _in1, _in2, _out;
    uint32_t i;
    for (i = 0; i < length; i += 2) {
        _in1 = vld1q_f64(&input1[i]);
        _in2 = vld1q_f64(&input2[i]);
        _out = vmulq_f64(_in1, _in2);
        vst1q_f64(&output[i], _out);
    }
    for (; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] * input2[i];
    }
#endif
}

void ym_v_div_int16(int16_t *input1, int16_t *input2, int16_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] / input2[i];
    }
}

void ym_v_div_int32(int32_t *input1, int32_t *input2, int32_t *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] / input2[i];
    }
}

void ym_v_div_float(float *input1, float *input2, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vdiv(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] / input2[i];
    }
#endif
}

void ym_v_div_double(double *input1, double *input2, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vdivD(input1, 1, input2, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input1[i] / input2[i];
    }
#endif
}

void ym_v_square_float(float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsq(input, 1, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= output[i];
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * input[i];
        }
    }
#endif
}

void ym_v_square_double(double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_vsqD(input, 1, output, 1, length);
#else
    if (input == output) {
        for (uint32_t i = 0; i < length; i++) {
            output[i] *= output[i];
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            output[i] = input[i] * input[i];
        }
    }
#endif
}

void ym_v_abs_cpx_float(ym_cpx_float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    DSPSplitComplex tmp = { (float *)input, (float *)(input + length / 2) };
    vDSP_ctoz((DSPComplex *)input, 2, &tmp, 1, length);
    vDSP_zvabs((const DSPSplitComplex *)input, 1, output, 1, length);
    vDSP_ztoc(&tmp, 1, (DSPComplex *)input, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrtf(input[i].real * input[i].real + input[i].imag * input[i].imag);
    }
#endif
}

void ym_v_abs_cpx_double(ym_cpx_double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    DSPDoubleSplitComplex tmp = { (double *)input, (double *)(input + length / 2) };
    vDSP_ctozD((DSPDoubleComplex *)input, 2, &tmp, 1, length);
    vDSP_zvabsD((const DSPDoubleSplitComplex *)input, 1, output, 1, length);
    vDSP_ztocD(&tmp, 1, (DSPDoubleComplex *)input, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrt(input[i].real * input[i].real + input[i].imag * input[i].imag);
    }
#endif
}

void ym_v_abs_cpx_split_float(ym_cpx_split_float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvabs((const DSPSplitComplex *)input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrtf(input->real[i] * input->real[i] + input->imag[i] * input->imag[i]);
    }
#endif
}

void ym_v_abs_cpx_split_double(ym_cpx_split_double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvabsD((const DSPDoubleSplitComplex *)input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = sqrt(input->real[i] * input->real[i] + input->imag[i] * input->imag[i]);
    }
#endif
}

void ym_v_mag_cpx_float(ym_cpx_float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    DSPSplitComplex tmp = { (float *)input, (float *)(input + length / 2) };
    vDSP_ctoz((DSPComplex *)input, 2, &tmp, 1, length);
    vDSP_zvmags((DSPSplitComplex *)input, 1, output, 1, length);
    vDSP_ztoc(&tmp, 1, (DSPComplex *)input, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i].real * input[i].real + input[i].imag * input[i].imag;
    }
#endif
}

void ym_v_mag_cpx_double(ym_cpx_double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    DSPDoubleSplitComplex tmp = { (double *)input, (double *)(input + length / 2) };
    vDSP_ctozD((DSPDoubleComplex *)input, 2, &tmp, 1, length);
    vDSP_zvmagsD((DSPDoubleSplitComplex *)input, 1, output, 1, length);
    vDSP_ztocD(&tmp, 1, (DSPDoubleComplex *)input, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input[i].real * input[i].real + input[i].imag * input[i].imag;
    }
#endif
}

void ym_v_mag_cpx_split_float(ym_cpx_split_float *input, float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvmags((DSPSplitComplex *)input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input->real[i] * input->real[i] + input->imag[i] * input->imag[i];
    }
#endif
}

void ym_v_mag_cpx_split_double(ym_cpx_split_double *input, double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvmagsD((DSPDoubleSplitComplex *)input, 1, output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i] = input->real[i] * input->real[i] + input->imag[i] * input->imag[i];
    }
#endif
}

void ym_v_add_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length) {
#if YM_OS_APPLE
    float *in1_tmp = (float *)input1;
    float *in2_tmp = (float *)input2;
    float *out_tmp = (float *)output;
    vDSP_vadd(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
    in1_tmp = in1_tmp + 1;
    in2_tmp = in2_tmp + 1;
    out_tmp = out_tmp + 1;
    vDSP_vadd(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real + input2[i].real;
        output[i].imag = input1[i].imag + input2[i].imag;
    }
#endif
}

void ym_v_add_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length) {
#if YM_OS_APPLE
    double *in1_tmp = (double *)input1;
    double *in2_tmp = (double *)input2;
    double *out_tmp = (double *)output;
    vDSP_vaddD(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
    in1_tmp = in1_tmp + 1;
    in2_tmp = in2_tmp + 1;
    out_tmp = out_tmp + 1;
    vDSP_vaddD(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real + input2[i].real;
        output[i].imag = input1[i].imag + input2[i].imag;
    }
#endif
}
void ym_v_add_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvadd((DSPSplitComplex *)input1, 1, (DSPSplitComplex *)input2, 1, (DSPSplitComplex *)output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] + input2->real[i];
        output->imag[i] = input1->imag[i] + input2->imag[i];
    }
#endif
}

void ym_v_add_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvaddD((DSPDoubleSplitComplex *)input1, 1, (DSPDoubleSplitComplex *)input2, 1, (DSPDoubleSplitComplex *)output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] + input2->real[i];
        output->imag[i] = input1->imag[i] + input2->imag[i];
    }
#endif
}

void ym_v_sub_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length) {
#if YM_OS_APPLE
    float *in1_tmp = (float *)input1;
    float *in2_tmp = (float *)input2;
    float *out_tmp = (float *)output;
    vDSP_vsub(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
    in1_tmp = in1_tmp + 1;
    in2_tmp = in2_tmp + 1;
    out_tmp = out_tmp + 1;
    vDSP_vsub(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real - input2[i].real;
        output[i].imag = input1[i].imag - input2[i].imag;
    }
#endif
}

void ym_v_sub_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length) {
#if YM_OS_APPLE
    double *in1_tmp = (double *)input1;
    double *in2_tmp = (double *)input2;
    double *out_tmp = (double *)output;
    vDSP_vsubD(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
    in1_tmp = in1_tmp + 1;
    in2_tmp = in2_tmp + 1;
    out_tmp = out_tmp + 1;
    vDSP_vsubD(out_tmp, 2, in1_tmp, 2, in2_tmp, 2, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real - input2[i].real;
        output[i].imag = input1[i].imag - input2[i].imag;
    }
#endif
}

void ym_v_sub_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvsub((DSPSplitComplex *)input1, 1, (DSPSplitComplex *)input2, 1, (DSPSplitComplex *)output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] - input2->real[i];
        output->imag[i] = input1->imag[i] - input2->imag[i];
    }
#endif
}

void ym_v_sub_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvsubD((DSPDoubleSplitComplex *)input1, 1, (DSPDoubleSplitComplex *)input2, 1, (DSPDoubleSplitComplex *)output, 1, length);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] - input2->real[i];
        output->imag[i] = input1->imag[i] - input2->imag[i];
    }
#endif
}

void ym_v_mul_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real * input2[i].real - input1[i].imag * input2[i].imag;
        output[i].imag = input1[i].imag * input2[i].real + input1[i].real * input2[i].imag;
    }
}

void ym_v_mul_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        output[i].real = input1[i].real * input2[i].real - input1[i].imag * input2[i].imag;
        output[i].imag = input1[i].imag * input2[i].real + input1[i].real * input2[i].imag;
    }
}

void ym_v_mul_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvmul((DSPSplitComplex *)input1, 1, (DSPSplitComplex *)input2, 1, (DSPSplitComplex *)output, 1, length, 1);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] * input2->real[i] - input1->imag[i] * input2->imag[i];
        output->imag[i] = input1->imag[i] * input2->real[i] + input1->real[i] * input2->imag[i];
    }
#endif
}

void ym_v_mul_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length) {
#if YM_OS_APPLE
    vDSP_zvmulD((DSPDoubleSplitComplex *)input1, 1, (DSPDoubleSplitComplex *)input2, 1, (DSPDoubleSplitComplex *)output, 1, length, 1);
#else
    for (uint32_t i = 0; i < length; i++) {
        output->real[i] = input1->real[i] * input2->real[i] - input1->imag[i] * input2->imag[i];
        output->imag[i] = input1->imag[i] * input2->real[i] + input1->real[i] * input2->imag[i];
    }
#endif
}

void ym_v_div_cpx_float(ym_cpx_float *input1, ym_cpx_float *input2, ym_cpx_float *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        float tmp = input2[i].real * input2[i].real + input2[i].imag * input2[i].imag;
        output[i].real = (input1[i].real * input2[i].real + input1[i].imag * input2[i].imag) / tmp;
        output[i].imag = (input1[i].imag * input2[i].real - input1[i].real * input2[i].imag) / tmp;
    }
}

void ym_v_div_cpx_double(ym_cpx_double *input1, ym_cpx_double *input2, ym_cpx_double *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        float tmp = input2[i].real * input2[i].real + input2[i].imag * input2[i].imag;
        output[i].real = (input1[i].real * input2[i].real + input1[i].imag * input2[i].imag) / tmp;
        output[i].imag = (input1[i].imag * input2[i].real - input1[i].real * input2[i].imag) / tmp;
    }
}

void ym_v_div_cpx_split_float(ym_cpx_split_float *input1, ym_cpx_split_float *input2, ym_cpx_split_float *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        float tmp = input2->real[i] * input2->real[i] + input2->imag[i] * input2->imag[i];
        output->real[i] = (input1->real[i] * input2->real[i] + input1->imag[i] * input2->imag[i]) / tmp;
        output->imag[i] = (input1->imag[i] * input2->real[i] - input1->real[i] * input2->imag[i]) / tmp;
    }
}

void ym_v_div_cpx_split_double(ym_cpx_split_double *input1, ym_cpx_split_double *input2, ym_cpx_split_double *output, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        double tmp = input2->real[i] * input2->real[i] + input2->imag[i] * input2->imag[i];
        output->real[i] = (input1->real[i] * input2->real[i] + input1->imag[i] * input2->imag[i]) / tmp;
        output->imag[i] = (input1->imag[i] * input2->real[i] - input1->real[i] * input2->imag[i]) / tmp;
    }
}
