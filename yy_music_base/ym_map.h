//
//  ym_map.h
//  yy_music
//
//  Created by ibireme on 14-2-20.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_map__
#define __ym_map__

#include "ym_base.h"
#include "ym_array.h"

YM_EXTERN_C_BEGIN

typedef void (*ym_map_foreach_func)(const void *key, const void *value, void *context);

typedef void *(*ym_map_retain_callback)(const void *value);
typedef void (*ym_map_release_callback)(const void *value);
typedef bool (*ym_map_equal_callback)(const void *value1, const void *value2);
typedef ym_uint (*ym_map_hash_callback)(const void *value);

typedef struct _ym_map_key_callback {
    ym_map_retain_callback retain;
    ym_map_release_callback release;
    ym_map_equal_callback equal;
    ym_map_hash_callback hash;
} ym_map_key_callback_t;

typedef struct _ym_map_value_callback {
    ym_map_retain_callback retain;
    ym_map_release_callback release;
    ym_map_equal_callback equal;
} ym_map_value_callback_t;

extern ym_map_key_callback_t ym_map_string_key_callback;
extern ym_map_key_callback_t ym_map_object_key_callback;
extern ym_map_value_callback_t ym_map_string_value_callback;
extern ym_map_value_callback_t ym_map_object_value_callback;



typedef struct _ym_map *ym_map_ref;

ym_map_ref ym_map_create();
ym_map_ref ym_map_create_with_options(ym_uint capacity,
                                     const ym_map_key_callback_t *key_callback,
                                     const ym_map_value_callback_t *value_callback);


ym_uint ym_map_count(ym_map_ref map);
bool ym_map_contains_key(ym_map_ref map, const void *key);
bool ym_map_contains_value(ym_map_ref map,const void *value);
const void *ym_map_get(ym_map_ref map, const void *key);
bool ym_map_set(ym_map_ref map, const void *key, const void *value);
bool ym_map_set_all(ym_map_ref map, ym_map_ref add);
bool ym_map_remove(ym_map_ref map, const void *key);
bool ym_map_clear(ym_map_ref map);
bool ym_map_get_all_keys(ym_map_ref map, const void **keys);
bool ym_map_foreach(ym_map_ref map, ym_map_foreach_func func, void *context);
ym_array_ref ym_map_create_key_array(ym_map_ref map);

YM_EXTERN_C_END
#endif
