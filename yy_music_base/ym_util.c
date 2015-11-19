//
//  ym_util.c
//  yy_music
//
//  Created by  on 14-9-17.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_util.h"
#include <time.h>
#include <sys/time.h>


#if YM_OS_WINDOWS
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#endif

#if YM_OS_APPLE
#include <mach/mach.h>
#endif


bool ym_uint32_to_vlq(uint32_t num, uint8_t *vlq, int *vlq_length) {
    uint32_t buf;
    if (num > 0x0FFFFFFFUL || vlq == NULL || vlq_length == NULL) return false;
    buf = num & 0x7F;
    while ((num >>= 7)) {
        buf <<= 8;
        buf |= ((num & 0x7F) | 0x80);
    }
    *vlq_length = 1;
    while (true) {
        *vlq = buf & 0xFF;
        if (buf & 0x80) buf >>= 8;
        else break;
        vlq++;
        (*vlq_length)++;
    }
    return true;
}

bool ym_uint64_to_vlq(uint64_t num, uint8_t *vlq, int *vlq_length) {
    uint64_t buf;
    if (num > 0x00FFFFFFFFFFFFFFULL || vlq == NULL || vlq_length == NULL) return false;
    buf = num & 0x7F;
    while ((num >>= 7)) {
        buf <<= 8;
        buf |= ((num & 0x7F) | 0x80);
    }
    *vlq_length = 1;
    while (true) {
        *vlq = buf & 0xFF;
        if (buf & 0x80) buf >>= 8;
        else break;
        vlq++;
        (*vlq_length)++;
    }
    return true;
}

bool ym_vlq_to_uint32(uint8_t *vlq, uint32_t *num, int *vlq_length) {
    int i;
    if (vlq == NULL || num == NULL || vlq_length == NULL) return false;
    *num = 0;
    for (i = 0; i < 4; i++) {
        *num += vlq[i] & 0x7F;
        if ((vlq[i] & 0x80) == 0) break;
        if (i == 3) return false;
        *num <<= 7;
    }
    *vlq_length = i + 1;
    return true;
}

bool ym_vlq_to_uint64(uint8_t *vlq, uint64_t *num, int *vlq_length) {
    int i;
    if (vlq == NULL || num == NULL || vlq_length == NULL) return false;
    *num = 0;
    for (i = 0; i < 8; i++) {
        *num += vlq[i] & 0x7F;
        if ((vlq[i] & 0x80) == 0) break;
        if (i == 7) return false;
        *num <<= 7;
    }
    *vlq_length = i + 1;
    return true;
}

char *ym_current_time_str() {
    time_t curtime = time(NULL);
    struct tm *loctime = localtime(&curtime);
    char formatted[20];
    strftime(formatted, 20, "%Y-%m-%d %H:%M:%S", loctime);
    return strdup(formatted);
}

double ym_current_timestamp() {
#if YM_OS_WINDOWS
    FILETIME file_time;
    SYSTEMTIME system_time;
    ULARGE_INTEGER ularge;
    
    // FILETIME of Jan 1 1970 00:00:00.
    static const uint64_t _ym_epoch = (uint64_t)116444736000000000ULL;
    
    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;
    return (double)((ularge.QuadPart - _ym_epoch) / 10000000) + (double)(system_time.wMilliseconds) / 1000.0;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    return (double)now.tv_sec + (double)now.tv_usec / 1000000.0;
#endif
}

int64_t ym_current_timestamp_ms() {
#if YM_OS_WINDOWS
    FILETIME file_time;
    SYSTEMTIME system_time;
    ULARGE_INTEGER ularge;
    
    // FILETIME of Jan 1 1970 00:00:00.
    static const uint64_t _ym_epoch = (uint64_t)116444736000000000ULL;
    
    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;
    return (ularge.QuadPart - _ym_epoch) / 10000 + system_time.wMilliseconds;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
}

int64_t ym_clock_ms() {
#if YM_OS_WINDOWS
    // System frequency does not change at run-time, cache it
    static int64_t frequency = 0;
    if (frequency == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        // Windows documentation says that XP and later will always return non-zero
        assert(freq.QuadPart != 0);
        frequency = freq.QuadPart;
    }
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return (int64_t)(count.QuadPart * 1000) / frequency;
    
#elif YM_OS_APPLE
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    return (int64_t)((int64_t)mts.tv_sec * 1000 + (int64_t)mts.tv_nsec / 1000000);
    
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)((int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000);
    
#endif
}

int64_t ym_clock_us() {
#if YM_OS_WINDOWS
    //  System frequency does not change at run-time, cache it
    static int64_t frequency = 0;
    if (frequency == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        // Windows documentation says that XP and later will always return non-zero
        assert(freq.QuadPart != 0);
        frequency = freq.QuadPart;
    }
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return (int64_t)(count.QuadPart * 1000000) / frequency;
    
#elif YM_OS_APPLE
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    return (int64_t)((int64_t)mts.tv_sec * 1000000 + (int64_t)mts.tv_nsec / 1000);
    
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)((int64_t)ts.tv_sec * 1000000 + (int64_t)ts.tv_nsec / 1000);
    
#endif
}

void ym_sleep_ms(uint32_t msecs) {
#if YM_OS_WINDOWS
    if (msecs > 0) Sleep(msecs);
#else
    struct timespec t;
    t.tv_sec = msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep(&t, NULL);
#endif
}

ym_path_info ym_util_get_path_info(const char *path) {
    ym_path_info info = { 0 };
    
    if (access(path, 0) != 0) return info;
    info.is_exist = true;
    if (access(path, R_OK) == 0) info.is_readable = true;
    if (access(path, W_OK) == 0) info.is_writable = true;
    if (access(path, X_OK) == 0) info.is_executable = true;
    
    struct stat attr;
    if (stat(path, &attr) != 0) return info;
    info.size = attr.st_size;
    mode_t mode = attr.st_mode;
    if (S_ISDIR(mode)) info.is_directory = true;
    else if (S_ISREG(mode)) info.is_file = true;
    
#if YM_OS_APPLE
    struct timespec time = attr.st_ctimespec;
    info.time = (double)time.tv_sec + (double)time.tv_nsec / 1000000000.0;
#else
    info.time = info.st_ctime;
#endif
    
    return info;
}

bool ym_util_create_file(const char *path) {
    if (access(path, 0) == 0) return false;
    FILE *fd = fopen(path, "a");
    if (fd) {
        fclose(fd);
        return true;
    } else {
        return false;
    }
}

bool ym_util_delete_file(const char *path) {
#if YM_OS_WINDOWS
    return DeleteFileA(path) != 0;
#else
    return unlink(path) == 0;
#endif
}

bool ym_util_create_dir(const char *path) {
#if YM_OS_WINDOWS
    return CreateDirectoryA(path, NULL) != 0;
#else
    return mkdir(path, 0775) == 0;
#endif
}

bool ym_util_delete_dir(const char *path) {
#if YM_OS_WINDOWS
    return RemoveDirectoryA(path) != 0;
#else
    return rmdir(path) == 0;
#endif
    return false;
}

long ym_cpu_available_core_count() {
    long count = 1;
#if defined(YM_WINDOWS)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    count = si.dwNumberOfProcessors;
#else
    count = sysconf(_SC_NPROCESSORS_ONLN);
    // get_nprocs();   //GNU fuction
#endif
    return count;
}
