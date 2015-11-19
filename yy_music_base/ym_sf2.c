//
//  ym_sf2.c
//  yy_music
//
//  Created by ibireme on 14-3-2.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_sf2.h"
#include "ym_util.h"
#include "ym_math.h"

static void _ym_sf2_preset_dealloc(ym_sf2_preset_ref preset) {
    if (preset->name) free((void *)preset->name);
    ym_release(preset->samples);
}

ym_sf2_preset_ref ym_sf2_preset_create(char *name, uint16_t bank, uint16_t patch) {
    ym_sf2_preset_ref preset;
    
    preset = ym_alloc(struct _ym_sf2_preset, _ym_sf2_preset_dealloc);
    if (preset) {
        preset->bank = bank;
        preset->patch = patch;
        if (name) {
            preset->name = strdup(name);
        }
        preset->samples = ym_array_create_for_object();
        if (preset->samples == NULL) {
            ym_release(preset);
            return NULL;
        }
    }
    return preset;
}

static void _ym_sf2_sample_dealloc(ym_sf2_sample_ref sample) {
    if (sample->name) free((void *)sample->name);
    ym_release(sample->data);
}

ym_sf2_sample_ref ym_sf2_sample_create(const char *name) {
    ym_sf2_sample_ref sample;
    
    sample = ym_alloc(struct _ym_sf2_sample, _ym_sf2_sample_dealloc);
    
    if (sample && name) {
        sample->name = strdup(name);
    }
    return sample;
}

ym_inline void *_ym_sf2_preset_key(uint16_t bank, uint16_t patch) {
    return (void *)(long)((bank << 16) | (patch));
}

typedef enum {
    YM_SF2_GEN_Instrument                 = 41,
    YM_SF2_GEN_Sample                     = 53,
    
    YM_SF2_GEN_KeyRange                   = 43,
    YM_SF2_GEN_VelRange                   = 44,
    YM_SF2_GEN_Keynum                     = 46,
    YM_SF2_GEN_Velocity                   = 47,
    YM_SF2_GEN_CoarseTune                 = 51,
    YM_SF2_GEN_FineTune                   = 52,
    YM_SF2_GEN_ScaleTuning                = 56,
    YM_SF2_GEN_OverridingRootKey          = 58,
    
    YM_SF2_GEN_InitialAttenuation         = 48,
    YM_SF2_GEN_Pan                        = 17,
    
    YM_SF2_GEN_InitialFilterFc            = 8,
    YM_SF2_GEN_InitialFilterQ             = 9,
    
    YM_SF2_GEN_DelayVolEnv                = 33,
    YM_SF2_GEN_AttackVolEnv               = 34,
    YM_SF2_GEN_HoldVolEnv                 = 35,
    YM_SF2_GEN_DecayVolEnv                = 36,
    YM_SF2_GEN_SustainVolEnv              = 37,
    YM_SF2_GEN_ReleaseVolEnv              = 38,
    YM_SF2_GEN_KeynumToVolEnvHold         = 39,
    YM_SF2_GEN_KeynumToVolEnvDecay        = 40,
    
    YM_SF2_GEN_DelayModEnv                = 25,
    YM_SF2_GEN_AttackModEnv               = 26,
    YM_SF2_GEN_HoldModEnv                 = 27,
    YM_SF2_GEN_DecayModEnv                = 28,
    YM_SF2_GEN_SustainModEnv              = 29,
    YM_SF2_GEN_ReleaseModEnv              = 30,
    YM_SF2_GEN_ModEnvToPitch              = 7,
    YM_SF2_GEN_ModEnvToFilterFc           = 11,
    YM_SF2_GEN_KeynumToModEnvHold         = 31,
    YM_SF2_GEN_KeynumToModEnvDecay        = 32,
    
    YM_SF2_GEN_DelayModLFO                = 21,
    YM_SF2_GEN_FreqModLFO                 = 22,
    YM_SF2_GEN_ModLfoToVolume             = 13,
    YM_SF2_GEN_ModLfoToPitch              = 5,
    YM_SF2_GEN_ModLfoToFilterFc           = 10,
    
    YM_SF2_GEN_DelayVibLFO                = 23,
    YM_SF2_GEN_FreqVibLFO                 = 24,
    YM_SF2_GEN_VibLfoToPitch              = 6,
    
    YM_SF2_GEN_ChorusEffectsSend          = 15,
    YM_SF2_GEN_ReverbEffectsSend          = 16,
    
    YM_SF2_GEN_SampleModes                = 54,
    YM_SF2_GEN_ExclusiveClass             = 57,
    
    YM_SF2_GEN_StartAddrsOffset           = 0,
    YM_SF2_GEN_StartAddrsCoarseOffset     = 4,
    YM_SF2_GEN_EndAddrsOffset             = 1,
    YM_SF2_GEN_EndAddrsCoarseOffset       = 12,
    YM_SF2_GEN_StartloopAddrsOffset       = 2,
    YM_SF2_GEN_StartloopAddrsCoarseOffset = 45,
    YM_SF2_GEN_EndloopAddrsOffset         = 3,
    YM_SF2_GEN_EndloopAddrsCoarseOffset   = 50,
    
    YM_SF2_GEN_Reserved1                  = 42,
    YM_SF2_GEN_Reserved2                  = 49,
    YM_SF2_GEN_Reserved3                  = 55,
    YM_SF2_GEN_Unused1                    = 14,
    YM_SF2_GEN_Unused2                    = 18,
    YM_SF2_GEN_Unused3                    = 19,
    YM_SF2_GEN_Unused4                    = 20,
    YM_SF2_GEN_Unused5                    = 59,
    YM_SF2_GEN_EOF                        = 60,
} ym_sf2_gen_type;




typedef enum {
    YM_SF2_GEN_MERGE_ADD,       /* add to global */
    YM_SF2_GEN_MERGE_OVERWRITE, /* overwrite global */
    YM_SF2_GEN_MERGE_FILL,      /* fill from global if empty */
    YM_SF2_GEN_MERGE_RANGE,     /* narrow range */
} ym_sf2_gen_merge_policy;


typedef struct _ym_sf2_gen_def {
    ym_sf2_gen_merge_policy policy;
    int32_t min;
    int32_t max;
    int32_t def;
} ym_sf2_gen_def;


#define RANGE(lo, hi) (((hi) & 0xff) << 8 | ((lo) & 0xff))
#define SMIN -0x7FFF
#define SMAX 0x7FFF

ym_sf2_gen_def sf2_gen_def[YM_SF2_GEN_EOF] = {
    { YM_SF2_GEN_MERGE_ADD,       0,      SMAX,  0      }, /* 0 YM_SF2_GEN_StartAddrsOffset */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   0,     0      }, /* 1 YM_SF2_GEN_EndAddrsOffset */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   SMAX,  0      }, /* 2 YM_SF2_GEN_StartloopAddrsOffset */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   SMAX,  0      }, /* 3 YM_SF2_GEN_EndloopAddrsOffset */
    { YM_SF2_GEN_MERGE_ADD,       0,      SMAX,  0      }, /* 4 YM_SF2_GEN_StartAddrsCoarseOffset */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 12000, 0      }, /* 5 YM_SF2_GEN_ModLfoToPitch */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 12000, 0      }, /* 6 YM_SF2_GEN_VibLfoToPitch */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 12000, 0      }, /* 7 YM_SF2_GEN_ModEnvToPitch */
    { YM_SF2_GEN_MERGE_ADD,       1500,   13500, 13500  }, /* 8 YM_SF2_GEN_InitialFilterFc */
    { YM_SF2_GEN_MERGE_ADD,       0,      960,   0      }, /* 9 YM_SF2_GEN_InitialFilterQ */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 12000, 0      }, /* 10 YM_SF2_GEN_ModLfoToFilterFc */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 12000, 0      }, /* 11 YM_SF2_GEN_ModEnvToFilterFc */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   SMAX,  0      }, /* 12 YM_SF2_GEN_EndAddrsCoarseOffset */
    { YM_SF2_GEN_MERGE_ADD,       -960,   960,   0      }, /* 13 YM_SF2_GEN_ModLfoToVolume */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 14 YM_SF2_GEN_Unused1 */
    { YM_SF2_GEN_MERGE_ADD,       0,      1000,  0      }, /* 15 YM_SF2_GEN_ChorusEffectsSend */
    { YM_SF2_GEN_MERGE_ADD,       0,      1000,  0      }, /* 16 YM_SF2_GEN_ReverbEffectsSend */
    { YM_SF2_GEN_MERGE_ADD,       -500,   500,   0      }, /* 17 YM_SF2_GEN_Pan */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 18 YM_SF2_GEN_Unused2 */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 19 YM_SF2_GEN_Unused3 */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 20 YM_SF2_GEN_Unused4 */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 21 YM_SF2_GEN_DelayModLFO */
    { YM_SF2_GEN_MERGE_ADD,       -16000, 4500,  0      }, /* 22 YM_SF2_GEN_FreqModLFO */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 23 YM_SF2_GEN_DelayVibLFO */
    { YM_SF2_GEN_MERGE_ADD,       -16000, 4500,  0      }, /* 24 YM_SF2_GEN_FreqVibLFO */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 25 YM_SF2_GEN_DelayModEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 26 YM_SF2_GEN_AttackModEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 27 YM_SF2_GEN_HoldModEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 28 YM_SF2_GEN_DecayModEnv */
    { YM_SF2_GEN_MERGE_ADD,       0,      1000,  0      }, /* 29 YM_SF2_GEN_SustainModEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 30 YM_SF2_GEN_ReleaseModEnv */
    { YM_SF2_GEN_MERGE_ADD,       -1200,  1200,  0      }, /* 31 YM_SF2_GEN_KeynumToModEnvHold */
    { YM_SF2_GEN_MERGE_ADD,       -1200,  1200,  0      }, /* 32 YM_SF2_GEN_KeynumToModEnvDecay */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 33 YM_SF2_GEN_DelayVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 34 YM_SF2_GEN_AttackVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 5000,  -12000 }, /* 35 YM_SF2_GEN_HoldVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 36 YM_SF2_GEN_DecayVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       0,      1440,  0      }, /* 37 YM_SF2_GEN_SustainVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       -12000, 8000,  -12000 }, /* 38 YM_SF2_GEN_ReleaseVolEnv */
    { YM_SF2_GEN_MERGE_ADD,       -1200,  1200,  0      }, /* 39 YM_SF2_GEN_KeynumToVolEnvHold */
    { YM_SF2_GEN_MERGE_ADD,       -1200,  1200,  0      }, /* 40 YM_SF2_GEN_KeynumToVolEnvDecay */
    { YM_SF2_GEN_MERGE_FILL,      0,      0,     0      }, /* 41 YM_SF2_GEN_Instrument */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 42 YM_SF2_GEN_Reserved1 */
    { YM_SF2_GEN_MERGE_RANGE,     0,      0,     RANGE(0, 127) }, /* 43 YM_SF2_GEN_KeyRange */
    { YM_SF2_GEN_MERGE_RANGE,     0,      0,     RANGE(0, 127) }, /* 44 YM_SF2_GEN_VelRange */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   SMAX,  0      }, /* 45 YM_SF2_GEN_StartloopAddrsCoarseOffset */
    { YM_SF2_GEN_MERGE_OVERWRITE, 0,      127,   -1     }, /* 46 YM_SF2_GEN_Keynum */
    { YM_SF2_GEN_MERGE_OVERWRITE, 1,      127,   -1     }, /* 47 YM_SF2_GEN_Velocity */
    { YM_SF2_GEN_MERGE_ADD,       0,      1440,  0      }, /* 48 YM_SF2_GEN_InitialAttenuation */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 49 YM_SF2_GEN_Reserved2 */
    { YM_SF2_GEN_MERGE_ADD,       SMIN,   SMAX,  0      }, /* 50 YM_SF2_GEN_EndloopAddrsCoarseOffset */
    { YM_SF2_GEN_MERGE_ADD,       -120,   120,   0      }, /* 51 YM_SF2_GEN_CoarseTune */
    { YM_SF2_GEN_MERGE_ADD,       -99,    99,    0      }, /* 52 YM_SF2_GEN_FineTune */
    { YM_SF2_GEN_MERGE_FILL,      0,      0,     0      }, /* 53 YM_SF2_GEN_Sample */
    { YM_SF2_GEN_MERGE_OVERWRITE, 0,      0,     0      }, /* YM_SF2_GEN_SampleModes 54 */
    { YM_SF2_GEN_MERGE_OVERWRITE, 0,      0,     0      }, /* 55 YM_SF2_GEN_Reserved3 */
    { YM_SF2_GEN_MERGE_ADD,       0,      1200,  100    }, /* 56 YM_SF2_GEN_ScaleTuning */
    { YM_SF2_GEN_MERGE_OVERWRITE, 1,      127,   0      }, /* 57 YM_SF2_GEN_ExclusiveClass */
    { YM_SF2_GEN_MERGE_OVERWRITE, 0,      127,   -1     }, /* 58 YM_SF2_GEN_OverridingRootKey */
    { YM_SF2_GEN_MERGE_ADD,       0,      0,     0      }, /* 59 YM_SF2_GEN_Unused5 */
};




typedef union _ym_sf2_generator_amount
{
    int16_t sword;
    uint16_t uword;
    struct {
        uint8_t lo;
        uint8_t hi;
    } range;
} ym_sf2_gen_amount_t;


typedef struct _ym_sf2_generator_table {
    ym_sf2_gen_amount_t value[YM_SF2_GEN_EOF];
    bool set[YM_SF2_GEN_EOF];
} ym_sf2_gen_table_t;


typedef struct _ym_sf2_modulator {
    uint16_t src;
    uint16_t dest;
    int16_t amount;
    uint16_t amount_src;
    uint16_t trans;
} ym_sf2_mod_t;


typedef struct _ym_sf2_generator {
    uint16_t oper;
    int16_t amount;
} ym_sf2_gen_t;


typedef struct _ym_sf2_bag {
    uint16_t mod_index;
    uint16_t gen_index;
} ym_sf2_bag_t;


typedef struct _ym_sf2_zone {
    int mod_count;
    ym_sf2_mod_t *mods;
    int gen_count;
    ym_sf2_gen_t *gens;
    ym_sf2_gen_table_t gen_table;
} ym_sf2_zone_t;


typedef struct _ym_sf2_preset_header {
    char name[21];
    uint16_t preset;
    uint16_t bank;
    uint16_t bag_index;
    uint32_t library;       /* not used */
    uint32_t genre;         /* not used */
    uint32_t morphology;    /* not used */
    
    int zone_count;
    ym_sf2_zone_t *zones;
    bool has_global_zone;
} ym_sf2_phdr_t;


typedef struct _ym_sf2_instument_header {
    char name[21];
    uint16_t bag_index;
    
    int zone_count;
    ym_sf2_zone_t *zones;
    bool has_global_zone;
} ym_sf2_ihdr_t;


typedef struct _ym_sf2_sample_header {
    char name[21];
    uint32_t start;
    uint32_t end;
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t sample_rate;
    uint8_t original_pitch;
    int8_t pitch_correction;
    uint16_t sample_link; /* Not used */
    uint16_t sample_type; /* 1:mono 2:right 4:left 8:linked  0x8000:ROM */
} ym_sf2_shdr_t;



ym_inline void _free_zone(ym_sf2_zone_t *zones, int count) {
    int i;
    if (zones) {
        for (i = 0; i < count; i++) {
            ym_sf2_zone_t *zone = &zones[i];
            if (zone) {
                if (zone->gens) free(zone->gens);
                if (zone->mods) free(zone->mods);
            }
        }
        free(zones);
    }
}

ym_inline void _set_table(ym_sf2_gen_table_t *table, ym_sf2_zone_t *zone) {
    int i;
    for (i = 0; i < zone->gen_count; i++) {
        ym_sf2_gen_t *gen = &zone->gens[i];
        table->value[gen->oper].sword = gen->amount;
        table->set[gen->oper] = true;
    }
}

ym_inline void _merge_global_table(ym_sf2_gen_table_t *global, ym_sf2_gen_table_t *table) {
    int i;
    for (i = 0; i < YM_SF2_GEN_EOF; i++) {
        if (global->set[i] && !table->set[i]) {
            table->value[i] = global->value[i];
            table->set[i] = true;
        }
    }
}

ym_inline void _merge_table(ym_sf2_gen_table_t *p_table,
                            ym_sf2_gen_table_t *i_table,
                            ym_sf2_gen_table_t *table) {
    int oper;
    for (oper = 0; oper < YM_SF2_GEN_EOF; oper++) {
        ym_sf2_gen_def *item = &sf2_gen_def[oper];
        
        if (!p_table->set[oper] && !i_table->set[oper]) {
            table->value[oper].sword = item->def;
        } else if (!p_table->set[oper] && i_table->set[oper]) {
            table->value[oper] = i_table->value[oper];
            table->set[oper] = true;
        } else if (p_table->set[oper] && !i_table->set[oper]) {
            table->value[oper] = p_table->value[oper];
            table->set[oper] = true;
        } else {
            ym_sf2_gen_amount_t pv, iv;
            pv = p_table->value[oper];
            iv = i_table->value[oper];
            
            switch (item->policy) {
                case YM_SF2_GEN_MERGE_ADD:
                    table->value[oper].sword = pv.sword + iv.sword;
                    break;
                case YM_SF2_GEN_MERGE_OVERWRITE:
                    table->value[oper] = pv;
                    break;
                case YM_SF2_GEN_MERGE_FILL:
                    if (i_table->set[oper]) table->value[oper] = iv;
                    else table->value[oper] = pv;
                    break;
                case YM_SF2_GEN_MERGE_RANGE:
                    if (!i_table->set[oper]) {
                        table->value[oper] = pv;
                    } else {
                        table->value[oper].range.lo = YM_MAX(pv.range.lo, iv.range.lo);
                        table->value[oper].range.hi = YM_MIN(pv.range.hi, iv.range.hi);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

static bool _set_sample(ym_sf2_sample_ref sample,
                        ym_sf2_gen_table_t *table,
                        ym_sf2_shdr_t *     shdr) {
    { /// sample offset
        sample->start = (table->value[YM_SF2_GEN_StartAddrsCoarseOffset].sword << 15)
        + table->value[YM_SF2_GEN_StartAddrsOffset].sword
        + shdr->start;
        
        sample->length = (table->value[YM_SF2_GEN_EndAddrsCoarseOffset].sword << 15)
        + table->value[YM_SF2_GEN_EndAddrsOffset].sword
        + shdr->end - sample->start;
        
        sample->loop_start = (table->value[YM_SF2_GEN_StartloopAddrsCoarseOffset].sword << 15)
        + table->value[YM_SF2_GEN_StartloopAddrsOffset].sword
        + shdr->loop_start - sample->start;
        
        sample->loop_end = (table->value[YM_SF2_GEN_EndloopAddrsCoarseOffset].sword << 15)
        + table->value[YM_SF2_GEN_EndloopAddrsOffset].sword
        + shdr->loop_end - sample->start
        - 1;
        
        if (sample->length <= 0) return false;
        sample->loop_end = YM_CLAMP(sample->loop_end, 0, sample->length - 1);
        sample->loop_start = YM_CLAMP(sample->loop_start, 0, sample->loop_end);
    }
    
    { /// sample rate
        sample->sample_rate = shdr->sample_rate;
        sample->sample_rate = YM_CLAMP(sample->sample_rate, 400, 192000);
        sample->bit = YM_SF2_SAMPLE_BIT_16_INTEGER;
    }
    
    { /// Attenuation
        if (!table->set[YM_SF2_GEN_InitialAttenuation]) {
            sample->gain = 0;
        } else {
            sample->gain = table->value[YM_SF2_GEN_InitialAttenuation].sword / -10.0;
        }
    }
    
    { ///Panning
        sample->panning = table->value[YM_SF2_GEN_Pan].sword / 500.0f;
    }
    
    { ///envelope
        if (table->set[YM_SF2_GEN_DelayVolEnv] ||
            table->set[YM_SF2_GEN_AttackVolEnv] ||
            table->set[YM_SF2_GEN_HoldVolEnv] ||
            table->set[YM_SF2_GEN_DecayVolEnv] ||
            table->set[YM_SF2_GEN_SustainVolEnv] ||
            table->set[YM_SF2_GEN_ReleaseVolEnv]) {
            sample->vol_env.enabled = true;
        }
        sample->vol_env.delay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DelayVolEnv].sword / 100.0);
        sample->vol_env.attack = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_AttackVolEnv].sword / 100.0);
        sample->vol_env.hold = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_HoldVolEnv].sword / 100.0);
        sample->vol_env.decay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DecayVolEnv].sword / 100.0);
        sample->vol_env.sustain = table->value[YM_SF2_GEN_SustainVolEnv].sword / -10.0;
        sample->vol_env.release = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_ReleaseVolEnv].sword / 100.0);
        sample->vol_env.hold_ratio_per_key = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_KeynumToVolEnvHold].sword / 100.0);
        sample->vol_env.decay_ratio_per_key = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_KeynumToVolEnvDecay].sword / 100.0);
        
        if (table->set[YM_SF2_GEN_DelayModEnv] ||
            table->set[YM_SF2_GEN_AttackModEnv] ||
            table->set[YM_SF2_GEN_HoldModEnv] ||
            table->set[YM_SF2_GEN_DecayModEnv] ||
            table->set[YM_SF2_GEN_SustainModEnv] ||
            table->set[YM_SF2_GEN_ReleaseModEnv]) {
            sample->mod_env.enabled = true;
        }
        /// TODO 验证Sustain是否正确
        sample->mod_env.delay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DelayModEnv].sword / 100.0);
        sample->mod_env.attack = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_AttackModEnv].sword / 100.0);
        sample->mod_env.hold = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_HoldModEnv].sword / 100.0);
        sample->mod_env.decay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DecayModEnv].sword / 100.0);
        sample->mod_env.sustain = table->value[YM_SF2_GEN_SustainModEnv].sword / 1000;
        sample->mod_env.release = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_ReleaseModEnv].sword / 100.0);
        sample->mod_env.hold_ratio_per_key = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_KeynumToModEnvHold].sword / 100.0);
        sample->mod_env.decay_ratio_per_key = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_KeynumToModEnvDecay].sword / 100.0);
        
        sample->mod_env.mod_to_pitch = table->value[YM_SF2_GEN_ModEnvToPitch].sword / 100;
        sample->mod_env.mod_to_cutoff = table->value[YM_SF2_GEN_ModEnvToFilterFc].sword / 100;
    }
    
    { /// LFO
        if (table->set[YM_SF2_GEN_FreqModLFO]) {
            sample->mod_lfo.enabled = true;
        }
        sample->mod_lfo.delay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DelayModLFO].sword / 100.0);
        sample->mod_lfo.freq = ym_math_pitch_to_freq(table->value[YM_SF2_GEN_FreqModLFO].sword / 100.0);
        sample->mod_lfo.mod_to_volume = table->value[YM_SF2_GEN_ModLfoToVolume].sword / 10;
        sample->mod_lfo.mod_to_pitch = table->value[YM_SF2_GEN_ModLfoToPitch].sword / 100;
        sample->mod_lfo.mod_to_cutoff = table->value[YM_SF2_GEN_ModLfoToFilterFc].sword / 100;
        ///sample->mod_lfo.mod_to_cutoff = 30;
        
        if (table->set[YM_SF2_GEN_FreqVibLFO]) {
            sample->vib_lfo.enabled = true;
        }
        sample->vib_lfo.delay = ym_math_pitch_time_to_second(table->value[YM_SF2_GEN_DelayVibLFO].sword / 100.0);
        sample->vib_lfo.freq = ym_math_pitch_to_freq(table->value[YM_SF2_GEN_FreqVibLFO].sword / 100.0);
        sample->vib_lfo.mod_to_pitch = table->value[YM_SF2_GEN_VibLfoToPitch].sword / 100;
    }
    
    { /// Effects
        sample->effects.chorus = table->value[YM_SF2_GEN_ChorusEffectsSend].sword / 1000;
        sample->effects.reverb = table->value[YM_SF2_GEN_ReverbEffectsSend].sword / 1000;
    }
    
    { ///Loop模式
        int loop_mode = table->value[YM_SF2_GEN_SampleModes].uword & 0x03;
        switch (loop_mode) {
            case 0:
                sample->loop_mode = YM_SF2_SAMPLE_LOOP_NONE;
                break;
            case 1:
            case 2:
                sample->loop_mode = YM_SF2_SAMPLE_LOOP_CONTINUOUSLY;
                break;
            case 3:
                sample->loop_mode = YM_SF2_SAMPLE_LOOP_TO_END;
                break;
        }
    }
    
    { ///range
        sample->key_range.lo = table->value[YM_SF2_GEN_KeyRange].range.lo;
        sample->key_range.hi = table->value[YM_SF2_GEN_KeyRange].range.hi;
        sample->vel_range.lo = table->value[YM_SF2_GEN_VelRange].range.lo;
        sample->vel_range.hi = table->value[YM_SF2_GEN_VelRange].range.hi;
        if (table->set[YM_SF2_GEN_Keynum]) {
            sample->use_fixed_key = true;
            sample->fixed_key = table->value[YM_SF2_GEN_Keynum].uword;
        }
        if (table->set[YM_SF2_GEN_Velocity]) {
            sample->use_fixed_vel = true;
            sample->fixed_vel = table->value[YM_SF2_GEN_Velocity].uword;
        }
    }
    
    { ///SampleType
        if (shdr->sample_type & 0x8000) {  /* ignore rom type*/
            return false;
        } else if (shdr->sample_type & 0x01) {
            sample->type = YM_SF2_SAMPLE_TYPE_MONO;
        } else if (shdr->sample_type & 0x02) {
            sample->type = YM_SF2_SAMPLE_TYPE_LEFT;
        } else if (shdr->sample_type & 0x04) {
            sample->type = YM_SF2_SAMPLE_TYPE_RIGHT;
        } else if (shdr->sample_type & 0x08) { /* ignore link type*/
            return false;
        }
        sample->link_index = shdr->sample_link;
    }
    
    { ///cutoff
        if (table->set[YM_SF2_GEN_InitialFilterFc]) {
            sample->lowpass.enabled = true;
        }
        sample->lowpass.cutoff = table->value[YM_SF2_GEN_InitialFilterFc].uword / 100.0;
        sample->lowpass.Q = table->value[YM_SF2_GEN_InitialFilterQ].uword / 10.0f;
    }
    
    { /// exclusive
        sample->exclusive = table->value[YM_SF2_GEN_ExclusiveClass].uword != 0;
    }
    
    { ///key
        if (table->set[YM_SF2_GEN_OverridingRootKey]) {
            sample->root_key = table->value[YM_SF2_GEN_OverridingRootKey].sword;
        } else {
            sample->root_key = shdr->original_pitch;
        }
        sample->root_key -= shdr->pitch_correction / 100.0f;
        
        sample->turn.note = -table->value[YM_SF2_GEN_CoarseTune].sword;
        sample->turn.note -= table->value[YM_SF2_GEN_FineTune].sword / 100.0f;
        sample->turn.scale = table->value[YM_SF2_GEN_ScaleTuning].uword / 100.0f;
    }
    
    return true;
}

static ym_error_code _read_info_chunk(ym_sf2_ref sf2, int chunk_size) {
    uint32_t sub_chunk_id, sub_chunk_size;
    char *str;
    FILE *fd = sf2->fd;
    
    while (chunk_size > 0) {
        if (fread(&sub_chunk_id, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sub_chunk_size, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        chunk_size -= 8;
        
        /** SoundFont File Version */
        if (sub_chunk_id == ym_four_cc("ifil")) {
            if (sub_chunk_size != 4) return false;
            if (fread(&sf2->info.file_version.major, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
            if (fread(&sf2->info.file_version.minor, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        }
        /** Sound ROM Version (optional) */
        else if (sub_chunk_id == ym_four_cc("iver")) {
            if (sub_chunk_size != 4) return false;
            if (fread(&sf2->info.rom_version.major, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
            if (fread(&sf2->info.rom_version.minor, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        } else {
            str = calloc(1, sub_chunk_size + 1);
            if (str == NULL) return YM_ERROR_ALLOC_MEMORY;
            if (fread(str, sub_chunk_size, 1, fd) != 1) {
                free(str);
                return YM_ERROR_FILE_READ;
            }
            
            /** targer Sound Engine */
            if (sub_chunk_id == ym_four_cc("isng")) sf2->info.sound_engine = str;
            /** SoundFont Bank Name */
            else if (sub_chunk_id == ym_four_cc("INAM")) sf2->info.bank_name = str;
            /** Sound ROM Name                (optional) */
            else if (sub_chunk_id == ym_four_cc("irom")) sf2->info.rom_name = str;
            /** Create Date of Bank           (optional) */
            else if (sub_chunk_id == ym_four_cc("ICRD")) sf2->info.create_date = str;
            /** Sound Designers/Engineers     (optional) */
            else if (sub_chunk_id == ym_four_cc("IENG")) sf2->info.designer_name = str;
            /** Product for which the Bank was intended (optional) */
            else if (sub_chunk_id == ym_four_cc("IPRD")) sf2->info.intended_product = str;
            /** Copyright Message             (optional) */
            else if (sub_chunk_id == ym_four_cc("ICOP")) sf2->info.copyright = str;
            /** Comments on the Bank          (optional) */
            else if (sub_chunk_id == ym_four_cc("ICMT")) sf2->info.comment = str;
            /** Create Tools of SoundFont     (optional) */
            else if (sub_chunk_id == ym_four_cc("ISFT")) sf2->info.create_tools = str;
            else {
                free(str);
            }
        }
        chunk_size -= sub_chunk_size;
    }
    
    if (chunk_size != 0) return YM_ERROR_FILE_FORMAT_CONTENT;
    return YM_ERROR_NONE;
}

static ym_error_code _read_sdta_chunk(ym_sf2_ref sf2, int chunk_size) {
    uint32_t sub_chunk_id, sub_chunk_size;
    FILE *fd = sf2->fd;
    
    while (chunk_size > 0) {
        if (fread(&sub_chunk_id, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sub_chunk_size, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        chunk_size -= 8;
        
        /** Samples for the upper 16bits  (optional) */
        if (sub_chunk_id == ym_four_cc("smpl")) {
            sf2->hidden.sample_offset = (uint32_t)ftell(fd);
            sf2->hidden.sample_size = sub_chunk_size;
        }
        /** Samples for the lower 8bits   (optional) */
        else if (sub_chunk_id == ym_four_cc("sm24")) {
            sf2->hidden.sample24_offset = (uint32_t)ftell(fd);
            sf2->hidden.sample24_size = sub_chunk_size;
        } else {
            /** unknown SDTA chunk, ignore */
        }
        
        if (fseek(fd, sub_chunk_size, SEEK_CUR) != 0) return YM_ERROR_FILE_READ;
        chunk_size -= sub_chunk_size;
    }
    
    if (sf2->hidden.sample24_size != (sf2->hidden.sample_size + 1) / 2) {
        sf2->hidden.sample24_size = 0;
        sf2->hidden.sample24_offset = 0;
    }
    
    if (chunk_size != 0) return YM_ERROR_FILE_FORMAT_CONTENT;
    return YM_ERROR_NONE;
}

static ym_error_code _process_phdr(FILE *fd, int size, ym_sf2_phdr_t **presets, int *preset_count) {
    int i;
    if (size <= 0 || size % 38) return YM_ERROR_FILE_FORMAT_CONTENT;
    *preset_count = size / 38;
    *presets = calloc(*preset_count, sizeof(ym_sf2_phdr_t));
    if (*presets == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *preset_count; i++) {
        ym_sf2_phdr_t *preset = *presets + i;
        if (fread(&preset->name, 20, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->preset, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->bank, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->bag_index, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->library, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->genre, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&preset->morphology, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
    }
    return YM_ERROR_NONE;
}

static ym_error_code _process_ihdr(FILE *fd, int size, ym_sf2_ihdr_t **ihdrs, int *ihdr_count) {
    int i;
    if (size <= 0 || size % 22) return YM_ERROR_FILE_FORMAT_CONTENT;
    *ihdr_count = size / 22;
    *ihdrs = calloc(*ihdr_count, sizeof(ym_sf2_ihdr_t));
    if (*ihdrs == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *ihdr_count; i++) {
        ym_sf2_ihdr_t *inst = *ihdrs + i;
        if (fread(&inst->name, 20, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&inst->bag_index, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
    }
    return YM_ERROR_NONE;
}

static ym_error_code _process_shdr(FILE *fd, int size, ym_sf2_shdr_t **samples, int *sample_count) {
    int i;
    if (size <= 0 || size % 46) return YM_ERROR_FILE_FORMAT_CONTENT;
    *sample_count = size / 46;
    *samples = calloc(*sample_count, sizeof(ym_sf2_shdr_t));
    if (*samples == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *sample_count; i++) {
        ym_sf2_shdr_t *sample = *samples + i;
        if (fread(&sample->name, 20, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->start, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->end, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->loop_start, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->loop_end, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->sample_rate, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->original_pitch, 1, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->pitch_correction, 1, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->sample_link, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&sample->sample_type, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (sample->original_pitch > 127) sample->original_pitch = 60;
    }
    return YM_ERROR_NONE;
}

static ym_error_code _process_bag(FILE *fd, int size, ym_sf2_bag_t **bags, int *bag_count) {
    int i;
    if (size <= 0 || size % 4) return YM_ERROR_FILE_FORMAT_CONTENT;
    *bag_count = size / 4;
    *bags = calloc(*bag_count, sizeof(ym_sf2_bag_t));
    if (*bags == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *bag_count; i++) {
        ym_sf2_bag_t *bag = *bags + i;
        if (fread(&bag->gen_index, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&bag->mod_index, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
    }
    return YM_ERROR_NONE;
}

static ym_error_code _process_mod(FILE *fd, int size, ym_sf2_mod_t **mods, int *mod_count) {
    int i;
    if (size <= 0 || size % 10) return YM_ERROR_FILE_FORMAT_CONTENT;
    *mod_count = size / 10;
    *mods = calloc(*mod_count, sizeof(ym_sf2_mod_t));
    if (*mods == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *mod_count; i++) {
        ym_sf2_mod_t *mod = *mods + i;
        if (fread(&mod->src, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&mod->dest, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&mod->amount, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&mod->amount_src, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&mod->trans, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
    }
    return YM_ERROR_NONE;
}

static ym_error_code _process_gen(FILE *fd, int size, ym_sf2_gen_t **gens, int *gen_count) {
    int i;
    if (size <= 0 || size % 4) return YM_ERROR_FILE_FORMAT_CONTENT;
    *gen_count = size / 4;
    *gens = calloc(*gen_count, sizeof(ym_sf2_gen_t));
    if (*gens == NULL) return YM_ERROR_ALLOC_MEMORY;
    for (i = 0; i < *gen_count; i++) {
        ym_sf2_gen_t *gen = *gens + i;
        if (fread(&gen->oper, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&gen->amount, 2, 1, fd) != 1) return YM_ERROR_FILE_READ;
    }
    return YM_ERROR_NONE;
}

#define pdta_err(err) do { error = (err); goto cleanup; } while (0)

static ym_error_code _read_pdta_chunk(ym_sf2_ref sf2, int chunk_size) {
    FILE *fd;
    ym_error_code error;
    uint32_t sub_chunk_id, sub_chunk_size;
    int phdr_count, pbag_count, pmod_count, pgen_count;
    int ihdr_count, ibag_count, imod_count, igen_count;
    int shdr_count;
    ym_sf2_phdr_t *phdrs;
    ym_sf2_ihdr_t *ihdrs;
    ym_sf2_shdr_t *shdrs;
    ym_sf2_bag_t *pbags, *ibags;
    ym_sf2_mod_t *pmods, *imods;
    ym_sf2_gen_t *pgens, *igens;
    int p, i, b, m, g, z, iz, pz;
    
    fd = sf2->fd;
    phdr_count = pbag_count = pmod_count = pgen_count = 0;
    ihdr_count = ibag_count = imod_count = igen_count = 0;
    shdr_count = 0;
    phdrs = NULL;
    ihdrs = NULL;
    shdrs = NULL;
    pbags = ibags = NULL;
    pmods = imods = NULL;
    pgens = igens = NULL;
    error = YM_ERROR_NONE;
    
    /******************************** read ************************************/
    while (chunk_size > 0) {
        if (fread(&sub_chunk_id, 4, 1, fd) != 1) error = YM_ERROR_FILE_READ;
        if (fread(&sub_chunk_size, 4, 1, fd) != 1) error = YM_ERROR_FILE_READ;
        if (error != YM_ERROR_NONE) goto cleanup;
        chunk_size -= 8;
        if (sub_chunk_size == 0) continue;
        
        if (sub_chunk_id == ym_four_cc("phdr")) {
            error = _process_phdr(fd, sub_chunk_size, &phdrs, &phdr_count);
        } else if (sub_chunk_id == ym_four_cc("pbag")) {
            error = _process_bag(fd, sub_chunk_size, &pbags, &pbag_count);
        } else if (sub_chunk_id == ym_four_cc("pmod")) {
            error = _process_mod(fd, sub_chunk_size, &pmods, &pmod_count);
        } else if (sub_chunk_id == ym_four_cc("pgen")) {
            error = _process_gen(fd, sub_chunk_size, &pgens, &pgen_count);
        } else if (sub_chunk_id == ym_four_cc("inst")) {
            error = _process_ihdr(fd, sub_chunk_size, &ihdrs, &ihdr_count);
        } else if (sub_chunk_id == ym_four_cc("ibag")) {
            error = _process_bag(fd, sub_chunk_size, &ibags, &ibag_count);
        } else if (sub_chunk_id == ym_four_cc("imod")) {
            error = _process_mod(fd, sub_chunk_size, &imods, &imod_count);
        } else if (sub_chunk_id == ym_four_cc("igen")) {
            error = _process_gen(fd, sub_chunk_size, &igens, &igen_count);
        } else if (sub_chunk_id == ym_four_cc("shdr")) {
            error = _process_shdr(fd, sub_chunk_size, &shdrs, &shdr_count);
        } else {
            if (fseek(fd, sub_chunk_size, SEEK_CUR) != 0) error = YM_ERROR_FILE_READ;
        }
        
        if (error != YM_ERROR_NONE) goto cleanup;
        chunk_size -= sub_chunk_size;
    }
    if (chunk_size != 0) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    
    
    /******************************** validate ********************************/
    if (phdr_count == 0 || pbag_count == 0 || pmod_count == 0
        || pgen_count == 0 || ihdr_count == 0 || ibag_count == 0
        || imod_count == 0 || igen_count == 0 || shdr_count == 0) {
        pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    }
    for (i = 0; i < phdr_count - 1; i++) {
        if (phdrs[i].bag_index >= phdrs[i + 1].bag_index) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    }
    if (phdr_count && phdrs[phdr_count - 1].bag_index > pbag_count) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    for (i = 0; i < ihdr_count - 1; i++) {
        if (ihdrs[i].bag_index >= ihdrs[i + 1].bag_index) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    }
    if (ihdr_count && ihdrs[ihdr_count - 1].bag_index > ibag_count) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
    
    
    
    /******************************** set zone ********************************/
    for (p = 0; p < phdr_count - 1; p++) {
        ym_sf2_phdr_t *preset = &phdrs[p];
        ym_sf2_phdr_t *preset_next = &phdrs[p + 1];
        preset->zone_count = preset_next->bag_index - preset->bag_index;
        if (preset->zone_count == 0) continue;
        if (!(preset->zones = calloc(preset->zone_count, sizeof(ym_sf2_zone_t)))) {
            pdta_err(YM_ERROR_ALLOC_MEMORY);
        }
        
        for (b = preset->bag_index; b < preset_next->bag_index; b++) {
            ym_sf2_bag_t *bag = &pbags[b];
            ym_sf2_bag_t *bag_next = &pbags[b + 1];
            ym_sf2_zone_t *zone = &preset->zones[b - preset->bag_index];
            zone->mod_count = bag_next->mod_index - bag->mod_index;
            zone->gen_count = bag_next->gen_index - bag->gen_index;
            
            if (zone->mod_count) {
                if (!(zone->mods = calloc(zone->mod_count, sizeof(ym_sf2_mod_t)))) {
                    pdta_err(YM_ERROR_ALLOC_MEMORY);
                }
                for (m = 0; m < zone->mod_count; m++) {
                    zone->mods[m] = pmods[bag->mod_index + m];
                }
            }
            if (zone->gen_count) {
                if (!(zone->gens = calloc(zone->gen_count, sizeof(ym_sf2_gen_t)))) {
                    pdta_err(YM_ERROR_ALLOC_MEMORY);
                }
                for (g = 0; g < zone->gen_count; g++) {
                    zone->gens[g] = pgens[bag->gen_index + g];
                }
            }
        }
    }
    
    for (i = 0; i < ihdr_count - 1; i++) {
        ym_sf2_ihdr_t *inst = &ihdrs[i];
        ym_sf2_ihdr_t *inst_next = &ihdrs[i + 1];
        inst->zone_count = inst_next->bag_index - inst->bag_index;
        if (inst->zone_count == 0) continue;
        if (!(inst->zones = calloc(inst->zone_count, sizeof(ym_sf2_zone_t)))) {
            pdta_err(YM_ERROR_ALLOC_MEMORY);
        }
        
        for (b = inst->bag_index; b < inst_next->bag_index; b++) {
            ym_sf2_bag_t *bag = &ibags[b];
            ym_sf2_bag_t *bag_next = &ibags[b + 1];
            ym_sf2_zone_t *zone = &inst->zones[b - inst->bag_index];
            zone->mod_count = bag_next->mod_index - bag->mod_index;
            zone->gen_count = bag_next->gen_index - bag->gen_index;
            
            if (zone->mod_count) {
                if (!(zone->mods = calloc(zone->mod_count, sizeof(ym_sf2_mod_t)))) {
                    pdta_err(YM_ERROR_ALLOC_MEMORY);
                }
                for (m = 0; m < zone->mod_count; m++) {
                    zone->mods[m] = imods[bag->mod_index + m];
                }
            }
            if (zone->gen_count) {
                if (!(zone->gens = calloc(zone->gen_count, sizeof(ym_sf2_gen_t)))) {
                    pdta_err(YM_ERROR_ALLOC_MEMORY);
                }
                for (g = 0; g < zone->gen_count; g++) {
                    zone->gens[g] = igens[bag->gen_index + g];
                }
            }
        }
    }
    
    
    /******************************** merge global zone ***********************/
    for (p = 0; p < phdr_count - 1; p++) {
        ym_sf2_phdr_t *phdr = &phdrs[p];
        if (phdr->zone_count == 0) continue;
        for (z = 0; z < phdr->zone_count; z++) {
            _set_table(&phdr->zones[z].gen_table, &phdr->zones[z]);
        }
        if (!phdr->zones[0].gen_table.set[YM_SF2_GEN_Instrument]) {
            phdr->has_global_zone = true;
            for (z = 1; z < phdr->zone_count; z++) {
                _merge_global_table(&phdr->zones[0].gen_table, &phdr->zones[z].gen_table);
            }
        }
    }
    
    for (i = 0; i < ihdr_count - 1; i++) {
        ym_sf2_ihdr_t *ihdr = &ihdrs[i];
        if (ihdr->zone_count == 0) continue;
        for (z = 0; z < ihdr->zone_count; z++) {
            _set_table(&ihdr->zones[z].gen_table, &ihdr->zones[z]);
        }
        if (!ihdr->zones[0].gen_table.set[YM_SF2_GEN_Sample]) {
            ihdr->has_global_zone = true;
            for (z = 1; z < ihdr->zone_count; z++) {
                _merge_global_table(&ihdr->zones[0].gen_table, &ihdr->zones[z].gen_table);
            }
        }
    }
    
    /******************************** merge preset to inst ********************/
    for (p = 0; p < phdr_count - 1; p++) {
        ym_sf2_phdr_t *phdr = &phdrs[p];
        void *key = _ym_sf2_preset_key(phdr->bank, phdr->preset);
        if (ym_map_contains_key(sf2->presets, key)) continue;
        
        ym_sf2_preset_ref preset = ym_sf2_preset_create(phdr->name, phdr->bank, phdr->preset);
        if (preset == NULL) pdta_err(YM_ERROR_ALLOC_MEMORY);
        ym_map_set(sf2->presets, key, preset);
        ym_release(preset);
        
        for (pz = phdr->has_global_zone ? 1 : 0; pz < phdr->zone_count; pz++) {
            ym_sf2_zone_t *p_zone = &phdr->zones[pz];
            uint16_t ihdr_index = p_zone->gen_table.value[YM_SF2_GEN_Instrument].uword;
            if (ihdr_index >= ihdr_count) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
            ym_sf2_ihdr_t *ihdr = &ihdrs[ihdr_index];
            
            for (iz = ihdr->has_global_zone ? 1 : 0; iz < ihdr->zone_count; iz++) {
                ym_sf2_gen_table_t table;
                memset(&table, 0, sizeof(ym_sf2_gen_table_t));
                
                ym_sf2_zone_t *i_zone = &ihdr->zones[iz];
                _merge_table(&p_zone->gen_table, &i_zone->gen_table, &table);
                if (table.value[YM_SF2_GEN_KeyRange].range.lo > table.value[YM_SF2_GEN_KeyRange].range.hi
                    || table.value[YM_SF2_GEN_VelRange].range.lo > table.value[YM_SF2_GEN_VelRange].range.hi) {
                    continue;
                }
                uint16_t shdr_index = table.value[YM_SF2_GEN_Sample].uword;
                if (shdr_index >= shdr_count) pdta_err(YM_ERROR_FILE_FORMAT_CONTENT);
                ym_sf2_shdr_t *shdr = &shdrs[shdr_index];
                
                ym_sf2_sample_ref sample = ym_sf2_sample_create(shdr->name);
                if (sample == NULL) pdta_err(YM_ERROR_ALLOC_MEMORY);
                if (_set_sample(sample, &table, shdr)) {
                    ym_array_append(preset->samples, sample);
                }
                ym_release(sample);
            }
        }
    }
    
    
cleanup:
    if (phdrs) {
        for (p = 0; p < phdr_count; p++) {
            _free_zone(phdrs[p].zones, phdrs[p].zone_count);
        }
        free(phdrs);
    }
    if (ihdrs) {
        for (i = 0; i < ihdr_count; i++) {
            _free_zone(ihdrs[i].zones, ihdrs[i].zone_count);
        }
        free(ihdrs);
    }
    if (shdrs) free(shdrs);
    if (pbags) free(pbags);
    if (pmods) free(pmods);
    if (pgens) free(pgens);
    if (ibags) free(ibags);
    if (imods) free(imods);
    if (igens) free(igens);
    return error;
}

ym_error_code _ym_sf2_read(ym_sf2_ref sf2) {
    uint32_t chunk_id, chunk_size, chunk_type;
    long file_size, i;
    ym_error_code error;
    FILE *fd = sf2->fd;
    
    if (fseek(fd, 0, SEEK_END) != 0) return YM_ERROR_FILE_READ;
    file_size = ftell(fd);
    if (fseek(fd, 0, SEEK_SET) != 0) return YM_ERROR_FILE_READ;
    
    if (fread(&chunk_id, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
    if (fread(&chunk_size, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
    if (fread(&chunk_type, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
    
    if (chunk_id != ym_four_cc("RIFF")) return YM_ERROR_FILE_FORMAT;
    if (chunk_size != file_size - 8) return YM_ERROR_FILE_FORMAT_CONTENT;
    if (chunk_type != ym_four_cc("sfbk")) return YM_ERROR_FILE_FORMAT;
    
    
    static char *id_name[3] = { "INFO", "sdta", "pdta" };
    static ym_error_code(*read_chunk_func[3]) (ym_sf2_ref, int) = {
        _read_info_chunk,
        _read_sdta_chunk,
        _read_pdta_chunk
    };
    
    for (i = 0; i < 3; i++) {
        if (fread(&chunk_id, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&chunk_size, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (fread(&chunk_type, 4, 1, fd) != 1) return YM_ERROR_FILE_READ;
        if (chunk_id != ym_four_cc("LIST")) return YM_ERROR_FILE_FORMAT_CONTENT;
        if (chunk_size + ftell(fd) - 4 > file_size || chunk_size <= 4) return YM_ERROR_FILE_FORMAT_CONTENT;
        if (chunk_type != ym_four_cc(id_name[i])) return YM_ERROR_FILE_FORMAT_CONTENT;
        error = read_chunk_func[i](sf2, chunk_size - 4);
        if (error != YM_ERROR_NONE) return error;
    }
    
    return YM_ERROR_NONE;
}

static void _ym_sf2_dealloc(ym_sf2_ref sf2) {
    if (sf2->path) free(sf2->path);
    if (sf2->fd) fclose(sf2->fd);
    if (sf2->info.sound_engine) free(sf2->info.sound_engine);
    if (sf2->info.bank_name) free(sf2->info.bank_name);
    if (sf2->info.rom_name) free(sf2->info.rom_name);
    if (sf2->info.create_date) free(sf2->info.create_date);
    if (sf2->info.designer_name) free(sf2->info.designer_name);
    if (sf2->info.intended_product) free(sf2->info.intended_product);
    if (sf2->info.copyright) free(sf2->info.copyright);
    if (sf2->info.comment) free(sf2->info.comment);
    if (sf2->info.create_tools) free(sf2->info.create_tools);
    ym_release(sf2->presets);
    ym_release(sf2->hidden.cache);
}

#define sf2_err(err) do { \
ym_release(sf2); \
if (error) *error = err; \
return NULL; \
} while (0)


ym_sf2_ref ym_sf2_create_from_path(const char *path, ym_error_code *error) {
    ym_sf2_ref sf2;
    FILE *fd;
    ym_error_code read_error;
    
    if (error) *error = YM_ERROR_NONE;
    sf2 = ym_alloc(struct _ym_sf2, _ym_sf2_dealloc);
    if (sf2 == NULL) sf2_err(YM_ERROR_ALLOC_MEMORY);
    
    if (path == NULL) sf2_err(YM_ERROR_INVALID_PARAMS);
    fd = fopen(path, "rb");
    if (fd == NULL) sf2_err(YM_ERROR_FILE_OPEN);
    sf2->path = strdup(path);
    sf2->fd = fd;
    
    sf2->presets = ym_map_create_with_options(0, NULL, &ym_map_object_value_callback);
    if (sf2->presets == NULL) sf2_err(YM_ERROR_ALLOC_MEMORY);
    read_error = _ym_sf2_read(sf2);
    if (read_error != YM_ERROR_NONE) sf2_err(read_error);
    return sf2;
}

/* ************************************************************************** */


ym_sf2_preset_ref ym_sf2_get_preset(ym_sf2_ref sf2, uint16_t bank, uint16_t patch) {
    void *key;
    
    if (sf2 == NULL || sf2->presets == NULL) return NULL;
    key = _ym_sf2_preset_key(bank, patch);
    return (ym_sf2_preset_ref)ym_map_get(sf2->presets, key);
}

ym_error_code _load_one_sample(ym_sf2_ref sf2, ym_sf2_sample_ref sample) {
    ym_data_ref data;
    void *buffer;
    char key[32];
    long offset;
    const int padding = 10; /* add some padding for synthesizer */
    
    /************************ try load from cache *****************************/
    if (sample->data) return YM_ERROR_NONE;
    sprintf(key, "%x,%x", sample->start, sample->length);
    data = (ym_data_ref)ym_map_get(sf2->hidden.cache, key);
    if (data) {
        sample->data = (ym_data_ref)ym_retain(data);
        return YM_ERROR_NONE;
    }
    
    /********************* cache miss, load from file *************************/
    offset = sf2->hidden.sample_offset + sample->start * sizeof(int16_t);
    if (fseek(sf2->fd, offset, SEEK_SET) != 0) return YM_ERROR_FILE_READ;
    buffer = malloc((sample->length + padding) * sizeof(int16_t));
    if (buffer == NULL) return YM_ERROR_ALLOC_MEMORY;
    if (fread(buffer, sizeof(int16_t), sample->length, sf2->fd) != sample->length) {
        free(buffer);
        return YM_ERROR_FILE_READ;
    }
    memset(((uint16_t *)buffer) + padding, 0, padding * sizeof(int16_t));
    data = ym_data_create_from_mem(buffer, (sample->length + padding) * sizeof(int16_t), true);
    if (data == NULL) {
        free(buffer);
        return YM_ERROR_FILE_READ;
    }
    
    /************************** add to cache **********************************/
    sample->data = data;
    ym_map_set(sf2->hidden.cache, key, data);
    return YM_ERROR_NONE;
}

void _unload_one_sample(ym_sf2_ref sf2, ym_sf2_sample_ref sample) {
    ym_data_ref data;
    char key[32];
    
    if (sample->data) {
        ym_release(sample->data);
        sample->data = NULL;
    }
    
    /********** if not used by any sample, then remove from cache *************/
    sprintf(key, "%x,%x", sample->start, sample->length);
    data = (ym_data_ref)ym_map_get(sf2->hidden.cache, key);
    if (data && ym_retain_count(data) == 1) {
        ym_map_remove(sf2->hidden.cache, key);
    }
}

ym_error_code ym_sf2_load_sample_data(ym_sf2_ref sf2, uint16_t bank, uint16_t patch) {
    ym_sf2_preset_ref preset;
    ym_sf2_sample_ref sample;
    ym_error_code error;
    void *key;
    int i;
    
    if (sf2 == NULL || sf2->presets == NULL) return YM_ERROR_INVALID_PARAMS;
    key = _ym_sf2_preset_key(bank, patch);
    preset = (ym_sf2_preset_ref)ym_map_get(sf2->presets, key);
    if (preset == NULL) return YM_ERROR_INVALID_PARAMS;
    
    if (sf2->hidden.cache == NULL)
        sf2->hidden.cache = ym_map_create_with_options(0,
                                                       &ym_map_string_key_callback,
                                                       &ym_map_object_value_callback);
    
    if (sf2->hidden.cache == NULL) return YM_ERROR_ALLOC_MEMORY;
    
    for (i = 0; i < ym_array_count(preset->samples); i++) {
        sample = (ym_sf2_sample_ref)ym_array_get(preset->samples, i);
        error = _load_one_sample(sf2, sample);
        if (error != YM_ERROR_NONE) return error;
    }
    
    return YM_ERROR_NONE;
}

ym_error_code ym_sf2_load_all_sample_data(ym_sf2_ref sf2) {
    ym_array_ref keys;
    ym_sf2_preset_ref preset;
    ym_error_code error;
    int i;
    
    if (sf2 == NULL || sf2->presets == NULL) return YM_ERROR_INVALID_PARAMS;
    keys = ym_map_create_key_array(sf2->presets);
    if (keys == NULL) return YM_ERROR_ALLOC_MEMORY;
    
    for (i = 0; i < ym_array_count(keys); i++) {
        preset = (ym_sf2_preset_ref)ym_map_get(sf2->presets, ym_array_get(keys, i));
        error = ym_sf2_load_sample_data(sf2, preset->bank, preset->patch);
        if (error != YM_ERROR_NONE) {
            ym_release(keys);
            return error;
        }
    }
    ym_release(keys);
    return YM_ERROR_NONE;
}

ym_error_code ym_sf2_unload_sample_data(ym_sf2_ref sf2, uint16_t bank, uint16_t patch) {
    ym_sf2_preset_ref preset;
    ym_sf2_sample_ref sample;
    void *key;
    int i;
    
    if (sf2 == NULL || sf2->presets == NULL) return YM_ERROR_INVALID_PARAMS;
    key = _ym_sf2_preset_key(bank, patch);
    preset = (ym_sf2_preset_ref)ym_map_get(sf2->presets, key);
    if (preset == NULL) return YM_ERROR_INVALID_PARAMS;
    if (sf2->hidden.cache == NULL) return YM_ERROR_NONE;
    
    for (i = 0; i < ym_array_count(preset->samples); i++) {
        sample = (ym_sf2_sample_ref)ym_array_get(preset->samples, i);
        if (sample->data) {
            _unload_one_sample(sf2, sample);
        }
    }
    return YM_ERROR_NONE;
}

ym_error_code ym_sf2_unload_all_sample_data(ym_sf2_ref sf2) {
    ym_array_ref keys;
    ym_sf2_preset_ref preset;
    ym_error_code error;
    int i;
    
    if (sf2 == NULL || sf2->presets == NULL) return YM_ERROR_INVALID_PARAMS;
    keys = ym_map_create_key_array(sf2->presets);
    if (keys == NULL) return YM_ERROR_ALLOC_MEMORY;
    
    for (i = 0; i < ym_array_count(keys); i++) {
        preset = (ym_sf2_preset_ref)ym_map_get(sf2->presets, ym_array_get(keys, i));
        error = ym_sf2_unload_sample_data(sf2, preset->bank, preset->patch);
        if (error != YM_ERROR_NONE) {
            ym_release(keys);
            return error;
        }
    }
    ym_release(keys);
    return YM_ERROR_NONE;
}

bool ym_sf2_is_sample_data_loaded(ym_sf2_preset_ref preset) {
    ym_sf2_sample_ref sample;
    int i;
    
    if (preset == NULL || preset->samples == NULL) return false;
    for (i = 0; i < ym_array_count(preset->samples); i++) {
        sample = (ym_sf2_sample_ref)ym_array_get(preset->samples, i);
        if (sample->data == NULL) return false;
    }
    return true;
}
