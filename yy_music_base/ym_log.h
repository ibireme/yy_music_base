//
//  ym_log.h
//  yy_music
//
//  Created by ibireme on 14-2-15.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_log__
#define __ym_log__

#include "ym_base.h"

YM_EXTERN_C_BEGIN

typedef enum {
    YM_LOG_LEVEL_INFO = 0,
    YM_LOG_LEVEL_WARN = 1,
    YM_LOG_LEVEL_ERROR = 2,
} ym_log_level;


void ym_log(ym_log_level level, char *fmt, ...);
void ym_log_info(char *fmt, ...);
void ym_log_warn(char *fmt, ...);
void ym_log_error(char *fmt, ...);

YM_EXTERN_C_END
#endif
