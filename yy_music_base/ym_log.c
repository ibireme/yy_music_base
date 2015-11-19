//
//  ym_log.c
//  yy_music
//
//  Created by ibireme on 14-2-15.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_log.h"


static void ym_log_func(ym_log_level level, char *fmt, va_list args) {
    switch (level) {
        case YM_LOG_LEVEL_INFO: printf("[YM_INFO] "); break;
        case YM_LOG_LEVEL_WARN: printf("[YM_WARN] "); break;
        case YM_LOG_LEVEL_ERROR: printf("[YM_ERROR] "); break;
        default:  break;
    }
    vprintf(fmt, args);
    printf("\n");
}

#define YM_DO_LOG(level) do { \
va_list args; \
va_start(args, fmt); \
ym_log_func((level), fmt, args); \
va_end(args); \
} while (0)

void ym_log(ym_log_level level, char *fmt, ...) {
    YM_DO_LOG(level);
}

void ym_log_info(char *fmt, ...) {
    YM_DO_LOG(YM_LOG_LEVEL_INFO);
}

void ym_log_warn(char *fmt, ...) {
    YM_DO_LOG(YM_LOG_LEVEL_WARN);
}

void ym_log_error(char *fmt, ...) {
    YM_DO_LOG(YM_LOG_LEVEL_ERROR);
}
