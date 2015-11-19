//
//  ym_math.h
//  yy_music
//
//  Created by ibireme on 14-2-27.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __YM_MATH__
#define __YM_MATH__

#include "ym_base.h"

YM_EXTERN_C_BEGIN


/**
 分贝(这里是更小的cB) 转换成 amplitude (振幅衰减)
 可以做成查表的
 
 振幅转换为dB
 G(dB) = 20 * log10 (A1/A0)
 A1: amp
 A0: amp refered
 G : gain in dB
 
 dB转换位振幅
 A1 = A0 * 10^(G / 20)
 A1: amp
 A0: refer amp
 G : gain in dB
 
 (16 bit = 96 dB, 24 bit = 144 dB).
 */
ym_inline double ym_math_cb_to_amp(double cb){
    return exp(cb * M_LN10 * -0.005); // 10^(cB/-200)
}

ym_inline double ym_math_db_to_amp_ratio(double dB) {
    return exp(dB * M_LN10 * 0.05); // 10^(db/20)
}

ym_inline double ym_math_amp_ratio_to_db(double amp_ratio) {
    return 20.0 * log10(amp_ratio);
}



//ym_inline float timecent_to_second(int timecent) {
//    return powf(2.0f, timecent / 1200.0f);
//}
//
//ym_inline float timecent_per_key_to_second(int timecent, int timecent_per_key, int keynum) {
//    return powf(2.0f, ((60 - keynum) * timecent_per_key + timecent) / 1200.0f);
//}


ym_inline double ym_math_pitch_time_to_second(double pitch_time) {
    return pow(2.0, pitch_time / 12.0);
}

ym_inline double ym_math_second_to_pitch_time(double second) {
    return 12.0 * log2(second);
}

ym_inline double ym_math_second_add_ratio_per_key(double second, double ratio_per_key, uint16_t key) {
    return second * pow(ratio_per_key, 60 - key);
}

ym_inline double ym_math_second_ratio_per_key(double ratio_per_key, uint16_t key) {
    return pow(ratio_per_key, 60 - key);
}


/////
//ym_inline double ym_math_per_key_pitch_to_second(double second, int keynum, double pitch_time_per_key) {
//    return second * ym_math_pitch_time_to_second((60-keynum) * pitch_time_per_key);
//}


//ym_inline double ym_math_abscent_to_freq(double abscent) {
//    return 8.1757989 * pow(2.0, abscent / 1200.0);
//}
//
//ym_inline double ym_math_freq_to_abscent(double freq) {
//    return 1200.0 * log2(freq / 8.1757989);
//}
//
//
//ym_inline double ym_math_abscent_to_pitch(double abscent) {
//    return abscent / 100.0;
//}
//
//ym_inline double ym_math_pitch_to_abscent(double pitch) {
//    return pitch * 100.0;
//}


/**
 * pitch to frequency
 * example: A4 pitch number: 69, frequency: 440.0Hz.
 * http://en.wikipedia.org/wiki/Pitch_(music)
 *
 * @param pitch from 0 to 127
 * @return frequency
 */

ym_inline double ym_math_pitch_to_freq(double pitch) {
    return 440.0 * pow(2.0, (pitch - 69.0) / 12.0);
}

/**
 * frequency to pitch
 * example: frequency: 440.0Hz, pitch number: 69 (A4).
 * http://en.wikipedia.org/wiki/Pitch_(music)
 *
 * @param freq frequency
 * @return pitch number
 */
ym_inline double ym_math_freq_to_pitch(double freq) {
    return 69.0 + 12.0 * log2(freq / 440.0);
}


/**
 *
 * @param pan  [-1~1] -1:full left 1:full right 0:center
 * @param side -1:left gain, 1:right gain
 *
 * @return 0.0~1.0 gain
 */
ym_inline double ym_math_pan_to_amp_ratio(double pan, int side) {
    return sin(M_PI_4 * (pan * side + M_PI_4));
}




void ym_math_double_to_float80(double num, uint8_t *bytes); // 10byte
double ym_math_float80_to_double(uint8_t *bytes); // 10byte


YM_EXTERN_C_END
#endif
