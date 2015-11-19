//
//  ym_complex.h
//  yy_music
//
//  Created by ibireme on 15/1/12.
//  Copyright (c) 2015 ibireme. All rights reserved.
//

#ifndef __yy_music__ym_complex__
#define __yy_music__ym_complex__

#include "ym_base.h"


typedef struct {
    float real;
    float imag;
} ym_cpx_float;

typedef struct {
    double real;
    double imag;
} ym_cpx_double;

typedef struct {
    float *real;
    float *imag;
} ym_cpx_split_float;

typedef struct {
    double *real;
    double *imag;
} ym_cpx_split_double;


ym_inline ym_cpx_float ym_cpx_make_float(float real, float imag) {
    return (ym_cpx_float) { real, imag };
}

ym_inline ym_cpx_double ym_cpx_make_double(double real, double imag) {
    return (ym_cpx_double) { real, imag };
}


/// cpx a + b
ym_inline ym_cpx_float ym_cpx_add_float(ym_cpx_float a, ym_cpx_float b) {
    ym_cpx_float r;
    r.real = a.real + b.real;
    r.imag = b.imag + b.imag;
    return r;
}

/// cpx a - b
ym_inline ym_cpx_float ym_cpx_sub_float(ym_cpx_float a, ym_cpx_float b) {
    ym_cpx_float r;
    r.real = a.real - b.real;
    r.imag = b.imag - b.imag;
    return r;
}

/// cpx a * b
ym_inline ym_cpx_float ym_cpx_mul_float(ym_cpx_float a, ym_cpx_float b) {
    ym_cpx_float r;
    r.real = a.real * b.real - a.imag * b.imag;
    r.imag = a.imag * b.real + a.real * b.imag;
    return r;
}

/// cpx a / b
ym_inline ym_cpx_float ym_cpx_div_float(ym_cpx_float a, ym_cpx_float b) {
    ym_cpx_float r;
    float bb = b.real * b.real + b.imag * b.imag;
    r.real = (a.real * b.real + a.imag * b.imag) / bb;
    r.imag = (a.imag * b.real - a.real * b.imag) / bb;
    return r;
}

/// cpx abs(v)
ym_inline float ym_cpx_abs_float(ym_cpx_float v) {
    return sqrt(v.real * v.real + v.imag * v.imag);
}

/// epx exp(v)
ym_inline ym_cpx_float ym_cpx_exp_float(ym_cpx_float v) {
    ym_cpx_float r;
    r.real = exp(v.real) * cos(v.imag);
    r.imag = exp(v.real) * sin(v.imag);
    return r;
}



/*
 
 http://www.opensource.apple.com/source/Libm/Libm-92/complex.c
 */


#endif
