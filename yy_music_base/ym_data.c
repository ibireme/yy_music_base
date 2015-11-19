//
//  ym_data.c
//  yy_music
//
//  Created by ibireme on 14-2-25.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_data.h"
#include "ym_log.h"


void _ym_data_dealloc(ym_data_ref data) {
    if (data->mem) free(data->mem);
}

ym_data_ref ym_data_create(size_t size) {
    ym_data_ref data;
    
    if (size == 0) {
        ym_log_error("ym_data_t:%s() size cannot be zero",
                     __func__, sizeof(ym_data_ref));
        return NULL;
    }
    
    data = ym_alloc(struct _ym_data, _ym_data_dealloc);
    if (data == NULL) {
        ym_log_error("ym_data_t:%s() attempt to allocate %lu bytes failed",
                     __func__, sizeof(struct _ym_data));
        return NULL;
    }
    *(uint8_t **)&data->mem = malloc(size);
    if (data->mem == NULL) {
        ym_release(data);
        ym_log_error("ym_data_t:%s() attempt to allocate %lu bytes failed",
                     __func__, size);
        return NULL;
    }
    *((size_t *)&data->size) = size;
    data->cur = (uint8_t *)data->mem;
    return data;
}

ym_data_ref ym_data_create_from_mem(const void *mem, size_t size, bool autofree) {
    ym_data_ref data;
    
    if (mem == NULL || size == 0) {
        ym_log_error("ym_data_t:%s() invalid params",
                     __func__, size);
        return NULL;
    }
    if (autofree) {
        data = ym_alloc(ym_data_ref, _ym_data_dealloc);
    } else {
        data = ym_alloc(ym_data_ref, NULL);
    }
    if (data == NULL) {
        ym_log_error("ym_data_t:%s() attempt to allocate %lu bytes failed",
                     __func__, sizeof(ym_data_ref));
        return NULL;
    }
    
    *(uint8_t **)&data->mem = (uint8_t *)mem;
    *((size_t *)&data->size) = size;
    data->cur = (uint8_t *)data->mem;
    return data;
}
