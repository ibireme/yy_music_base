//
//  ym_config.h
//  yy_music
//
//  Created by ibireme on 14-2-13.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_define__
#define __ym_define__


// OS
#define YM_OS_WINDOWS 0
#define     YM_OS_WIN32 0
#define     YM_OS_WIN64 0
#define YM_OS_LINUX 0
#define     YM_OS_ANDROID 0
#define YM_OS_APPLE 0
#define     YM_OS_OSX 0
#define     YM_OS_IOS 0
#define         YM_OS_IOS_DEVICE 0
#define         YM_OS_IOS_SIMULATOR 0
#define YM_OS_UNIX 0

// __UNIX__

#if defined(_WIN32)
#undef  YM_OS_WINDOWS
    #define YM_OS_WINDOWS 1
    #if defined(_WIN64)
        #undef  YM_OS_WIN64
        #define YM_OS_WIN64 1
    #else
        #undef  YM_OS_WIN32
        #define YM_OS_WIN32 1
    #endif
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__linux)
    #undef  YM_OS_LINUX
    #define YM_OS_LINUX 1
    #if defined(__ANDROID__)
        #undef  YM_OS_ANDROID
        #define YM_OS_ANDROID 1
    #endif
#elif defined(__APPLE__)
    #undef  YM_OS_APPLE
    #define YM_OS_APPLE 1
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #undef  YM_OS_IOS
        #define YM_OS_IOS 1
        #undef  YM_OS_IOS_SIMULATOR
        #define YM_OS_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #undef  YM_OS_IOS
        #define YM_OS_IOS 1
        #undef  YM_OS_IOS_DEVICE
        #define YM_OS_IOS_DEVICE 1
    #elif TARGET_OS_MAC
        #undef  YM_OS_OSX
        #define YM_OS_OSX 1
    #endif
#elif defined(__UNIX__)
    #undef  YM_OS_UNIX
    #define YM_OS_UNIX 1
#else
    #error Do not know the operating system
#endif




// Have
#define YM_HAVE_NEON 0
#define YM_HAVE_SSE 0

#ifdef __ARM_NEON__
#undef  YM_HAVE_NEON
#define YM_HAVE_NEON 1
#endif

#if (defined(__x86_64__) || defined(i386) || defined(_M_IX86) || defined(_M_X64) )
#undef YM_HAVE_SSE
#define YM_HAVE_SSE 1
#endif


// Is
#define YM_IS_32_BIT 0
#define YM_IS_64_BIT 0
#define YM_IS_SMALL_ENDIAN 0
#define YM_IS_BIG_ENDIAN 0


#if defined(__ia64) || defined(__itanium__) || defined(_M_IA64) || defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__64BIT__) || defined(_LP64) || defined(__LP64__) || defined(__x86_64__) || defined(_M_X64) || defined(_WIN64)
    #undef  YM_IS_64_BIT
    #define YM_IS_64_BIT 1
#else
    #undef  YM_IS_32_BIT
    #define YM_IS_32_BIT 1
#endif


#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
    #error Do not know the endianess of this architecture
#endif
#ifdef __BIG_ENDIAN__
    #undef  YM_IS_BIG_ENDIAN
    #define YM_IS_BIG_ENDIAN 1
#else
    #undef  YM_IS_SMALL_ENDIAN
    #define YM_IS_SMALL_ENDIAN 1
#endif




#endif
