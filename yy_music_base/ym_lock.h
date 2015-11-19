//
//  ym_lock.h
//  yy_music
//
//  Created by ibireme on 14/12/27.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __ym_lock__
#define __ym_lock__

#include "ym_base.h"

YM_EXTERN_C_BEGIN


/*
 spin lock 
 
 usage:
 ym_spin_lock_t lock = YM_SPIN_LOCK_INIT;
 ym_spin_lock(lock);
 ...
 ym_spin_unlock(lock);
 */

#if YM_OS_APPLE
#include <libkern/OSAtomic.h>
typedef OSSpinLock ym_spin_lock_t;
#define YM_SPIN_LOCK_INIT OS_SPINLOCK_INIT

#elif YM_OS_LINUX
#include <linux/spinlock.h>
typedef spinlock_t ym_spin_lock_t;
#define YM_SPIN_LOCK_INIT SPIN_LOCK_UNLOCKED

#elif YM_OS_WINDOWS
#include <windows.h>
typedef LONG ym_spin_lock_t;
#define YM_SPIN_LOCK_INIT 0
#endif

void ym_spin_lock(ym_spin_lock_t *lock);
void ym_spin_unlock(ym_spin_lock_t *lock);



/*
 mutex lock
 
 usage:
 ym_mutex_ref mutex = ym_mutex_create();
 ym_mutex_lock(mutex);
 ...
 ym_mutex_unlock(mutex);
 */

typedef struct _ym_mutex *ym_mutex_ref;

ym_mutex_ref ym_mutex_create();

bool ym_mutex_lock(ym_mutex_ref mutex);
bool ym_mutex_unlock(ym_mutex_ref mutex);
bool ym_mutex_trylock(ym_mutex_ref mutex);


YM_EXTERN_C_END
#endif
