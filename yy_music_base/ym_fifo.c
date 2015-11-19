//
//  ym_buffer.c
//  yy_music
//
//  Created by ibireme on 14-4-26.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_fifo.h"
#include "ym_util.h"

#define YM_FIFO_INIT_COUNT 2048
#define YM_FIFO_FACTOR 16

struct _ym_fifo {
    void *mem;
    size_t item_total;
    size_t item_index;
    size_t item_count;
    size_t size_per_item;
};

static void _ym_fifo_dealloc(ym_fifo_ref fifo) {
    if (fifo->mem) free(fifo->mem);
}

ym_fifo_ref ym_fifo_create(size_t item_count, size_t size_per_item) {
    ym_fifo_ref fifo;
    
    fifo = ym_alloc(struct _ym_fifo, _ym_fifo_dealloc);
    if (fifo) {
        fifo->size_per_item = size_per_item;
        fifo->item_count = item_count;
        fifo->item_total = YM_MAX(item_count, YM_FIFO_INIT_COUNT);
        fifo->mem = calloc(fifo->item_total, size_per_item);
        if (fifo->mem == NULL) {
            ym_release(fifo);
            return NULL;
        }
    }
    return fifo;
}

size_t ym_fifo_item_count(ym_fifo_ref fifo) {
    return fifo->item_count;
}

size_t ym_fifo_size_per_item(ym_fifo_ref fifo) {
    return fifo->size_per_item;
}

bool ym_fifo_append(ym_fifo_ref fifo, void *data, size_t item_count) {
    if (item_count == 0) return true;
    if (data == NULL) return false;
    
    size_t free_all = fifo->item_total - fifo->item_count;
    if (free_all < item_count * YM_FIFO_FACTOR) {
        size_t new_count = fifo->item_count + item_count * YM_FIFO_FACTOR;
        void *new_mem = malloc(new_count * fifo->size_per_item);
        if (new_mem == NULL) return false;
        memcpy(new_mem,
               fifo->mem + fifo->item_index * fifo->size_per_item,
               fifo->item_count * fifo->size_per_item);
        free(fifo->mem);
        fifo->mem = new_mem;
        fifo->item_index = 0;
        fifo->item_total = new_count;
    }
    
    size_t free_end = fifo->item_total - fifo->item_count - fifo->item_index;
    if (free_end < item_count) {
        memmove(fifo->mem,
                fifo->mem + fifo->item_index * fifo->size_per_item,
                fifo->item_count * fifo->size_per_item);
        fifo->item_index = 0;
    }
    
    memcpy(fifo->mem + (fifo->item_index + fifo->item_count) * fifo->size_per_item,
           data,
           item_count * fifo->size_per_item);
    fifo->item_count += item_count;
    return true;
}

bool ym_fifo_remove(ym_fifo_ref fifo, size_t item_count) {
    if (item_count > fifo->item_count) return false;
    if (item_count == 0) return true;
    fifo->item_index += item_count;
    fifo->item_count -= item_count;
    return true;
}

void *ym_fifo_get(ym_fifo_ref fifo) {
    return fifo->mem + fifo->item_index * fifo->size_per_item;
}

void *ym_fifo_get_inner(ym_fifo_ref fifo, size_t item_index) {
    return fifo->mem + (fifo->item_index + item_index) * fifo->size_per_item;
}
