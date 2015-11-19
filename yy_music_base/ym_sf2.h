//
//  ym_sf2.h
//  yy_music
//
//  Created by ibireme on 14-3-2.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

/**
 Soundfont2 (*.sf2 file)
 */

#ifndef __YM_SF2__
#define __YM_SF2__

#include "ym_base.h"
#include "ym_array.h"
#include "ym_map.h"
#include "ym_data.h"

YM_EXTERN_C_BEGIN

typedef enum {
    YM_SF2_SAMPLE_LOOP_NONE         = 0,
    YM_SF2_SAMPLE_LOOP_CONTINUOUSLY = 1, /**< continue loop after release */
    YM_SF2_SAMPLE_LOOP_TO_END       = 2, /**< play to the end after release */
} ym_sf2_sample_loop_mode;

typedef enum {
    YM_SF2_SAMPLE_TYPE_MONO,
    YM_SF2_SAMPLE_TYPE_LEFT,
    YM_SF2_SAMPLE_TYPE_RIGHT,
} ym_sf2_sample_type;

typedef enum {
    YM_SF2_SAMPLE_BIT_16_INTEGER,
    YM_SF2_SAMPLE_BIT_32_INTEGER,
    YM_SF2_SAMPLE_BIT_32_FLOAT,
    YM_SF2_SAMPLE_BIT_64_FLOAT,
} ym_sf2_sample_bit;


typedef struct _ym_sf2 *ym_sf2_ref;
typedef struct _ym_sf2_preset *ym_sf2_preset_ref;
typedef struct _ym_sf2_sample *ym_sf2_sample_ref;

ym_sf2_ref ym_sf2_create_from_path(const char *path, ym_error_code *error);
ym_sf2_preset_ref ym_sf2_get_preset(ym_sf2_ref sf2, uint16_t bank, uint16_t patch);

ym_error_code ym_sf2_load_sample_data(ym_sf2_ref sf2, uint16_t bank, uint16_t patch);
ym_error_code ym_sf2_unload_sample_data(ym_sf2_ref sf2, uint16_t bank, uint16_t patch);
ym_error_code ym_sf2_load_all_sample_data(ym_sf2_ref sf2);
ym_error_code ym_sf2_unload_all_sample_data(ym_sf2_ref sf2);

bool ym_sf2_is_sample_data_loaded(ym_sf2_preset_ref preset);



struct _ym_sf2 {
    char *path;
    FILE *fd;
    ym_map_ref presets;
    
    struct {
        struct {
            uint16_t major;
            uint16_t minor;
        } file_version;
        struct {
            uint16_t major;
            uint16_t minor;
        } rom_version;
        char *sound_engine;
        char *bank_name;
        char *rom_name;
        char *create_date;
        char *designer_name;
        char *intended_product;
        char *copyright;
        char *comment;
        char *create_tools;
    } info;
    
    struct {
        uint32_t sample_offset;
        uint32_t sample_size;
        uint32_t sample24_offset;
        uint32_t sample24_size;
        ym_map_ref cache;
    } hidden;
};


struct _ym_sf2_preset {
    const char *name;
    uint16_t bank;
    uint16_t patch;
    ym_array_ref samples;
};

struct _ym_sf2_sample {
    uint16_t index;
    const char *name;
    
    int32_t sample_rate;
    ym_sf2_sample_bit bit;
    ym_data_ref data;
    int32_t start;
    int32_t length;
    int32_t loop_start; /**< loop start frame index, 0 ~ length-1 */
    int32_t loop_end;   /**< loop end frame index, 0 ~ length-1 */
    ym_sf2_sample_loop_mode loop_mode;
    ym_sf2_sample_type type;
    uint16_t link_index;
    
    float gain; /** gain in dB, -144.0~144.0, negative for attenuation,position for gain */
    //float volume;  /**< 0%~100% (0.0~1.0) */
    float panning; /**< full_left:-1.0  full_right:1.0  center:0.0 */
    
    struct {
        uint8_t lo;
        uint8_t hi;
    } key_range;
    struct {
        uint8_t lo;
        uint8_t hi;
    } vel_range;
    
    bool use_fixed_key;
    uint8_t fixed_key;
    bool use_fixed_vel;
    uint8_t fixed_vel;
    
    
    
    float root_key;  /**< semitone */
    struct {
        float note;  /**< semitone default:0.0 , add to root_key */
        float scale; /**< semitone 0.0~12.0 default:1.0 */
    } turn;
    
    
    struct {
        bool enabled;
        float delay;       /**< second */
        float attack;      /**< second */
        float hold;        /**< second */
        float decay;       /**< second */
        float sustain;     /**< decibel -144.0~0.0 dB, default:0.0dB */
        float release;     /**< second */
        float hold_ratio_per_key;    /**< ratio_per_key (0.5~2.0), default:1.0 */
        float decay_ratio_per_key;   /**< ratio_per_key (0.5~2.0), default:1.0 */
    } vol_env; /**< volume envelope */
    
    
    struct {
        bool enabled;
        float delay;   /**< second */
        float attack;  /**< second */
        float hold;    /**< second */
        float decay;   /**< second */
        float sustain; /**< 0%~100% (0.0~1.0), default:0.0 */
        float release; /**< second */
        float hold_ratio_per_key;    /**< ratio_per_key (0.5~2.0), default:1.0 */
        float decay_ratio_per_key;   /**< ratio_per_key (0.5~2.0), default:1.0 */
        
        float mod_to_pitch;   /**< in pitch (semitone) */
        float mod_to_cutoff;  /**< in pitch (semitone) (-120.0~120.0)*/
    } mod_env; /**< modulation envelope */
    
    struct {
        bool enabled;
        float delay;      /**< second */
        float freq;    /**< Hz */
        
        float mod_to_volume; /**< in dB gain  */
        float mod_to_pitch;  /**< in pitch (semitone) */
        float mod_to_cutoff;     /**< in pitch (semitone) */
    } mod_lfo; /**< tremolo */
    
    struct {
        bool enabled;
        float delay;      /**< second */
        float freq;    /**< Hz */
        float mod_to_pitch;  /**< semitone */
    } vib_lfo; /**< vibrato*/
    
    struct {
        bool enabled;
        float cutoff; /**< in pitch */
        float Q;         /**< in dB (0~96.0) */
    } lowpass;
    
    struct {
        float chorus;  /**< 0.0~1.0 */
        float reverb;  /**< 0.0~1.0 */
    } effects;
    
    bool exclusive;
};

YM_EXTERN_C_END
#endif
