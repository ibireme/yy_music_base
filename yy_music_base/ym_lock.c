//
//  ym_lock.c
//  yy_music
//
//  Created by ibireme on 14/12/27.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_lock.h"


#if YM_OS_APPLE
void ym_spin_lock(ym_spin_lock_t *lock) {
    OSSpinLockLock(lock);
}

void ym_spin_unlock(ym_spin_lock_t *lock) {
    OSSpinLockUnlock(lock);
}

#elif YM_OS_LINUX
void ym_spin_lock(ym_spin_lock_t *lock) {
    spin_lock(lock);
}
void ym_spin_unlock(ym_spin_lock_t *lock) {
    spin_unlock(lock);
}

#elif YM_OS_WINDOWS
void ym_spin_lock(ym_spin_lock_t *lock) {
    while (InterlockedExchange(lock, (LONG) 1) == 1) {
        // Try again, until somebody else changes the value to 0
        // with "ym_spin_unlock()".
    }
}

void ym_spin_unlock(ym_spin_lock_t *lock) {
    InterlockedExchange(lock, (LONG) 0);
}

#endif









#include <pthread.h>

struct _ym_mutex {
    pthread_mutex_t handler;
    bool destory_when_dealloc;
};

static void _ym_mutex_dealloc(ym_mutex_ref mutex) {
    if (mutex->destory_when_dealloc) {
        pthread_mutex_destroy(&mutex->handler);
    }
}

ym_mutex_ref ym_mutex_create() {
    ym_mutex_ref mutex = ym_alloc(struct _ym_mutex, _ym_mutex_dealloc);
    if (mutex) {
        if (pthread_mutex_init(&mutex->handler, NULL) != 0) {
            ym_release(mutex);
            return NULL;
        }
        mutex->destory_when_dealloc = true;
    }
    return mutex;
}

bool ym_mutex_lock(ym_mutex_ref mutex) {
    return pthread_mutex_lock(&mutex->handler) != 0;
}

bool ym_mutex_unlock(ym_mutex_ref mutex) {
    return pthread_mutex_unlock(&mutex->handler) != 0;
}

bool ym_mutex_trylock(ym_mutex_ref mutex) {
    return pthread_mutex_trylock(&mutex->handler) != 0;
}

