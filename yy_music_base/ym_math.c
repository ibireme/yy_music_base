//
//  ym_math.c
//  yy_music
//
//  Created by ibireme on 14-2-27.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_math.h"
#include "ym_util.h"

typedef struct {
    int8_t sign;
    uint16_t exp;
    uint32_t hi;
    uint32_t lo;
} ym_float80;


void ym_math_double_to_float80(double num, uint8_t *bytes) {
    ym_float80 f80;
    int exp;
    double mant;
    uint16_t sign;
    
    if (bytes == NULL) return;
    
    mant = frexp(num, &exp);
    sign = mant < 0 ? 0x8000 : 0;
    if (mant < 0) mant = -mant;
    
    if (mant == 0) {
        f80.exp = 0; f80.hi = 0; f80.lo = 0;
    } else if ((exp > 0x4000) || !(mant < 1)) { // Infinity or NaN
        f80.exp = sign | 0x7FFF; f80.hi = 0; f80.lo = 0;
    } else {
        exp += 0x3FFF - 1;
        if (exp < 0) { // denormalized
            mant = ldexp(mant, exp);
            exp = 0;
        }
        f80.exp = exp | sign;
        mant = ldexp(mant, 0x20);
        f80.hi = floor(mant);
        mant = ldexp(mant - f80.hi, 0x20);
        f80.lo = floor(mant);
    }
    
    *(uint16_t *)&bytes[0] = ym_swap_endian_uint16(f80.exp);
    *(uint32_t *)&bytes[2] = ym_swap_endian_uint32(f80.hi);
    *(uint32_t *)&bytes[6] = ym_swap_endian_uint32(f80.lo);
}

double ym_math_float80_to_double(uint8_t *bytes) {
    ym_float80 f80;
    
    if (bytes == NULL) return 0;
    
    f80.sign = (bytes[0] & 0x80) ? -1 : 1;
    f80.exp = ym_swap_endian_uint16(*(uint16_t *)&bytes[0]) & 0x7FFF;
    f80.hi = ym_swap_endian_uint32(*(uint32_t *)&bytes[2]);
    f80.lo = ym_swap_endian_uint32(*(uint32_t *)&bytes[6]);
    
    if (f80.exp == 0 && f80.hi == 0 && f80.lo == 0) return 0;
    if (f80.exp == 0x7FFF) return INFINITY;
    return f80.sign *
    (ldexp(f80.hi, f80.exp - 0x3FFF - 0x1F) +
     ldexp(f80.lo, f80.exp - 0x3FFF - 0x3F));
}
