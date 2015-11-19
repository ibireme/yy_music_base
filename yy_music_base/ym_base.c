//
//  ym_base.c
//  yy_music
//
//  Created by ibireme on 14-2-20.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_base.h"
#include "ym_lock.h"

typedef struct _ym_object_header ym_object_header;

struct _ym_object_header {
    long ref_count;
    ym_spin_lock_t lock;
    void (*dealloc)(ym_object_ref);
    /* object struct */
};

#define header(object) (((ym_object_header *)object) - 1)

ym_object_ref _ym_alloc(size_t size, void (*dealloc)(ym_object_ref)) {
    ym_object_header *header = calloc(1, sizeof(ym_object_header) + size);
    if (header) {
        header->ref_count = 1;
        header->dealloc = dealloc;
        header->lock = YM_SPIN_LOCK_INIT;
        return (ym_object_ref)(header + 1);
    }
    return NULL;
}

ym_object_ref ym_retain(ym_object_ref object) {
    if (!object) return NULL;
    ym_spin_lock(&header(object)->lock);
    header(object)->ref_count++;
    ym_spin_unlock(&header(object)->lock);
    return object;
}

ym_object_ref ym_release(ym_object_ref object) {
    if (!object) return NULL;
    ym_spin_lock(&header(object)->lock);
    ym_object_header *header = header(object);
    header->ref_count--;
    if (header->ref_count > 0) {
        ym_spin_unlock(&header(object)->lock);
        return object;
    } else if (header->ref_count == 0) {
        if (header->dealloc) header->dealloc(object);
        ym_spin_unlock(&header(object)->lock);
        free(header);
        return NULL;
    } else {
        ym_spin_unlock(&header(object)->lock);
        return NULL;
    }
}

long ym_retain_count(ym_object_ref object) {
    if (!object) return 0;
    long count;
    ym_spin_lock(&header(object)->lock);
    count = header(object)->ref_count;
    ym_spin_unlock(&header(object)->lock);
    return count;
}
