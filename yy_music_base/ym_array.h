//
//  ym_array.h
//  yy_music
//
//  Created by ibireme on 14-2-14.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_array__
#define __ym_array__

#include "ym_base.h"

YM_EXTERN_C_BEGIN


typedef void (*ym_array_foreach_func)(ym_uint index, const void *value, void *context);
typedef void *(*ym_array_retain_callback)(const void *value);
typedef void (*ym_array_release_callback)(const void *value);
typedef bool (*ym_array_equal_callback)(const void *value1, const void *value2);

typedef struct _ym_array_callback {
    ym_array_retain_callback retain;
    ym_array_release_callback release;
    ym_array_equal_callback equal;
} ym_array_callback_t;

extern ym_array_callback_t ym_array_string_callback;
extern ym_array_callback_t ym_array_object_callback;


typedef struct _ym_array *ym_array_ref;

ym_array_ref ym_array_create();
ym_array_ref ym_array_create_for_string();
ym_array_ref ym_array_create_for_object();
ym_array_ref ym_array_create_with_options(ym_uint capacity, const ym_array_callback_t *callback);
ym_array_ref ym_array_create_copy(ym_array_ref array);
ym_array_ref ym_array_create_copy_range(ym_array_ref array, ym_range range);

const void *ym_array_get(ym_array_ref array, ym_uint index);
ym_uint ym_array_count(ym_array_ref array);
bool ym_array_get_range(ym_array_ref array, ym_range range, const void **values);
bool ym_array_replace_range(ym_array_ref array, ym_range range, const void **new_values, ym_uint new_count);
bool ym_array_append(ym_array_ref array, const void *value);
bool ym_array_prepend(ym_array_ref array, const void *value);
bool ym_array_set(ym_array_ref array, ym_uint index, const void *value);
bool ym_array_insert(ym_array_ref array, ym_uint index, const void *value);
bool ym_array_insert_all(ym_array_ref array, ym_uint index, ym_array_ref values);
bool ym_array_remove(ym_array_ref array, ym_uint index);
bool ym_array_exchange(ym_array_ref array, ym_uint index1, ym_uint index2);
bool ym_array_clear(ym_array_ref array);
bool ym_array_contains(ym_array_ref array, const void *value);
ym_uint ym_array_find_first_index(ym_array_ref array, ym_range range, const void *value);
ym_uint ym_array_find_last_index(ym_array_ref array, ym_range range, const void *value);
bool ym_array_sort(ym_array_ref array, ym_comparator_func cmp, void *context);
bool ym_array_sort_range(ym_array_ref array, ym_range range, ym_comparator_func cmp, void *context);
bool ym_array_foreach(ym_array_ref array, ym_array_foreach_func func, void *context);
bool ym_array_foreach_range(ym_array_ref array, ym_range range, ym_array_foreach_func func, void *context);

YM_EXTERN_C_END
#endif
