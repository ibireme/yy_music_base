//
//  ym_array.c
//  yy_music
//
//  Created by ibireme on 14-2-14.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_array.h"
#include "ym_log.h"

#pragma mark - Quick Sort Func

#define _ym_sort_swap(x, y) { tmp = (x); (x) = (y); (y) = tmp; }

ym_inline long _ym_quicksort_partition(const void **dst, const long left, const long right, const long pivot, ym_comparator_func cmpf, void *context) {
    const void *tmp;
    const void *value = dst[pivot];
    long index = left;
    long all_same = 1;
    
    /* move the pivot to the right */
    _ym_sort_swap(dst[pivot], dst[right]);
    
    for (long i = left; i < right; i++) {
        ym_order cmp = cmpf(dst[i], value, context);
        /* check if everything is all the same */
        if (cmp != YM_ORDER_EQUAL) {
            all_same &= 0;
        }
        
        if (cmp == YM_ORDER_ASC) {
            _ym_sort_swap(dst[i], dst[index]);
            index++;
        }
    }
    _ym_sort_swap(dst[right], dst[index]);
    
    /* avoid degenerate case */
    if (all_same) {
        return -1;
    }
    
    return index;
}

static void _ym_quicksort_inner(const void **dst, const long left, const long right, ym_comparator_func cmp, void *context) {
    if (right <= left) {
        return;
    }
    
    long pivot = left + ((right - left) >> 1);
    long new_pivot = _ym_quicksort_partition(dst, left, right, pivot, cmp, context);
    
    /* check for partition all equal */
    if (new_pivot < 0) {
        return;
    }
    _ym_quicksort_inner(dst, left, new_pivot - 1, cmp, context);
    _ym_quicksort_inner(dst, new_pivot + 1, right, cmp, context);
}

static void _ym_quick_sort(const void **values, const size_t size, ym_comparator_func cmp, void *context) {
    if (size <= 1 || values == NULL || cmp == NULL) return;
    _ym_quicksort_inner(values, 0, size - 1, cmp, context);
}

#pragma mark - Default Callback

static bool _ym_array_string_equal_callback(const void *key1, const void *key2) {
    return strcmp(key1, key2) == 0;
}

ym_array_callback_t ym_array_string_callback = {
    (ym_array_retain_callback)strdup,
    (ym_array_release_callback)free,
    _ym_array_string_equal_callback,
};

ym_array_callback_t ym_array_object_callback = {
    (ym_array_retain_callback)ym_retain,
    (ym_array_release_callback)ym_release,
    NULL,
};



#pragma mark - Struct Define

struct _ym_array {
    ym_uint count;
    ym_uint capacity;
    ym_uint index;
    const void **ring;
    ym_array_callback_t callback;
};



#pragma mark - Private Func

/**
 * Validate range and log error.
 *
 * @param range  the range of array
 */
ym_inline bool _ym_array_validate_range(ym_array_ref array, ym_range range, const char *func) {
    if (range.location > array->count) {
        ym_log_error("ym_array_t(%p):%s() range.location(%lu) out of bounds(0,%lu)",
                     array, func, range.location, array->count);
        return false;
    }
    if (range.location + range.length > array->count) {
        ym_log_error("ym_array_t(%p):%s() range(%lu,%lu) ending index out of bounds(0,%lu)",
                     array, func, range.location, range.length, array->count);
        return false;
    }
    return true;
}

/**
 * Validate index and log error.
 *
 * @param index the index of array
 * @param edge  the index can be edge
 */
ym_inline bool _ym_array_validate_index(ym_array_ref array, ym_uint index, bool edge, const char *func) {
    if (edge) {
        if (index > array->count) {
            ym_log_error("ym_array_t(%p):%s() index(%lu) out of bounds(0,%lu)",
                         array, func, index, array->count);
            return false;
        }
    } else {
        if (array->count == 0) {
            ym_log_error("ym_array_t(%p):%s() is empty",
                         array, func, index, array->count - 1);
            return false;
        }
        if (index >= array->count) {
            ym_log_error("ym_array_t(%p):%s() index(%lu) out of bounds(0,%lu)",
                         array, func, index, array->count - 1);
            return false;
        }
    }
    return true;
}

/**
 * Expand capacity to fit 2^x.
 */
ym_inline ym_uint _ym_array_capacity_expand(ym_uint capacity) {
    int i = 1;
    if (capacity < 16) return 16;
    else if (capacity >= (LONG_MAX >> 1)) return LONG_MAX;
    while ((capacity >>= 1)) i++;
    return 1L << i;
}

/**
 * Split ring's range to absolute offset.
 *
 * @param range the range of array
 * @param abs1  absolute offset 1
 * @param abs2  absolute offset 2
 */
ym_inline void _ym_array_split(ym_array_ref array, ym_range range, ym_range *abs1, ym_range *abs2) {
    abs1->location = array->index + range.location;
    abs1->length = range.length;
    while (abs1->location >= array->capacity) abs1->location -= array->capacity;
    
    abs2->location = abs1->location + range.length;
    if (abs2->location >= array->capacity) {
        abs2->location = 0;
        abs2->length = abs1->location + abs1->length - array->capacity;
        abs1->length -= abs2->length;
    } else {
        abs2->length = 0;
    }
}

/**
 * Move the specified range of memory to left or right.
 *
 * @param range absolute offset range
 * @param move  negative value means move left,
 *              positive value means move right.
 */
ym_inline void _ym_array_move_range(ym_array_ref array, ym_range range, long move) {
    /*
	    move to the left    |    move to the right
     -----------------------+-----------------------------------
     1. [       ABC    ]    |    [    ABC       ]        before
	    [    ABC       ]    |    [       ABC    ]        after
     -----------------------+-----------------------------------
     2. [ ABC          ]    |    [          ABC ]        before
	    [BC           A]    |    [C           AB]        after
     -----------------------+-----------------------------------
     3. [BC           A]    |    [C           AB]        before
	    [          ABC ]    |    [ ABC          ]        after
     -----------------------+-----------------------------------
     4. [BC           A]    |    [C           AB]        before
	    [C           AB]    |    [BC           A]        after
     -----------------------------------------------------------
     */
    ym_range src1, src2, dest1, dest2;
    
    _ym_array_split(array, range, &src1, &src2);
    range.location += move + array->capacity;
    _ym_array_split(array, range, &dest1, &dest2);
    
    if (src2.length == 0 && dest2.length == 0) {            /** 1. */
        memmove(array->ring + dest1.location,
                array->ring + src1.location,
                src1.length * sizeof(void *));
    } else if (src2.length == 0 && dest2.length > 0) {      /** 2. */
        if (move < 0) {                                     /** 2.to left */
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    dest1.length * sizeof(void *));
            memmove(array->ring + dest2.location,
                    array->ring + src1.location + dest1.length,
                    dest2.length * sizeof(void *));
        } else {                                            /** 2.to right */
            memmove(array->ring + dest2.location,
                    array->ring + src1.location + dest1.length,
                    dest2.length * sizeof(void *));
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    dest1.length * sizeof(void *));
        }
    } else if (dest2.length == 0 && src2.length > 0) {      /** 3. */
        if (move < 0) {                                     /** 3.to left */
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    src1.length * sizeof(void *));
            memmove(array->ring + dest1.location + src1.length,
                    array->ring + src2.location,
                    src2.length * sizeof(void *));
        } else {                                            /** 3.to right */
            memmove(array->ring + dest1.location + src1.length,
                    array->ring + src2.location,
                    src2.length * sizeof(void *));
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    src1.length * sizeof(void *));
        }
    } else if (dest2.length > 0 && src2.length > 0) {       /** 4. */
        if (move < 0) {                                     /** 4.to left */
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    src1.length * sizeof(void *));
            memmove(array->ring + dest1.location + src1.length,
                    array->ring + src2.location,
                    (-move) * sizeof(void *));
            memmove(array->ring + dest2.location,
                    array->ring + src2.location - move,
                    dest2.length * sizeof(void *));
        } else {                                            /** 4.to right */
            memmove(array->ring + dest2.location + move,
                    array->ring + src2.location,
                    src2.length * sizeof(void *));
            memmove(array->ring + dest2.location,
                    array->ring + src1.location + dest1.length,
                    move * sizeof(void *));
            memmove(array->ring + dest1.location,
                    array->ring + src1.location,
                    (src1.length - move) * sizeof(void *));
        }
    }
}

ym_inline void _ym_array_release_range(ym_array_ref array, ym_range range) {
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    if (src1.length > 0) {
        const void **item = array->ring + src1.location;
        for (ym_uint i = 0; i < src1.length; i++, item++) {
            array->callback.release(*item);
        }
    }
    if (src2.length > 0) {
        const void **item = array->ring + src2.location;
        for (ym_uint i = 0; i < src2.length; i++, item++) {
            array->callback.release(*item);
        }
    }
}

static bool _ym_array_reposition_ring_regions(ym_array_ref array, ym_range range, ym_uint new_length) {
    ym_uint old_count = array->count;
    ym_uint old_length = range.length;
    ym_uint left_used = range.location;
    ym_uint right_used = old_count - left_used - old_length;
    
    ym_uint old_capacity = array->capacity;
    ym_uint new_capacity = array->count - range.length + new_length;
    
    if (new_capacity < old_capacity) {
        if (left_used < right_used) {
            ym_int move = old_length - new_length;
            _ym_array_move_range(array, ym_range_make(0, left_used), move);
            if ((long)array->index + move < 0) {
                array->index = array->index + move + array->capacity;
            } else {
                array->index = array->index + move;
            }
        } else {
            ym_int move = new_length - old_length;
            _ym_array_move_range(array, ym_range_make(left_used + old_length, right_used), move);
        }
    } else {
        new_capacity = _ym_array_capacity_expand(new_capacity);
        const void **new_ring = malloc(new_capacity * sizeof(void *));
        if (new_ring == NULL) {
            ym_log_error("ym_array_t(%p):%s() attempt to allocate %lu bytes failed",
                         array, __func__, new_capacity * sizeof(void *));
            return false;
        }
        
        if (left_used > 0) {
            ym_range src1, src2;
            _ym_array_split(array, ym_range_make(0, left_used), &src1, &src2);
            if (src1.length > 0) {
                memcpy(new_ring,
                       array->ring + src1.location,
                       src1.length * sizeof(void *));
            }
            if (src2.length > 0) {
                memcpy(new_ring + src1.length,
                       array->ring + src2.location,
                       src2.length * sizeof(void *));
            }
        }
        if (right_used > 0) {
            ym_range src1, src2;
            _ym_array_split(array, ym_range_make(left_used + old_length, right_used), &src1, &src2);
            if (src1.length > 0) {
                memcpy(new_ring + left_used + new_length,
                       array->ring + src1.location,
                       src1.length * sizeof(void *));
            }
            if (src2.length > 0) {
                memcpy(new_ring + left_used + new_length + src1.length,
                       array->ring + src2.location,
                       src2.length * sizeof(void *));
            }
        }
        
        free(array->ring);
        array->ring = new_ring;
        array->index = 0;
        array->capacity = new_capacity;
    }
    return true;
}

static bool _ym_array_replace_values(ym_array_ref array, ym_range range, const void **new_values, ym_uint new_length) {
    ym_uint old_count = array->count;
    ym_uint new_count = old_count - range.length + new_length;
    ym_uint old_capacity = array->capacity;
    
    /**************************** alloc memory ********************************/
    if (array->ring == NULL && new_count > 0) {
        ym_uint new_capacity = _ym_array_capacity_expand(new_count);
        array->ring = malloc(new_capacity * sizeof(void *));
        if (array->ring == NULL) {
            ym_log_error("ym_array_t(%p):%s() attempt to allocate %lu bytes failed",
                         array, __func__, new_capacity * sizeof(void *));
            return false;
        }
        array->index = 0;
        array->capacity = new_capacity;
    }
    
    /**************************** retain and release **************************/
    const void *buffer[64];
    const void **new_values_retained;
    bool retained_need_free = false;
    if (new_length > 0 && array->callback.retain) {
        if (new_length <= 64) {
            new_values_retained = buffer;
        } else {
            new_values_retained = malloc(new_length * sizeof(void *));
            if (new_values_retained == NULL) {
                ym_log_error("ym_array_t(%p):%s() attempt to allocate %lu bytes failed",
                             array, __func__, new_length * sizeof(void *));
                return false;
            }
            retained_need_free = true;
        }
        for (ym_uint i = 0; i < new_length; i++) {
            new_values_retained[i] = array->callback.retain(new_values[i]);
        }
    } else {
        new_values_retained = new_values;
    }
    
    if (range.length > 0 && array->callback.release) {
        _ym_array_release_range(array, range);
    }
    
    
    /**************************** resposition regions *************************/
    if (old_capacity > 0 && range.length != new_length) {
        bool result = _ym_array_reposition_ring_regions(array, range, new_length);
        if (!result) {
            if (retained_need_free) free(new_values_retained);
            return false;
        }
    }
    
    /**************************** copy new value ******************************/
    if (new_length > 0) {
        ym_range dest1, dest2;
        _ym_array_split(array, ym_range_make(range.location, new_length), &dest1, &dest2);
        if (dest1.length > 0) {
            memmove(array->ring + dest1.location,
                    new_values_retained,
                    dest1.length * sizeof(void *));
        }
        if (dest2.length > 0) {
            memmove(array->ring + dest2.location,
                    new_values_retained + dest1.length,
                    dest2.length * sizeof(void *));
        }
    }
    
    /**************************** finish **************************************/
    if (new_count == 0 && array->ring) {
        free(array->ring);
        array->index = 0;
        array->ring = NULL;
        array->capacity = 0;
    }
    if (retained_need_free) free(new_values_retained);
    
    array->count = new_count;
    return true;
}

static void _ym_array_dealloc(ym_array_ref array) {
    if (array->ring == NULL) return;
    if (array->callback.release && array->count > 0) {
        _ym_array_release_range(array, ym_range_make(0, array->count));
    }
    free(array->ring);
}

#pragma mark - Public Func

ym_array_ref ym_array_create() {
    return ym_array_create_with_options(0, NULL);
}

ym_array_ref ym_array_create_for_string() {
    return ym_array_create_with_options(0, &ym_array_string_callback);
}

ym_array_ref ym_array_create_for_object() {
    return ym_array_create_with_options(0, &ym_array_object_callback);
}

ym_array_ref ym_array_create_with_options(ym_uint capacity, const ym_array_callback_t *callback) {
    ym_array_ref array = ym_alloc(struct _ym_array, _ym_array_dealloc);
    if (array == NULL) {
        ym_log_error("ym_array_t:%s() attempt to allocate %lu bytes failed",
                     __func__, sizeof(struct _ym_array));
        return NULL;
    }
    if (capacity > 0) {
        capacity = _ym_array_capacity_expand(capacity);
        array->ring = calloc(capacity, sizeof(void *));
        if (array->ring == NULL) {
            ym_release(array);
            ym_log_error("ym_array_t:%s() attempt to allocate %lu bytes failed",
                         __func__, capacity * sizeof(void *));
            return NULL;
        }
        array->capacity = capacity;
    }
    
    if (callback) array->callback = *callback;
    return array;
}

ym_array_ref ym_array_create_copy(ym_array_ref array) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return NULL;
    }
    return ym_array_create_copy_range(array, ym_range_make(0, array->count));
}

ym_array_ref ym_array_create_copy_range(ym_array_ref array, ym_range range) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return NULL;
    }
    if (!_ym_array_validate_range(array, range, __func__)) {
        return NULL;
    }
    if (range.length == 0) {
        return ym_array_create_with_options(0, &array->callback);
    }
    
    ym_array_ref new_array = ym_array_create_with_options(array->count, &array->callback);
    if (new_array == NULL) {
        return NULL;
    }
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    if (src1.length > 0) {
        memmove(new_array->ring,
                array->ring + src1.location,
                src1.length * sizeof(void *));
    }
    if (src2.length > 0) {
        memmove(new_array->ring + src1.length,
                array->ring + src2.location,
                src2.length * sizeof(void *));
    }
    
    new_array->count = array->count;
    if (new_array->callback.retain) {
        for (ym_uint i = 0; i < new_array->count; i++) {
            new_array->ring[i] = new_array->callback.retain(new_array->ring[i]);
        }
    }
    return new_array;
}

const void *ym_array_get(ym_array_ref array, ym_uint index) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return NULL;
    }
    if (!_ym_array_validate_index(array, index, false, __func__)) {
        return NULL;
    }
    if (array->index + index >= array->capacity) index -= array->capacity;
    return array->ring[array->index + index];
}

ym_uint ym_array_count(ym_array_ref array) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return 0;
    }
    return array->count;
}

bool ym_array_get_range(ym_array_ref array, ym_range range, const void **values) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_range(array, range, __func__)) {
        return false;
    }
    if (range.length == 0) {
        return true;
    }
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    if (src1.length > 0) {
        memmove(values,
                array->ring + src1.location,
                src1.length * sizeof(void *));
    }
    if (src2.length > 0) {
        memmove(values + src1.length,
                array->ring + src2.location,
                src2.length * sizeof(void *));
    }
    return true;
}

bool ym_array_replace_range(ym_array_ref array, ym_range range, const void **new_values, ym_uint new_count) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_range(array, range, __func__)) {
        return false;
    }
    return _ym_array_replace_values(array, range, new_values, new_count);
}

bool ym_array_append(ym_array_ref array, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    return _ym_array_replace_values(array, ym_range_make(array->count, 0), &value, 1);
}

bool ym_array_prepend(ym_array_ref array, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    return _ym_array_replace_values(array, ym_range_make(0, 0), &value, 1);
}

bool ym_array_set(ym_array_ref array, ym_uint index, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_index(array, index, false, __func__)) {
        return false;
    }
    if (array->index + index >= array->capacity) index -= array->capacity;
    if (array->callback.retain) {
        value = array->callback.retain(value);
    }
    if (array->callback.release) {
        array->callback.release(array->ring[array->index + index]);
    }
    array->ring[array->index + index] = value;
    return true;
}

bool ym_array_insert(ym_array_ref array, ym_uint index, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_index(array, index, true, __func__)) {
        return false;
    }
    return _ym_array_replace_values(array, ym_range_make(index, 0), &value, 1);
}

bool ym_array_insert_all(ym_array_ref array, ym_uint index, ym_array_ref values) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_index(array, index, true, __func__)) {
        return false;
    }
    if (values == NULL) {
        return false;
    }
    if (values->count == 0) {
        return true;
    }
    
    ym_range src1, src2;
    _ym_array_split(values, ym_range_make(0, values->count), &src1, &src2);
    if (src1.length > 0) {
        bool result = _ym_array_replace_values(array,
                                               ym_range_make(index, 0),
                                               values->ring + src1.location,
                                               src1.length);
        if (result == false) return false;
    }
    if (src2.length > 0) {
        bool result = _ym_array_replace_values(array,
                                               ym_range_make(index + src1.length, 0),
                                               values->ring + src2.location,
                                               src2.length);
        if (result == false) return false;
    }
    return true;
}

bool ym_array_remove(ym_array_ref array, ym_uint index) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_index(array, index, false, __func__)) {
        return false;
    }
    return _ym_array_replace_values(array, ym_range_make(index, 1), NULL, 0);
}

bool ym_array_exchange(ym_array_ref array, ym_uint index1, ym_uint index2) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_index(array, index1, false, __func__)) {
        return false;
    }
    if (!_ym_array_validate_index(array, index2, false, __func__)) {
        return false;
    }
    
    if (array->index + index1 >= array->capacity) index1 -= array->capacity;
    if (array->index + index2 >= array->capacity) index2 -= array->capacity;
    
    const void *tmp = array->ring[array->index + index1];
    array->ring[array->index + index1] = array->ring[array->index + index2];
    array->ring[array->index + index2] = tmp;
    return true;
}

bool ym_array_clear(ym_array_ref array) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (array->ring == NULL) {
        return true;
    }
    if (array->callback.release && array->count > 0) {
        _ym_array_release_range(array, ym_range_make(0, array->count));
    }
    free(array->ring);
    array->capacity = 0;
    array->count = 0;
    array->ring = NULL;
    return true;
}

bool ym_array_contains(ym_array_ref array, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    return ym_array_find_first_index(array, ym_range_make(0, array->count), value) != YM_NOT_FOUND;
}

ym_uint ym_array_find_first_index(ym_array_ref array, ym_range range, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return YM_NOT_FOUND;
    }
    if (!_ym_array_validate_range(array, range, __func__) || range.length == 0) {
        return YM_NOT_FOUND;
    }
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    ym_uint index = range.location;
    if (src1.length > 0) {
        const void **item = array->ring + src1.location;
        for (ym_uint i = 0; i < src1.length; i++, index++, item++) {
            if (*item == value
                || (array->callback.equal && array->callback.equal(*item, value))) {
                return index;
            }
        }
    }
    if (src2.length > 0) {
        const void **item = array->ring + src2.location;
        for (ym_uint i = 0; i < src2.length; i++, index++, item++) {
            if (*item == value
                || (array->callback.equal && array->callback.equal(*item, value))) {
                return index;
            }
        }
    }
    return YM_NOT_FOUND;
}

ym_uint ym_array_find_last_index(ym_array_ref array, ym_range range, const void *value) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return YM_NOT_FOUND;
    }
    if (!_ym_array_validate_range(array, range, __func__) || range.length == 0) {
        return YM_NOT_FOUND;
    }
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    ym_uint index = range.location + range.length - 1;
    if (src2.length > 0) {
        const void **item = array->ring + src2.location;
        for (ym_uint i = src2.length; i > 0; i--, index--, item--) {
            if (*item == value
                || (array->callback.equal && array->callback.equal(*item, value))) {
                return index;
            }
        }
    }
    if (src1.length > 0) {
        const void **item = array->ring + src1.location + src1.length - 1;
        for (ym_uint i = src1.length; i > 0; i--, index--, item--) {
            if (*item == value
                || (array->callback.equal && array->callback.equal(*item, value))) {
                return index;
            }
        }
    }
    return YM_NOT_FOUND;
}

bool ym_array_sort(ym_array_ref array, ym_comparator_func cmp, void *context) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    return ym_array_sort_range(array, ym_range_make(0, array->count), cmp, context);
}

bool ym_array_sort_range(ym_array_ref array, ym_range range, ym_comparator_func cmp, void *context) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!_ym_array_validate_range(array, range, __func__)) {
        return false;
    }
    if (range.length <= 1) {
        return true;
    }
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    if (src2.length > 0) { // value not continuous in memory
        if (src1.length < src2.length) {
            _ym_array_move_range(array, range, src1.length);
        } else {
            _ym_array_move_range(array, range, -((long)src2.length));
        }
    }
    if (array->index + range.location >= array->capacity) range.location -= array->capacity;
    _ym_quick_sort(array->ring + array->index + range.location, range.length, cmp, context);
    return true;
}

bool ym_array_foreach(ym_array_ref array, ym_array_foreach_func func, void *context) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    return ym_array_foreach_range(array, ym_range_make(0, array->count), func, context);
}

bool ym_array_foreach_range(ym_array_ref array, ym_range range, ym_array_foreach_func func, void *context) {
    if (array == NULL) {
        ym_log_error("%s() input array cannot be null", __func__);
        return false;
    }
    if (!func) return false;
    if (!_ym_array_validate_range(array, range, __func__)) return false;
    
    ym_range src1, src2;
    _ym_array_split(array, range, &src1, &src2);
    ym_uint index = range.location;
    if (src1.length > 0) {
        const void **item = array->ring + src1.location;
        for (ym_uint i = 0; i < src1.length; i++, index++, item++) {
            func(index, *item, context);
        }
    }
    if (src2.length > 0) {
        const void **item = array->ring + src2.location;
        for (ym_uint i = 0; i < src2.length; i++, index++, item++) {
            func(index, *item, context);
        }
    }
    return true;
}
