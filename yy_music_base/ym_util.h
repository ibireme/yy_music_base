//
//  ym_util.h
//  yy_music
//
//  Created by  on 14-9-17.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_util__
#define __ym_util__

#include "ym_base.h"

YM_EXTERN_C_BEGIN



#pragma mark - Compare

#undef  YM_MAX
#define YM_MAX(_a_, _b_)  (((_a_) > (_b_)) ? (_a_) : (_b_))

#undef  YM_MIN
#define YM_MIN(_a_, _b_)  (((_a_) < (_b_)) ? (_a_) : (_b_))

#undef  YM_ABS
#define YM_ABS(_x_)  (((_x_) < 0) ? -(_a_) : (_a_))

#undef  YM_CLAMP
#define YM_CLAMP(_x_, _min_, _max_)  (((_x_) > (_max_)) ? (_max_) : (((_x_) < (_min_)) ? (_min_) : (_x_)))

#undef  YM_SWAP
#define YM_SWAP(_a_, _b_)  do { __typeof__(_a_) _tmp_ = (_a_); (_a_) = (_b_); (_b_) = _tmp_; } while (0)



#pragma mark - Endian

ym_inline uint16_t ym_swap_endian_uint16(uint16_t value) {
    return
    (uint16_t) ((value & 0x00FF) << 8) |
    (uint16_t) ((value & 0xFF00) >> 8) ;
}

ym_inline uint32_t ym_swap_endian_uint32(uint32_t value) {
    return
    (uint32_t)((value & 0x000000FFU) << 24) |
    (uint32_t)((value & 0x0000FF00U) <<  8) |
    (uint32_t)((value & 0x00FF0000U) >>  8) |
    (uint32_t)((value & 0xFF000000U) >> 24) ;
}

ym_inline uint64_t ym_swap_endian_uint64(uint64_t value) {
    return
    (uint64_t)((value << 56) & 0xFF00000000000000ULL) |
    (uint64_t)((value << 40) & 0x00FF000000000000ULL) |
    (uint64_t)((value << 24) & 0x0000FF0000000000ULL) |
    (uint64_t)((value <<  8) & 0x000000FF00000000ULL) |
    (uint64_t)((value >>  8) & 0x00000000FF000000ULL) |
    (uint64_t)((value >> 24) & 0x0000000000FF0000ULL) |
    (uint64_t)((value >> 40) & 0x000000000000FF00ULL) |
    (uint64_t)((value >> 56) & 0x00000000000000FFULL) ;
}

ym_inline int16_t ym_swap_endian_int16(int16_t value) {
    uint16_t tmp = *(uint16_t *)&value;
    tmp = ym_swap_endian_uint16(tmp);
    return *(int16_t *)&tmp;
}

ym_inline int32_t ym_swap_endian_int32(int32_t value) {
    uint32_t tmp = *(uint32_t *)&value;
    tmp = ym_swap_endian_uint32(value);
    return *(int32_t *)&tmp;
}

ym_inline int64_t ym_swap_endian_int64(int64_t value) {
    uint64_t tmp = *(uint64_t *)&value;
    tmp = ym_swap_endian_uint64(value);
    return *(int64_t *)&tmp;
}

ym_inline float ym_swap_endian_float(float value) {
    uint32_t tmp = *(uint32_t *)&value;
    tmp = ym_swap_endian_uint32(tmp);
    return *(float *)&tmp;
}

ym_inline double ym_swap_endian_double(double value) {
    uint64_t tmp = *(uint64_t *)&value;
    tmp = ym_swap_endian_uint64(tmp);
    return *(double *)&tmp;
}





#pragma mark - Four Char

/**
 Multi-character literal (revert order in hex editor)
 http://stackoverflow.com/questions/7459939/
 */
ym_inline uint32_t ym_char_literal(const char* char4) {
    return (char4[0]<<24) + (char4[1]<<16) + (char4[2]<<8) + char4[3];
}

/**
 Compile time function
 */
#define YM_CHAR_LITERAL(c1,c2,c3,c4) ((uint32_t)(((c1) << 24) | ((c2) << 16) | ((c3) << 8) | (c4)))

/**
 FourCC (as is show in hex editor)
 http://en.wikipedia.org/wiki/FourCC
 */
ym_inline uint32_t ym_four_cc(const char *char4) {
    return ((char4[3]<<24) | (char4[2]<<16) | (char4[1]<<8) | char4[0]);
}

/**
 Compile time function
 */
#define YM_FOUR_CC(c1,c2,c3,c4) ((uint32_t)(((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1)))





#pragma mark - Variable-length quantity
// http://en.wikipedia.org/wiki/Variable-length_quantity

/**
 convert number to variable-length qantity
 
 @param num        range: [0,0x0FFFFFFF].
 @param vlq        destination memory, should not smaller than 4 byte.
 @param vlq_length vlq's length in byte.
 @return true:success false:invalid parameters.
 */
bool ym_uint32_to_vlq(uint32_t num, uint8_t *vlq, int *vlq_length);

/**
 convert number to variable-length qantity
 
 @param num        range: [0,0x00FFFFFFFFFFFFFF].
 @param vlq        destination memory, should not smaller than 8 byte.
 @param vlq_length vlq's length in byte.
 @return true:success false:invalid parameters.
 */
bool ym_uint64_to_vlq(uint64_t num, uint8_t *vlq, int *vlq_length);

/**
 convert variable-length qantity to number
 
 @param vlq        vlq pointer.
 @param num        range: [0,0x0FFFFFFF].
 @param vlq_length vlq's length in byte.
 @return true:success false:invalid vlq.
 */
bool ym_vlq_to_uint32(uint8_t *vlq, uint32_t *num, int *vlq_length);

/**
 convert variable-length qantity to number
 
 @param vlq        vlq pointer.
 @param num        range: [0,0x00FFFFFFFFFFFFFF].
 @param vlq_length vlq's length in byte.
 @return true:success false:invalid vlq.
 */
bool ym_vlq_to_uint64(uint8_t *vlq, uint64_t *num, int *vlq_length);



#pragma mark - Time

/// Formatted time string, e.g."2014-12-31 23:59:59". call free() after used.
char *ym_current_time_str();

/// Unix timestamp (since 1970) in seconds (may affected by system clock).
double ym_current_timestamp();

/// Unix timestamp (since 1970) in milliseconds (may affected by system clock).
int64_t ym_current_timestamp_ms();

/// Current monotonic clock in milliseconds (not affected by system changes, used for time offsets) (1/1000) .
int64_t ym_clock_ms();

/// Current monotonic clock in microseconds (not affected by system changes, used for time offsets) (1/1000000) .
int64_t ym_clock_us();

/// Sleep for a number of milliseconds (1/1000).
void ym_sleep_ms(uint32_t msecs);



#pragma mark - File

typedef struct {
    uint64_t size;      ///< size in byte
    double time;        ///< changed time in unix second
    bool is_exist;      ///< false: file not exist or error occurred
    bool is_directory;  ///< is directory
    bool is_file;       ///< is file
    bool is_readable;   ///< is readable
    bool is_writable;   ///< is writable
    bool is_executable; ///< is executable
} ym_path_info;

ym_path_info ym_util_get_path_info(const char *path);
bool ym_util_create_file(const char *path);
bool ym_util_delete_file(const char *path);
bool ym_util_create_dir(const char *path);
bool ym_util_delete_dir(const char *path);

#pragma mark - Others

/// Avaliable CPU core count.
long ym_cpu_available_core_count();


YM_EXTERN_C_END
#endif
