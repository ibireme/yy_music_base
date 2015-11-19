//
//  ym_exs.h
//  yy_music
//
//  Created by ibireme on 14-7-1.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

/**
 EXS24 mkII (used by Apple Logic Pro / GarageBand)
 */

#ifndef __YM_EXS__
#define __YM_EXS__

#include "ym_base.h"
#include "ym_array.h"

YM_EXTERN_C_BEGIN

typedef enum {
    YM_EXS_DEST_NONE                 = 0,

    YM_EXS_DEST_SAMPLE_SELECT        = 2,

    YM_EXS_DEST_SAMPLE_START         = 4,
    YM_EXS_DEST_GLIDE_TIME           = 5,
    YM_EXS_DEST_PITCH                = 6,
    YM_EXS_DEST_FILTER_DRIVE         = 7,
    YM_EXS_DEST_FILTER_CUTOFF        = 8,
    YM_EXS_DEST_FILTER_RESONANCE     = 9,
    YM_EXS_DEST_VOLUME               = 10,
    YM_EXS_DEST_PAN                  = 11,
    YM_EXS_DEST_RELATIVE_VOLUME      = 12,
    YM_EXS_DEST_RELATIVE_VOLUME_AUTO = 13,

    YM_EXS_DEST_LFO1_DCY_DLY         = 15,
    YM_EXS_DEST_LFO1_SPEED           = 16,
    YM_EXS_DEST_LFO2_SPEED           = 17,
    YM_EXS_DEST_LFO3_SPEED           = 18,

    YM_EXS_DEST_ENV1_ATTACK          = 20,
    YM_EXS_DEST_ENV1_DECAY           = 21,
    YM_EXS_DEST_ENV1_RELEASE         = 22,
    YM_EXS_DEST_TIME                 = 23,
    YM_EXS_DEST_ENV2_ATTACK          = 24,
    YM_EXS_DEST_ENV2_DECAY           = 25,
    YM_EXS_DEST_ENV2_RELEASE         = 26,

    YM_EXS_DEST_HOLD                 = 28,

    YM_EXS_DEST_ARTICULATION_ID      = 30,
}ym_exs_dest_t;


typedef enum {
    YM_EXS_SRC_SIDE_CHAIN       = -17,
    YM_EXS_SRC_MAXMIUM          = -16,

    YM_EXS_SRC_ENV1             = -14,
    YM_EXS_SRC_ENV2             = -13,
    YM_EXS_SRC_LFO1             = -12,
    YM_EXS_SRC_LFO2             = -11,
    YM_EXS_SRC_LFO3             = -10,

    YM_EXS_SRC_RELEASE_VELOCITY = -8,
    YM_EXS_SRC_PRESSURE         = -7,

    YM_EXS_SRC_PITCH_BEND       = -5,
    YM_EXS_SRC_KEY              = -4,
    YM_EXS_SRC_VELOCITY         = -3,

    YM_EXS_SRC_NONE             = -1,

    /* 1~120 is Ctrl#1~Ctrl#120 */
    /* 7 and 10 Not Available */
    /* 11 Expression */
} ym_exs_src_via_t;



typedef struct _ym_exs_header *ym_exs_header_ref;
struct _ym_exs_header {
    uint32_t id;
    char name[65];
    // param unknown...
};

typedef struct _ym_exs_zone *ym_exs_zone_ref;
struct _ym_exs_zone {
    uint32_t id;
    char name[65];
    
    /* Index */
    int32_t group_index; ///< ungrouped:-1 grouped:0,1,2...
    int32_t sample_index; ///< nosample:-1 sample:0,1,2...
    
    /* Pitch */
    uint8_t key; ///< [0,127] root key (midi note from C-1 to G9) (logic note from C-2 to G8)
    int8_t coarse_tuning; ///< [-50,50]
    int8_t fine_tuning; ///< [-50,50]
    
    /* Mixer */
    int8_t volume; ///< [-96,12] dB
    int8_t pan; ///< [-100,100]
    int8_t scale; ///< [-6,6] dB
    int8_t output; ///< Group:-1  Main:0  3-4:1  5-6:2 ... 25-26:15
    
    /* Key Range */
    uint8_t key_lo; ///< [0,127] <=hi
    uint8_t key_hi; ///< [0,127] >=lo
    
    /* Play Back */
    bool pitch;
    bool oneshot;
    bool revert;
    int32_t offset; ///< [0,10000]
    
    /* Vel. Range */
    bool vel_range_on;
    uint8_t vel_lo; ///< [0,127] <=hi
    uint8_t vel_hi; ///< [0,127] >=lo
    
    /* Sample */
    int32_t sample_start;
    int32_t sample_end;
    int32_t sample_fade;
    
    /* Loop */
    bool loop_on;
    int32_t loop_start;
    int32_t loop_end;
    int8_t loop_tune; ///< [-50,50]
    int32_t loop_crossfade;
    bool loop_equal_power;
};


typedef struct _ym_exs_group *ym_exs_group_ref;
struct _ym_exs_group {
    uint32_t id;
    char name[65];
    
    /* Key Range */
    uint8_t key_lo; ///< [0,127] <=hi
    uint8_t key_hi; ///< [0,127] >=lo
    
    /* Mixer */
    int8_t volume; ///< [-96,12] dB
    int8_t pan; ///< [-100,100]
    int8_t output; ///< Main:0  3-4:1  5-6:2 ... 25-26:15
    
    /* Voices */
    uint8_t polyphony; ///< [0,16] 0:max    1~16:limit
    uint8_t exclusive; ///< [0,16] 0:empty  1~16:class 1~16
    uint8_t trigger;   ///< 0:key down      1:key release
    bool decay;        ///< invalid when trigger=0
    uint32_t decay_time; ///< [0,1000]  invalid when trigger=0
    
    /* Filter Offsets */
    int8_t cutoff;  ///< [-50,50]
    int8_t resonance; ///< [-50,50]
    
    /* Envelop 1 Offsets */
    int32_t a1, h1, d1, s1, r1; ///< [-9999,9999] ms  invalid when trigger=1
    
    /* Envelop 2 (Amp) Offsets */
    int32_t a2, h2, d2, s2, r2; ///< [-9999,9999] ms  invalid when trigger=1
    
    /* Vel. Range */
    uint8_t vel_lo; ///< [0,127] <=hi
    uint8_t vel_hi; ///< [0,127] >=lo
    
    /* Select Group By */
    uint8_t select_type; ///< 0:-- 1:Note 2:Group 3:Control 4:Bend 5:Midi Channel 6:Articulation ID 7:Tempo
    uint8_t select_number;
    int32_t select_group;
    uint8_t select_lo; ///< [0,127] <=hi
    uint8_t select_hi; ///< [0,127] >=lo
};


typedef struct _ym_exs_sample ym_exs_sample_t;
struct _ym_exs_sample {
    uint32_t id;
    char name[65];
    
    uint32_t type; ///< file type, 'WAVE' 'AIFF' 'caff' (wav/aif/caf)
    int32_t length;
    int32_t sample_rate; ///< standard:44100
    uint8_t bit_depth; ///< standard:16 (bit)
    
    char file_path[257]; ///< max 64, with '\0' padding  (e.g.  /Users/Library/aaa )
    char file_name[257]; ///< max 64, with '\0' padding  (e.g.  bbb.wav)
};


typedef struct _ym_exs_param *ym_exs_param_t;
struct _ym_exs_param {
    uint32_t id;
    char name[65];
    
    /* Global Params */
    int16_t output_volume; ///< [-60,0]dB default:-6
    int16_t key_scale;///< [-24,24]dB default:0
    int16_t pitch_bend_up; ///< [0,12] default:2
    int16_t pitch_bend_down; ///< [-1,36] default:-1(Linked)
    int16_t mono_mode; ///< 0:poly(mono off) 1:mono 2:legato
    int16_t voices; ///< [1,64] default:16
    bool unison;
    
    /* Start via Vel */
    int16_t transpose; ///< [-24,24] default:0
    int16_t coarse_tune; ///< [-24,24] default:0
    int16_t fine_tune; ///< [-50,50]cent default:0
    int16_t glide_time; ///< [0,127] for [0,5000]ms, unknown mapping
    int16_t pitcher; ///< [-12,12] default:0  <=picher_via_vel
    int16_t pitcher_via_vel; ///< [-12,12] default:0  >=picher
    
    /* Pitch Mod. Wheel */
    bool filter_on; ///<
    int16_t filter_type; ///< 5:HP12 2:LP24 1:LP18 0:LP12 3:LP6 4:BP12  (default:0)
    bool filter_fat; ///<
    
    /* Filter Mod. Wheel */
    int16_t filter_cutoff; ///< [0,1000] for (0~100%), default:1000
    int16_t filter_resonance; ///< [0,1000] for (0~100%), default:0
    int16_t filter_drive; ///< [0,1000] for (0~100%), default:100
    int16_t filter_via_key; ///< [0,1000] for (0~100%), default:0
    
    /* Filter ADSR via Vel */
    int16_t level_fixed; ///< [-96,0] for (-48dB~0dB), default:0   <=level_via_vel
    int16_t level_via_vel; ///< [-96,0] for (-48dB~0dB), default:0   >=level_fixed;
    
    /* Tremolo */
    int16_t lfo1_decay_delay; ///< [-9999,9999]ms negative:decay positive:delay
    int16_t lfo1_rate; ///< [0,127] default:98(4.8Hz)
    int16_t lfo1_waveform; ///< [0,6]
    int16_t lfo2_waveform; ///< [0,6]
    int16_t lfo2_rate; ///< [0,127] default:34(DC)
    int16_t lfo3_rate; ///< [0,127] default:98(4.8Hz)
    
    int16_t env1_attack; ///< [0,127] for (0~10000ms), <=env1_attack_via_vel @see: ym_exs_time_to_second()
    int16_t env1_attack_via_vel; ///< [0,127] for (0~10000ms), >=env1_attack @see: ym_exs_time_to_second()
    int16_t env1_decay; ///< [0,127] for (0~10000ms), @see: ym_exs_time_to_second
    int16_t env1_sustain; ///< [0,127] for (0,100%), liner mapping, default:0
    int16_t env1_release; ///< [0,127] for (0~10000ms), @see: ym_exs_time_to_second
    
    int16_t time_via;  ///< [0,100]cent default:0
    int16_t time_curve; ///< [-99,99] default:0
    
    int16_t env2_attack; ///< [0,127] for (0~10000ms), <=env1_attack_via_vel @see: ym_exs_time_to_second()
    int16_t env2_attack_via_vel; ///< [0,127] for (0~10000ms), >=env1_attack @see: ym_exs_time_to_second()
    int16_t env2_decay; ///< [0,127] for (0~10000ms), @see: ym_exs_time_to_second()
    int16_t env2_sustain; ///< [0,127] for (0,100%), liner mapping, default:127
    int16_t env2_release; ///< [0,127] for (0~10000ms), @see: ym_exs_time_to_second()
    
    int16_t velocity_offset; ///< [-127,127] default:0
    int16_t velocity_random; ///< [0,127] default:0
    int16_t velocity_x_fade; ///< [0,127] default:0
    int16_t velocity_x_fade_type; ///<  0:dB lin 1:liner 2:Eq.Pow
    int16_t coarse_tune_remote; ///< [-1,127] default:-1(OFF)
    int16_t hold_via; ///< [-17,120] default:64  @see: ym_exs_src_via_t
    int16_t sample_select_random; ///< [0,127] default:0
    int16_t random_detune; ///< [0,50]cent default:0
    
    /* Modulator */
    int16_t destinaion[10]; ///< [0,30] @see: ym_exs_dest_t
    int16_t source[10]; ///< [-17,120]  @see: ym_exs_src_via_t
    int16_t via[10]; ///< [-17,120]  @see: ym_exs_src_via_t
    int16_t amount[10]; ///< [-1000,1000] for (-100%~100%)  <=amount_via
    int16_t amount_via[10]; ///< [-1000,1000] for (-100%~100%)  >=amount
    bool invert[10];
    bool invert_via[10];
    bool bypass[10];
};




typedef struct _ym_exs *ym_exs_ref;
struct _ym_exs {
    ym_exs_header_ref header;
    ym_exs_param_t param;
    ym_array_ref zones; ///< ym_exs_zone_ref
    ym_array_ref groups; ///< ym_exs_group_ref
    ym_array_ref samples; ///< ym_exs_sample_t
};


ym_exs_ref ym_exs_create_from_data(const void *data, size_t data_size, ym_error_code *error);
ym_exs_ref ym_exs_create_from_file(FILE *fd, ym_error_code *error);
ym_exs_ref ym_exs_create_from_path(const char *path, ym_error_code *error);

double ym_exs_time_to_second(int16_t time);

YM_EXTERN_C_END
#endif
