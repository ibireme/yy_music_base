//
//  ym_def.h
//  yy_music
//
//  Created by ibireme on 14-2-13.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_base__
#define __ym_base__


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <limits.h>


#include "ym_define.h"


#ifdef __cplusplus
    #define YM_EXTERN_C_BEGIN  extern "C" {
    #define YM_EXTERN_C_END  }
#else
    #define YM_EXTERN_C_BEGIN
    #define YM_EXTERN_C_END
#endif

YM_EXTERN_C_BEGIN



#pragma mark - Define

#if !defined(ym_inline)
    #if defined(__GNUC__) && (__GNUC__ == 4) && !defined(DEBUG)
        #define ym_inline static __inline__ __attribute__((always_inline))
    #elif defined(__GNUC__)
        #define ym_inline static __inline__
    #elif defined(__cplusplus)
        #define ym_inline static inline
    #elif defined(_MSC_VER)
        #define ym_inline static __inline
    #elif YM_OS_WINDOWS
        #define ym_inline static __inline__
    #else
        #define ym_inline static inline
    #endif
#endif



#if !defined(ym_export)
    #if YM_OS_WINDOWS
        #if defined(__cplusplus)
            #define ym_export extern "C" __declspec(dllimport)
        #else
            #define ym_export extern __declspec(dllimport)
        #endif
    #else
        #define CF_EXPORT extern
    #endif
#endif








#pragma mark - Type

#if YM_IS_64_BIT
typedef double          ym_float;
typedef long            ym_int;
typedef unsigned long   ym_uint;
#define YM_FLOAT_MAX    DBL_MAX
#define YM_FLOAT_MIN    DBL_MIN
#define YM_INT_MAX      LONG_MAX
#define YM_INT_MIN      LONG_MIN
#define YM_UINT_MAX     ULONG_MAX
#define YM_UINT_MIN     ULONG_MIN
#else
typedef double          ym_float;
typedef int             ym_int;
typedef unsigned int    ym_uint;
#define YM_FLOAT_MAX    FLT_MAX
#define YM_FLOAT_MIN    FLT_MIN
#define YM_INT_MAX      INT_MAX
#define YM_INT_MIN      INT_MIN
#define YM_UINT_MAX     UINT_MAX
#define YM_UINT_MIN     UINT_MIN
#endif



#pragma mark - Endian

typedef enum {
    YM_ENDIAN_ANY = 0, ///< any / unknown
    YM_ENDIAN_SMALL,   ///< small endian
    YM_ENDIAN_BIG,     ///< big endian
} ym_endian;


ym_inline bool ym_endian_need_swap(ym_endian endian) {
#if YM_IS_BIG_ENDIAN
    return endian == YM_ENDIAN_SMALL;
#else
    return endian == YM_ENDIAN_BIG;
#endif
}


#pragma mark - Range

typedef struct {
    ym_uint location;
    ym_uint length;
} ym_range;

ym_inline ym_range ym_range_make(ym_uint location, ym_uint length) {
    ym_range range;
    range.location = location;
    range.length = length;
    return range;
}

enum {
    YM_NOT_FOUND = YM_UINT_MAX,
};



#pragma mark - Order

typedef enum {
    YM_ORDER_ASC = -1,
    YM_ORDER_EQUAL = 0,
    YM_ORDER_DESC = 1,
} ym_order;

typedef ym_order (*ym_comparator_func)(const void *value1, const void *value2, void *context);



#pragma mark - Error

typedef enum {
    YM_ERROR_NONE = 0,              ///< no error
    YM_ERROR_INVALID_PARAMS,        ///< function param error
    YM_ERROR_ALLOC_MEMORY,          ///< alloc memory error
    YM_ERROR_FILE_CREATE,           ///< fail to create file
    YM_ERROR_FILE_EXIST,            ///< file already exist
    YM_ERROR_FILE_OPEN,             ///< fail to open file
    YM_ERROR_FILE_READ,             ///< fail to read file
    YM_ERROR_FILE_WRITE,            ///< fail to write file
    YM_ERROR_FILE_FORMAT,           ///< unexpect file format
    YM_ERROR_FILE_FORMAT_TYPE,      ///< file format version/type not support
    YM_ERROR_FILE_FORMAT_CONTENT,   ///< file format content error
    YM_ERROR_UNKNOWN = -1,          ///< unknown error
} ym_error_code;



#pragma mark - Object

typedef void * ym_object_ref;

ym_object_ref ym_retain(ym_object_ref object);
ym_object_ref ym_release(ym_object_ref object);
long ym_retain_count(ym_object_ref object);

ym_object_ref _ym_alloc(size_t size, void (*dealloc)(ym_object_ref));
#define ym_alloc(type, dealloc) _ym_alloc(sizeof(type),(void(*)(ym_object_ref))(dealloc))


/**
 NOTE:
 ym_xxx_ref:        object reference
 ym_xxx_func:       function pointer
 ym_xxx_t:          struct
 ym_xxx_callback:   callback (a bunch of function pointer)
 */


YM_EXTERN_C_END
#endif
