//
//  ym_task.h
//  yy_music
//
//  Created by ibireme on 14-3-16.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#ifndef __YM_TASK__
#define __YM_TASK__

#include "ym_base.h"

YM_EXTERN_C_BEGIN

#ifndef YM_TASK_DEFAULT_ARGS_SIZE
#define YM_TASK_DEFAULT_ARGS_SIZE 128
#endif

/*
 something like closure
 
 example:
 
 
 void my_task_func(ym_task_t *task) {
 int i;
 float *p;
 struct my_task_cfg cfg;
 
 ym_task_arg_get(task, int, i);
 ym_task_arg_get(task, float *, p);
 ym_task_arg_get(task, struct my_task_cfg, cfg);
 
 printf("exec: %d, %f, a:%i b:%f\n",i, *p, cfg.a, cfg.b);
 }
 
 void do_test_task() {
 int i = 1;
 float p = 2.22222;
 struct my_task_cfg cfg = {5555,6666.666};
 
 printf("init: %d, %f, a:%i b:%f\n",i, p, cfg.a, cfg.b);
 
 ym_task_t *task = ym_task_create(my_task_func, NULL);
 ym_task_arg_set(task, int, i);
 ym_task_arg_set(task, float *, &p);
 ym_task_arg_set(task, struct my_task_cfg, cfg);
 
 ym_task_exec(task);
 ym_release(task);
 }
 */

typedef struct _ym_task *ym_task_ref;

typedef void (*ym_task_func)(ym_task_ref task);
typedef void (*ym_task_dealloc_func)(ym_task_ref task);

struct _ym_task {
    ym_task_func func;
    ym_task_dealloc_func dealloc;
    char *args;
    int incr;
};


ym_task_ref ym_task_create(ym_task_func func, ym_task_dealloc_func dealloc);
ym_task_ref ym_task_create_with_size(ym_task_func func, int args_size, ym_task_dealloc_func dealloc);
void ym_task_exec(ym_task_ref task);

#define ym_task_arg_begin(task) \
do { task->incr = 0; } while (0)

#define ym_task_arg_set(task, type, value) \
do { *(type *)&(task->args[task->incr]) = (value); task->incr += sizeof(type); } while (0)

#define ym_task_arg_get(task, type, value) \
do { (value) = *(type *)&(task->args[task->incr]); task->incr += sizeof(type); } while (0)

YM_EXTERN_C_END
#endif
