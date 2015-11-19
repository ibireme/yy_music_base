//
//  ym_buffer.h
//  yy_music
//
//  Created by ibireme on 14-4-26.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __YM_FIFO__
#define __YM_FIFO__

#include "ym_base.h"

YM_EXTERN_C_BEGIN

/**
 fifo buffer used for [small] data.
 */
typedef struct _ym_fifo *ym_fifo_ref;


ym_fifo_ref ym_fifo_create(size_t item_count, size_t size_per_item);
size_t ym_fifo_item_count(ym_fifo_ref fifo);
size_t ym_fifo_size_per_item(ym_fifo_ref fifo);

bool ym_fifo_append(ym_fifo_ref fifo, void *data, size_t item_count);
bool ym_fifo_remove(ym_fifo_ref fifo, size_t item_count);

void *ym_fifo_get(ym_fifo_ref fifo);
void *ym_fifo_get_inner(ym_fifo_ref fifo, size_t item_index);

YM_EXTERN_C_END
#endif
