//
//  ym_file.h
//  yy_music
//
//  Created by ibireme on 15/1/16.
//  Copyright (c) 2015 ibireme. All rights reserved.
//

#ifndef __ym_file__
#define __ym_file__

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "ym_base.h"

#undef YM_FILE_DIR_SEPARATOR
#if YM_OS_WINDOWS
#define YM_FILE_DIR_SEPARATOR '\\'
#else 
#define YM_FILE_DIR_SEPARATOR '/'
#endif

typedef enum {
    YM_FILE_READ  = 1 << 0,
    YM_FILE_WRITE = 1 << 1,
} ym_file_mode;

typedef struct _ym_file *ym_file_ref;

/**
 | file || exist    | not exist |
 |------||----------|-----------|
 | read ||open->head|  fail     |
 | write||open->tail|  create   |
 | r/w  ||open->head|  create   |
 */
ym_file_ref ym_file_create(const char *path, ym_file_mode mode, ym_error_code *error);

ym_file_mode ym_file_get_mode(ym_file_ref file);
const char *ym_file_get_path(ym_file_ref file);
const char *ym_file_get_name(ym_file_ref file);
const char *ym_file_get_dir(ym_file_ref file);
int64_t ym_file_get_size(ym_file_ref file); // -1 when error
ym_endian ym_file_get_endian(ym_file_ref file);
void ym_file_set_endian(ym_file_ref file, ym_endian endian);

int64_t ym_file_tell(ym_file_ref file); // -1 when error
bool ym_file_seek_set(ym_file_ref file, int64_t ofs);
bool ym_file_seek_end(ym_file_ref file, int64_t ofs);
bool ym_file_seek_cur(ym_file_ref file, int64_t ofs);
bool ym_file_is_eof(ym_file_ref file);
void ym_file_clear_eof(ym_file_ref file);
void ym_file_flush(ym_file_ref file);

uint32_t ym_file_write(ym_file_ref file, const void *buf, uint32_t size);
uint32_t ym_file_read(ym_file_ref file, void *buf, uint32_t size);

bool ym_file_read_uint8(ym_file_ref file, uint8_t *buf);
bool ym_file_read_uint16(ym_file_ref file, uint16_t *buf);
bool ym_file_read_uint32(ym_file_ref file, uint32_t *buf);
bool ym_file_read_uint64(ym_file_ref file, uint64_t *buf);
bool ym_file_read_int8(ym_file_ref file, int8_t *buf);
bool ym_file_read_int16(ym_file_ref file, int16_t *buf);
bool ym_file_read_int32(ym_file_ref file, int32_t *buf);
bool ym_file_read_int64(ym_file_ref file, int64_t *buf);
bool ym_file_read_float(ym_file_ref file, float *buf);
bool ym_file_read_double(ym_file_ref file, double *buf);
bool ym_file_read_fourcc(ym_file_ref file, uint32_t *buf);
bool ym_file_read_vlq32(ym_file_ref file, uint32_t *buf);
bool ym_file_read_vlq64(ym_file_ref file, uint64_t *buf);

bool ym_file_write_uint8(ym_file_ref file, uint8_t val);
bool ym_file_write_uint16(ym_file_ref file, uint16_t val);
bool ym_file_write_uint32(ym_file_ref file, uint32_t val);
bool ym_file_write_uint64(ym_file_ref file, uint64_t val);
bool ym_file_write_int8(ym_file_ref file, int8_t val);
bool ym_file_write_int16(ym_file_ref file, int16_t val);
bool ym_file_write_int32(ym_file_ref file, int32_t val);
bool ym_file_write_int64(ym_file_ref file, int64_t val);
bool ym_file_write_float(ym_file_ref file, float val);
bool ym_file_write_double(ym_file_ref file, double val);
bool ym_file_write_fourcc(ym_file_ref file, uint32_t val);
bool ym_file_write_vlq32(ym_file_ref file, uint32_t val);
bool ym_file_write_vlq64(ym_file_ref file, uint64_t val);


#endif