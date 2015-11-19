//
//  ym_data.h
//  yy_music
//
//  Created by ibireme on 14-2-25.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __YM_DATA__
#define __YM_DATA__

#include "ym_base.h"
#include "ym_util.h"

YM_EXTERN_C_BEGIN

typedef struct _ym_data *ym_data_ref;

struct _ym_data {
    const size_t size;
    uint8_t *const mem;
    uint8_t *cur;
};

ym_data_ref ym_data_create(size_t size);
ym_data_ref ym_data_create_from_mem(const void *mem, size_t size, bool autofree);



ym_inline bool ym_data_read(ym_data_ref data, size_t size, void *dest) {
    if (data == NULL) return false;
    if (data->cur + size > data->mem + data->size) return false;
    memcpy(dest, data->cur, size);
    data->cur += size;
    return true;
}

ym_inline bool ym_data_read_uint8(ym_data_ref data, uint8_t *value) {
    if (data == NULL) return false;
    if (data->cur + 1 > data->mem + data->size) return false;
    memcpy(value, data->cur, 1);
    data->cur += 1;
    return true;
}

ym_inline bool ym_data_read_uint16(ym_data_ref data, uint16_t *value) {
    if (data == NULL) return false;
    if (data->cur + 2 > data->mem + data->size) return false;
    memcpy(value, data->cur, 2);
    data->cur += 2;
    return true;
}

ym_inline bool ym_data_read_uint16_swap_endian(ym_data_ref data, uint16_t *value) {
    if (data == NULL) return false;
    if (data->cur + 2 > data->mem + data->size) return false;
    memcpy(value, data->cur, 2);
    *value = ym_swap_endian_uint16(*value);
    data->cur += 2;
    return true;
}

ym_inline bool ym_data_read_uint32(ym_data_ref data, uint32_t *value) {
    if (data == NULL) return false;
    if (data->cur + 4 > data->mem + data->size) return false;
    memcpy(value, data->cur, 4);
    data->cur += 4;
    return true;
}

ym_inline bool ym_data_read_uint32_swap_endian(ym_data_ref data, uint32_t *value) {
    if (data == NULL) return false;
    if (data->cur + 4 > data->mem + data->size) return false;
    memcpy(value, data->cur, 4);
    *value = ym_swap_endian_uint32(*value);
    data->cur += 4;
    return true;
}

ym_inline bool ym_data_read_uint64_swap_endian(ym_data_ref data, uint64_t *value) {
    if (data == NULL) return false;
    if (data->cur + 8 > data->mem + data->size) return false;
    memcpy(value, data->cur, 8);
    *value = ym_swap_endian_uint64(*value);
    data->cur += 8;
    return true;
}

/**
 * Variable-length quantity
 */
ym_inline bool ym_data_read_vlq(ym_data_ref data, uint64_t *vlq) {
    int i;
    uint8_t buff;
    uint64_t value = 0;
    
    if (data == NULL) return false;
    for (i = 0; i < 4; i++) {
        buff = *data->cur;
        if (!(buff & 0x80)) {
            *vlq = value | buff;
            data->cur++;
            return data->cur <= data->mem + data->size;
        }
        value = (value | (buff & 0x7F)) << 7;
        data->cur++;
    }
    return false;
}

YM_EXTERN_C_END
#endif
