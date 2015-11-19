//
//  main.c
//  yy_music
//
//  Created by guoyaoyuan on 14/10/15.
//  Copyright (c) 2014 ibireme. All rights reserved.
//


#import <Foundation/Foundation.h>
#import "ym_exs.h"

int main(int argc, const char * argv[]) {
    
    const char *path = "/Library/Application Support/GarageBand/Instrument Library/Sampler/Sampler Instruments/Grand Piano.exs";
    
    ym_error_code error = YM_ERROR_NONE;
    ym_exs_ref exs = ym_exs_create_from_path(path, &error);
    
    if (error == YM_ERROR_NONE) {
        printf("name:%s\n",exs->header->name);
        
        printf("samples:\n");
        for (ym_uint i = 0, max = ym_array_count(exs->samples); i < max; i++) {
            ym_exs_sample_t *sample = (ym_exs_sample_t *)ym_array_get(exs->samples, i);
            printf("%s/%s\n",sample->file_path, sample->name);
        }
        
    } else {
        printf("error:%d",error);
    }
    
    ym_release(exs);
    
    printf("\n --fin-- \n");
    return 0;
}
