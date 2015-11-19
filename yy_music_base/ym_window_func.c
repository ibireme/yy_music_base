//
//  ym_window_func.c
//  yy_music
//
//  Created by ibireme on 14/12/31.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_window_func.h"
#include "ym_log.h"


static double _ym_cheby_poly(int n, double x) {
    return (fabs(x) <= 1) ? cos(n * acos(x)) : cosh(n * acosh(x));
}


static double _ym_bessel_i0(double x) {
#define BIZ_EPSILON 1E-21 // Max error acceptable
    double temp;
    double sum = 1.0;
    double u = 1.0;
    double halfx = x / 2.0;
    int n = 1;
    do {
        temp = halfx / (double)n;
        u *= temp * temp;
        sum += u;
        n++;
    } while (u >= BIZ_EPSILON * sum);
    return sum;
#undef BIZ_EPSILON
}






void ym_window_function_data_float(ym_window_type type, float *data, uint32_t length) {
    float alpha;
    switch (type) {
        case YM_WINDOW_TYPE_Kaiser: alpha = 2; break;
        case YM_WINDOW_TYPE_Gaussian: alpha = 2.5; break;
        case YM_WINDOW_TYPE_Dolph_Chebyshev: alpha = 5; break;
        default: alpha = 0; break;
    }
    ym_window_function_data_float_with_option(type, data, length, true, alpha);
}


void ym_window_function_data_float_with_option(ym_window_type type, float *data, uint32_t length, bool is_symmetric, float alpha) {
    
    float size = is_symmetric ? length - 1 : length;
    switch (type) {
        case YM_WINDOW_TYPE_None: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 1;
            }
        } break;
        case YM_WINDOW_TYPE_Bartlett: {
            if (is_symmetric) {
                for (uint32_t i = 0; i < length / 2; i++) {
                    data[i] = (i / (size / 2));
                    data[length - 1 - i] = (i / (size / 2));
                }
            } else {
                for (uint32_t i = 0; i < length / 2; i++) {
                    data[i] = (i / (length / 2));
                    data[i + (length / 2)] = (1.0 - (i / (length / 2)));
                }
            }
        } break;
            
        case YM_WINDOW_TYPE_Hamming: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.54 - 0.46 * cos(2 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Hanning: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.5 - 0.5 * cos(2 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Welch: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 4 * i / (float)length * (1 - (i / size));
            }
        } break;
            
        case YM_WINDOW_TYPE_Blackman: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.42 -
                0.50 * cos(2 * M_PI * i / size) +
                0.08 * cos(4 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Blackman_Harris: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.35875 -
                0.48829 * cos(2 * M_PI * i / size) +
                0.14128 * cos(4 * M_PI * i / size) -
                0.01168 * cos(6 * M_PI * i / size);
            }
        } break;
        case YM_WINDOW_TYPE_Blackman_Nuttall: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.3635819 -
                0.4891775 * cos(2 * M_PI * i / size) +
                0.1365995 * cos(4 * M_PI * i / size) -
                0.0106411 * cos(6 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Nuttal: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.355768 -
                0.487396 * cos(2 * M_PI * i / size) +
                0.114232 * cos(4 * M_PI * i / size) -
                0.012604 * cos(6 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Flat_top: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.215578995 -
                0.416631580 * cos(2 * M_PI * i / size) +
                0.277263158 * cos(4 * M_PI * i / size) -
                0.083578947 * cos(6 * M_PI * i / size) +
                0.006947368 * cos(8 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Kaiser: {
            for (uint32_t i = 0; i < length; i++) {
                double m = M_PI * alpha * sqrt(1.0 - pow(2 * i / size - 1, 2.0));
                data[i] = _ym_bessel_i0(m) / _ym_bessel_i0(M_PI * alpha);
            }
        } break;
            
        case YM_WINDOW_TYPE_Parzen: {
            for (uint32_t i = 0; i < length; i++) {
                double k = i - size / 2;
                double a = 2 * fabs(k) / length;
                if (fabs(k) <= size / 4) {
                    data[i] = 1 - 6 * pow(a, 2) + 6 * pow(a, 3);
                } else {
                    data[i] = 2 * pow((1 - a), 3);
                }
            }
        } break;
            
        case YM_WINDOW_TYPE_Lanczos: {
            for (uint32_t i = 0; i < length; i++) {
                double x =  2 * i / size - 1;
                double sinc = (x < 1e-6 && x > -1e-6) ? 1 : sin(M_PI * x) / (M_PI * x);
                data[i] = sinc;
            }
        } break;
            
        case YM_WINDOW_TYPE_Gaussian: {
            double a = -2 * alpha * alpha;
            for (uint32_t i = 0; i < length; i++) {
                data[i] = exp(a * (0.25 + ((i / size) * (i / size)) - (i / size)));
            }
        } break;
            
        case YM_WINDOW_TYPE_Dolph_Chebyshev: {
            //double alpha = 6; //6:-120dB 5:-100dB
            double tg = pow(10, alpha);
            double b = cosh(acosh(tg) / size);
            double M = size / 2.0;
            
            double max = 0;
            for (int i = 0; i < length; i++) {
                double sum = 0;
                for (int k = 1; k <= M; k++) {
                    sum += _ym_cheby_poly(size, b * cos(M_PI * k / length)) * cos(2.0 * (i - M) * M_PI * k / length);
                }
                data[i] = tg + 2 * sum;
                if (data[i] > max) max = data[i];
            }
            for (int i = 0; i < length; i++) data[i] /= max; //normalise
        } break;
            
        default: {
            ym_log_error("ym_window_func: invalid window type:%d\n",type);
        } break;
    }
}



/* ---------------------------- copy for double ------------------------------ */


void ym_window_function_data_double(ym_window_type type, double *data, uint32_t length) {
    double alpha;
    switch (type) {
        case YM_WINDOW_TYPE_Kaiser: alpha = 2; break;
        case YM_WINDOW_TYPE_Gaussian: alpha = 2.5; break;
        case YM_WINDOW_TYPE_Dolph_Chebyshev: alpha = 5; break;
        default: alpha = 0; break;
    }
    ym_window_function_data_double_with_option(type, data, length, true, alpha);
}


void ym_window_function_data_double_with_option(ym_window_type type, double *data, uint32_t length, bool is_symmetric, double alpha) {
    
    double size = is_symmetric ? length - 1 : length;
    switch (type) {
        case YM_WINDOW_TYPE_None: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 1;
            }
        } break;
        case YM_WINDOW_TYPE_Bartlett: {
            if (is_symmetric) {
                for (uint32_t i = 0; i < length / 2; i++) {
                    data[i] = (i / (size / 2));
                    data[length - 1 - i] = (i / (size / 2));
                }
            } else {
                for (uint32_t i = 0; i < length / 2; i++) {
                    data[i] = (i / (length / 2));
                    data[i + (length / 2)] = (1.0 - (i / (length / 2)));
                }
            }
        } break;
            
        case YM_WINDOW_TYPE_Hamming: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.54 - 0.46 * cos(2 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Hanning: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.5 - 0.5 * cos(2 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Welch: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 4 * i / (double)length * (1 - (i / size));
            }
        } break;
            
        case YM_WINDOW_TYPE_Blackman: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.42 -
                0.50 * cos(2 * M_PI * i / size) +
                0.08 * cos(4 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Blackman_Harris: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.35875 -
                0.48829 * cos(2 * M_PI * i / size) +
                0.14128 * cos(4 * M_PI * i / size) -
                0.01168 * cos(6 * M_PI * i / size);
            }
        } break;
        case YM_WINDOW_TYPE_Blackman_Nuttall: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.3635819 -
                0.4891775 * cos(2 * M_PI * i / size) +
                0.1365995 * cos(4 * M_PI * i / size) -
                0.0106411 * cos(6 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Nuttal: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.355768 -
                0.487396 * cos(2 * M_PI * i / size) +
                0.114232 * cos(4 * M_PI * i / size) -
                0.012604 * cos(6 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Flat_top: {
            for (uint32_t i = 0; i < length; i++) {
                data[i] = 0.215578995 -
                0.416631580 * cos(2 * M_PI * i / size) +
                0.277263158 * cos(4 * M_PI * i / size) -
                0.083578947 * cos(6 * M_PI * i / size) +
                0.006947368 * cos(8 * M_PI * i / size);
            }
        } break;
            
        case YM_WINDOW_TYPE_Kaiser: {
            for (uint32_t i = 0; i < length; i++) {
                double m = M_PI * alpha * sqrt(1.0 - pow(2 * i / size - 1, 2.0));
                data[i] = _ym_bessel_i0(m) / _ym_bessel_i0(M_PI * alpha);
            }
        } break;
            
        case YM_WINDOW_TYPE_Parzen: {
            for (uint32_t i = 0; i < length; i++) {
                double k = i - size / 2;
                double a = 2 * fabs(k) / length;
                if (fabs(k) <= size / 4) {
                    data[i] = 1 - 6 * pow(a, 2) + 6 * pow(a, 3);
                } else {
                    data[i] = 2 * pow((1 - a), 3);
                }
            }
        } break;
            
        case YM_WINDOW_TYPE_Lanczos: {
            for (uint32_t i = 0; i < length; i++) {
                double x =  2 * i / size - 1;
                double sinc = (x < 1e-6 && x > -1e-6) ? 1 : sin(M_PI * x) / (M_PI * x);
                data[i] = sinc;
            }
        } break;
            
        case YM_WINDOW_TYPE_Gaussian: {
            double a = -2 * alpha * alpha;
            for (uint32_t i = 0; i < length; i++) {
                data[i] = exp(a * (0.25 + ((i / size) * (i / size)) - (i / size)));
            }
        } break;
            
        case YM_WINDOW_TYPE_Dolph_Chebyshev: {
            //double alpha = 6; //6:-120dB 5:-100dB
            double tg = pow(10, alpha);
            double b = cosh(acosh(tg) / size);
            double M = size / 2.0;
            
            double max = 0;
            for (int i = 0; i < length; i++) {
                double sum = 0;
                for (int k = 1; k <= M; k++) {
                    sum += _ym_cheby_poly(size, b * cos(M_PI * k / length)) * cos(2.0 * (i - M) * M_PI * k / length);
                }
                data[i] = tg + 2 * sum;
                if (data[i] > max) max = data[i];
            }
            for (int i = 0; i < length; i++) data[i] /= max; //normalise
        } break;
            
        default: {
            ym_log_error("ym_window_func: invalid window type:%d\n",type);
        } break;
    }
}
