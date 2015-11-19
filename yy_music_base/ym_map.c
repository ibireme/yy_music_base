//
//  ym_map.c
//  yy_music
//
//  Created by ibireme on 14-2-20.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_map.h"
#include "ym_log.h"

#pragma mark - Const Values

static const float _ym_map_resize_factor = 0.75;

static const uint64_t _ym_map_size_prime[] = {
    7ULL, // 1<<3 Minimum Capacity
    13ULL, // 1<<4
    31ULL, // 1<<5
    61ULL, // 1<<6
    127ULL, // 1<<7
    251ULL, // 1<<8
    509ULL, // 1<<9
    1021ULL, // 1<<10
    2039ULL, // 1<<11
    4093ULL, // 1<<12
    8191ULL, // 1<<13
    16381ULL, // 1<<14
    32749ULL, // 1<<15
    65521ULL, // 1<<16
    131071ULL, // 1<<17
    262139ULL, // 1<<18
    524287ULL, // 1<<19
    1048573ULL, // 1<<20
    2097143ULL, // 1<<21
    4194301ULL, // 1<<22
    8388593ULL, // 1<<23
    16777213ULL, // 1<<24
    33554393ULL, // 1<<25
    67108859ULL, // 1<<26
    134217689ULL, // 1<<27
    268435399ULL, // 1<<28
    536870909ULL, // 1<<29
    1073741789ULL, // 1<<30
    2147483647ULL, // 1<<31
    4294967291ULL, // 1<<32
    8589934583ULL, // 1<<33
    17179869143ULL, // 1<<34
    34359738337ULL, // 1<<35
    68719476731ULL, // 1<<36
    137438953447ULL, // 1<<37
    274877906899ULL, // 1<<38
    549755813881ULL, // 1<<39
    1099511627689ULL, // 1<<40
    2199023255531ULL, // 1<<41
    4398046511093ULL, // 1<<42
    8796093022151ULL, // 1<<43
    17592186044399ULL, // 1<<44
    35184372088777ULL, // 1<<45
    70368744177643ULL, // 1<<46
    140737488355213ULL, // 1<<47
    281474976710597ULL, // 1<<48
    562949953421231ULL, // 1<<49
    1125899906842597ULL, // 1<<50
    2251799813685119ULL, // 1<<51
    4503599627370449ULL, // 1<<52
    9007199254740881ULL, // 1<<53
    18014398509481951ULL, // 1<<54
    36028797018963913ULL, // 1<<55
    72057594037927931ULL, // 1<<56
    144115188075855859ULL, // 1<<57
    288230376151711717ULL, // 1<<58
    576460752303423433ULL, // 1<<59
    1152921504606846883ULL, // 1<<60
    2305843009213693951ULL, // 1<<61
    4611686018427387847ULL, // 1<<62
    9223372036854775783ULL, // 1<<63
};



#pragma mark - Default Callback

/**
 * Pointer Hash Function.
 * This implementation is the Robert Jenkins' 32 bit Mix Function,
 * with a simple adaptation for 64-bit values.
 */
static ym_uint _ym_map_hash_callback_default(const void *key) {
    ym_uint k = (ym_uint)key;
    k += (k << 12);
    k ^= (k >> 22);
    k += (k << 4);
    k ^= (k >> 9);
    k += (k << 10);
    k ^= (k >> 2);
    k += (k << 7);
    k ^= (k >> 12);
#if YM_IS_64_BIT
    k += (k << 44);
    k ^= (k >> 54);
    k += (k << 36);
    k ^= (k >> 41);
    k += (k << 42);
    k ^= (k >> 34);
    k += (k << 39);
    k ^= (k >> 44);
#endif
    return k;
}

/**
 * BKDR Hash Function.
 * Using a strange set of possible seeds: 31 131 1313 13131 etc...
 */
static ym_uint _ym_map_string_hash_callback(const void *key) {
    ym_uint hash = 0;
    char *str = (char *)key;
    while (*str) hash = hash * 131 + (*str++);
    return hash;
}

static bool _ym_map_string_equal_callback(const void *key1, const void *key2) {
    return strcmp(key1, key2) == 0;
}

ym_map_key_callback_t ym_map_string_key_callback = {
    (ym_map_retain_callback)strdup,
    (ym_map_release_callback)free,
    _ym_map_string_equal_callback,
    _ym_map_string_hash_callback,
};

ym_map_key_callback_t ym_map_object_key_callback = {
    (ym_map_retain_callback)ym_retain,
    (ym_map_release_callback)ym_release,
    NULL,
    _ym_map_hash_callback_default,
};

ym_map_value_callback_t ym_map_string_value_callback = {
    (ym_map_retain_callback)strdup,
    (ym_map_release_callback)free,
    _ym_map_string_equal_callback,
};

ym_map_value_callback_t ym_map_object_value_callback = {
    (ym_map_retain_callback)ym_retain,
    (ym_map_release_callback)ym_release,
    NULL,
};



#pragma mark - Struct Define

typedef struct _ym_map_node ym_map_node_t;

struct _ym_map_node {
    const void *key;
    const void *value;
    ym_uint hash;
    ym_map_node_t *next;
};

struct _ym_map {
    ym_uint node_count;
    ym_uint node_count_threshold;
    ym_uint bucket_count;
    ym_map_node_t **buckets;
    ym_map_key_callback_t key_callback;
    ym_map_value_callback_t value_callback;
};



#pragma mark - Private Func

ym_inline ym_map_node_t *_ym_map_get_node(ym_map_ref map, ym_map_node_t **bucket, const void *key) {
    ym_map_node_t *node = *bucket;
    while (node) {
        if (node->key == key
            || (map->key_callback.equal &&  map->key_callback.equal(node->key, key))) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

static bool _ym_map_resize(ym_map_ref map, ym_uint new_bucket_count) {
    ym_map_node_t **new_buckets = calloc(new_bucket_count, sizeof(ym_map_node_t *));
    if (new_buckets == NULL) {
        ym_log_error("ym_array_t:%s() attempt to allocate %lu bytes failed",
                     __func__, new_bucket_count * sizeof(ym_map_node_t *));
        return false;
    }
    
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_uint new_bucket_index;
        ym_map_node_t *node, *next_node, *new_node;
        node = map->buckets[i];
        while (node) {
            next_node = node->next;
            node->next = NULL;
            new_bucket_index = node->hash % new_bucket_count;
            new_node = new_buckets[new_bucket_index];
            if (!new_node) {
                new_buckets[new_bucket_index] = node;
            } else {
                while (new_node->next) new_node = new_node->next;
                new_node->next = node;
            }
            node = next_node;
        }
    }
    free(map->buckets);
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;
    return true;
}

static void _ym_map_dealloc(ym_map_ref map) {
    if (map->buckets == NULL) return;
    ym_map_clear(map);
    free(map->buckets);
}

#pragma mark - Public Func

ym_map_ref ym_map_create() {
    return ym_map_create_with_options(0, NULL, NULL);
}

ym_map_ref ym_map_create_with_options(ym_uint                        capacity,
                                      const ym_map_key_callback_t *  key_callback,
                                      const ym_map_value_callback_t *value_callback) {
    int prime_index = 0;
    while (_ym_map_size_prime[prime_index] < (uint64_t)capacity) {
        prime_index++;
    }
    capacity = _ym_map_size_prime[prime_index];
    
    ym_map_ref map = ym_alloc(struct _ym_map, _ym_map_dealloc);
    if (map == NULL) {
        ym_log_error("ym_map_t:%s() attempt to allocate %lu bytes failed",
                     __func__, sizeof(struct _ym_map));
        return NULL;
    }
    
    map->buckets = calloc(capacity, sizeof(ym_map_node_t *));
    if (map->buckets == NULL) {
        ym_release(map);
        ym_log_error("ym_map_t:%s() attempt to allocate %lu bytes failed",
                     __func__, capacity * sizeof(ym_map_node_t *));
        return NULL;
    }
    
    map->node_count = 0;
    map->node_count_threshold = capacity * _ym_map_resize_factor;
    map->bucket_count = capacity;
    if (key_callback) map->key_callback = *key_callback;
    if (value_callback) map->value_callback = *value_callback;
    if (map->key_callback.hash == NULL) {
        map->key_callback.hash = _ym_map_hash_callback_default;
    }
    return map;
}

ym_uint ym_map_count(ym_map_ref map) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return 0;
    }
    return map->node_count;
}

bool ym_map_contains_key(ym_map_ref map, const void *key) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    ym_uint bucket_index = map->key_callback.hash(key) % map->bucket_count;
    ym_map_node_t **bucket = &(map->buckets[bucket_index]);
    ym_map_node_t *node = _ym_map_get_node(map, bucket, key);
    return node != NULL;
}

bool ym_map_contains_value(ym_map_ref map, const void *value) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_map_node_t **bucket = &map->buckets[i];
        ym_map_node_t *node = *bucket;
        while (node) {
            if (node->value == value
                || (map->value_callback.equal && map->value_callback.equal(node->value, value))) {
                return true;
            }
            node = node->next;
        }
    }
    return false;
}

const void *ym_map_get(ym_map_ref map, const void *key) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return NULL;
    }
    ym_uint bucket_index = map->key_callback.hash(key) % map->bucket_count;
    ym_map_node_t **bucket = &(map->buckets[bucket_index]);
    ym_map_node_t *node = _ym_map_get_node(map, bucket, key);
    if (node) return node->value;
    return NULL;
}

bool ym_map_set(ym_map_ref map, const void *key, const void *value) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    
    ym_uint hash = map->key_callback.hash(key);
    ym_uint bucket_index = hash % map->bucket_count;
    ym_map_node_t **bucket = &(map->buckets[bucket_index]);
    ym_map_node_t *node = _ym_map_get_node(map, bucket, key);
    
    if (node) {
        if (map->value_callback.retain) value = map->value_callback.retain(value);
        if (map->value_callback.release) map->value_callback.release(node->value);
        node->value = value;
        return true;
    }
    
    node = calloc(1, sizeof(ym_map_node_t));
    if (node == NULL) {
        ym_log_error("ym_map_t:%s() attempt to allocate %lu bytes failed",
                     __func__, sizeof(ym_map_node_t));
        return false;
    }
    
    if (*bucket) {
        ym_map_node_t *cur_node = *bucket;
        while (cur_node->next) cur_node = cur_node->next;
        cur_node->next = node;
    } else {
        *bucket = node;
    }
    
    if (map->key_callback.retain) node->key = map->key_callback.retain(key);
    else node->key = key;
    if (map->value_callback.retain) value = map->value_callback.retain(value);
    node->value = value;
    node->hash = hash;
    map->node_count++;
    
    if (map->node_count > map->node_count_threshold) {
        int prime_index = 0;
        while (_ym_map_size_prime[prime_index] < (uint64_t)map->node_count) {
            prime_index++;
        }
        ym_uint capacity = _ym_map_size_prime[prime_index + 1];
        if (_ym_map_resize(map, capacity)) {
            map->node_count_threshold = capacity * _ym_map_resize_factor;
        }
    }
    return true;
}

bool ym_map_set_all(ym_map_ref map, ym_map_ref add) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    if (add == NULL) return false;
    
    for (ym_uint i = 0; i < add->bucket_count; i++) {
        ym_map_node_t **bucket = &add->buckets[i];
        if (!*bucket) continue;
        ym_map_node_t *node = *bucket;
        while (node) {
            ym_map_set(map, node->key, node->value);
            node = node->next;
        }
    }
    return true;
}

bool ym_map_remove(ym_map_ref map, const void *key) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    ym_uint bucket_index = map->key_callback.hash(key) % map->bucket_count;
    ym_map_node_t **bucket = &map->buckets[bucket_index];
    
    ym_map_node_t *node = *bucket;
    ym_map_node_t *prev_node = NULL;
    while (node) {
        if (node->key == key
            || (map->key_callback.equal &&  map->key_callback.equal(node->key, key))) {
            break;
        }
        prev_node = node;
        node = node->next;
    }
    if (node == NULL) return false;
    
    if (map->key_callback.release) map->key_callback.release(node->key);
    if (map->value_callback.release) map->value_callback.release(node->value);
    if (prev_node == NULL) *bucket = node->next;
    else prev_node->next = node->next;
    free(node);
    map->node_count--;
    return true;
}

bool ym_map_clear(ym_map_ref map) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_map_node_t **bucket = &map->buckets[i];
        ym_map_node_t *node = *bucket;
        while (node) {
            if (map->key_callback.release) map->key_callback.release(node->key);
            if (map->value_callback.release) map->value_callback.release(node->value);
            ym_map_node_t *next_node = node->next;
            free(node);
            node = next_node;
        }
        *bucket = NULL;
    }
    map->node_count = 0;
    return true;
}

bool ym_map_get_all_keys(ym_map_ref map, const void **keys) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    if (keys == NULL) return false;
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_map_node_t **bucket = &map->buckets[i];
        ym_map_node_t *node = *bucket;
        while (node) {
            *keys = node->key;
            keys++;
            node = node->next;
        }
    }
    return true;
}

bool ym_map_foreach(ym_map_ref map, ym_map_foreach_func func, void *context) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return false;
    }
    if (func == NULL) return false;
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_map_node_t **bucket = &map->buckets[i];
        ym_map_node_t *node = *bucket;
        while (node) {
            func(node->key, node->value, context);
            node = node->next;
        }
    }
    return true;
}

ym_array_ref ym_map_create_key_array(ym_map_ref map) {
    if (map == NULL) {
        ym_log_error("%s() input map cannot be null", __func__);
        return NULL;
    }
    
    ym_array_callback_t callback;
    callback.retain = map->key_callback.retain;
    callback.release = map->key_callback.release;
    callback.equal = map->key_callback.equal;
    ym_array_ref array = ym_array_create_with_options(map->bucket_count, &callback);
    if (array == NULL) return NULL;
    
    for (ym_uint i = 0; i < map->bucket_count; i++) {
        ym_map_node_t **bucket = &map->buckets[i];
        ym_map_node_t *node = *bucket;
        while (node) {
            ym_array_append(array, node->key);
            node = node->next;
        }
    }
    return array;
}
