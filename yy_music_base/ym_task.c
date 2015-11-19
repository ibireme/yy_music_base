//
//  ym_task.c
//  yy_music
//
//  Created by ibireme on 14-3-16.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_task.h"


static void _ym_task_dealloc(ym_task_ref task) {
    if (task->dealloc) {
        ym_task_arg_begin(task);
        task->dealloc(task);
    }
    if (task->args) free(task->args);
}

ym_task_ref ym_task_create(ym_task_func func, ym_task_dealloc_func dealloc) {
    return ym_task_create_with_size(func, YM_TASK_DEFAULT_ARGS_SIZE, dealloc);
}

ym_task_ref ym_task_create_with_size(ym_task_func func, int args_size, ym_task_dealloc_func dealloc) {
    ym_task_ref task;
    
    if (func == NULL || args_size <= 1) {
        return NULL;
    }
    
    task = ym_alloc(struct _ym_task, _ym_task_dealloc);
    if (task) {
        task->func = func;
        task->dealloc = dealloc;
        task->args = calloc(1, args_size);
        if (task->args == NULL) {
            ym_release(task);
            return NULL;
        }
    }
    return task;
}

void ym_task_exec(ym_task_ref task) {
    if (task == NULL) return;
    ym_task_arg_begin(task);
    task->func(task);
}
