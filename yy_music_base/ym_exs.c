//
//  ym_exs.c
//  yy_music
//
//  Created by ibireme on 14-7-1.
//  Copyright (c) 2014 ibireme. All rights reserved.
//

#include "ym_exs.h"
#include "ym_util.h"


static void _ym_exs_dealloc(ym_exs_ref exs) {
    ym_release(exs->header);
    ym_release(exs->param);
    ym_release(exs->zones);
    ym_release(exs->groups);
    ym_release(exs->samples);
}

static ym_exs_header_ref _ym_exs_header_create(char *data, uint32_t size, bool is_big_endian) {
    ym_exs_header_ref header;
    
    header = ym_alloc(struct _ym_exs_header, NULL);
    if (header == NULL) return NULL;
    header->id = *((uint32_t *)&data[8]);
    if (is_big_endian) header->id = ym_swap_endian_uint32(header->id);
    memcpy(header->name, &data[20], 64);
    return header;
}

static ym_exs_zone_ref _ym_exs_zone_create(char *data, uint32_t size, bool is_big_endian) {
    ym_exs_zone_ref zone;
    
    zone = ym_alloc(struct _ym_exs_zone, NULL);
    if (zone == NULL) return NULL;
    zone->id = *((uint32_t *)&data[8]);
    if (is_big_endian) zone->id = ym_swap_endian_uint32(zone->id);
    memcpy(zone->name, &data[20], 64);
    
    zone->group_index = *((int32_t *)&data[172]);
    zone->sample_index = *((int32_t *)&data[176]);
    
    zone->key = *((uint8_t *)&data[85]);
    zone->coarse_tuning = *((int8_t *)&data[164]);
    zone->fine_tuning = *((int8_t *)&data[86]);
    
    zone->volume = *((int8_t *)&data[88]);
    zone->pan = *((int8_t *)&data[87]);
    zone->scale = *((int8_t *)&data[89]);
    if (((*((uint8_t *)&data[84])) & (1 << 6)) == 0) {
        zone->output = -1;
    } else {
        zone->output = *((int8_t *)&data[166]);
    }
    
    zone->key_lo = *((uint8_t *)&data[90]);
    zone->key_hi = *((uint8_t *)&data[91]);
    
    zone->pitch = ((*((uint8_t *)&data[84])) & (1 << 1)) == 0;
    zone->oneshot = ((*((uint8_t *)&data[84])) & (1 << 0)) != 0;
    zone->revert = ((*((uint8_t *)&data[84])) & (1 << 2)) != 0;
    zone->offset = size > 192 ? *((int32_t *)&data[192]) : 0;
    
    zone->vel_range_on = ((*((uint8_t *)&data[84])) & (1 << 3)) != 0;
    zone->vel_lo = *((uint8_t *)&data[93]);
    zone->vel_hi = *((uint8_t *)&data[94]);
    
    zone->sample_start = *((int32_t *)&data[96]);
    zone->sample_end = *((int32_t *)&data[100]);
    zone->sample_fade = size > 188 ? *((int32_t *)&data[188]) : 0;
    
    zone->loop_on = (*((uint8_t *)&data[117]) & (1 << 0)) != 0;
    zone->loop_start = *((int32_t *)&data[104]);
    zone->loop_end = *((int32_t *)&data[108]);
    zone->loop_tune = *((int8_t *)&data[116]);
    zone->loop_crossfade = *((int32_t *)&data[112]);
    zone->loop_equal_power = (*((uint8_t *)&data[117]) & (1 << 1)) != 0;
    
    if (is_big_endian) {
        zone->group_index = ym_swap_endian_int32(zone->group_index);
        zone->sample_index = ym_swap_endian_int32(zone->sample_index);
        zone->offset = ym_swap_endian_int32(zone->offset);
        zone->sample_start = ym_swap_endian_int32(zone->sample_start);
        zone->sample_end = ym_swap_endian_int32(zone->sample_end);
        zone->sample_fade = ym_swap_endian_int32(zone->sample_fade);
        zone->loop_start = ym_swap_endian_int32(zone->loop_start);
        zone->loop_end = ym_swap_endian_int32(zone->loop_end);
        zone->loop_crossfade = ym_swap_endian_int32(zone->loop_crossfade);
    }
    
    return zone;
}

static ym_exs_group_ref _ym_exs_group_create(char *data, uint32_t size, bool is_big_endian) {
    ym_exs_group_ref group;
    
    group = ym_alloc(struct _ym_exs_group, NULL);
    if (group == NULL) return NULL;
    group->id = *((uint32_t *)&data[8]);
    if (is_big_endian) group->id = ym_swap_endian_uint32(group->id);
    memcpy(group->name, &data[20], 64);
    
    group->key_lo = *((uint8_t *)&data[172]);
    group->key_hi = *((uint8_t *)&data[173]);
    
    group->volume = *((int8_t *)&data[84]);
    group->pan = *((int8_t *)&data[85]);
    group->output = *((int8_t *)&data[158]);
    
    group->polyphony = *((int8_t *)&data[86]);
    group->exclusive = *((int8_t *)&data[88]);
    group->trigger = *((int8_t *)&data[157]);
    group->decay = (*((uint8_t *)&data[87]) & (1 << 6)) != 0;
    group->decay_time = *((uint32_t *)&data[100]);
    
    group->cutoff = *((int8_t *)&data[125]);
    group->resonance = *((int8_t *)&data[127]);
    
    group->a1 = *((int32_t *)&data[184]);
    if (size > 200) group->h1 = *((int32_t *)&data[200]);
    if (size > 188) group->d1 = *((int32_t *)&data[188]);
    if (size > 192) group->s1 = *((int32_t *)&data[192]);
    if (size > 196) group->r1 = *((int32_t *)&data[196]);
    
    group->a2 = *((int32_t *)&data[140]);
    group->h2 = *((int32_t *)&data[180]);
    group->d2 = *((int32_t *)&data[144]);
    group->s2 = *((int32_t *)&data[148]);
    group->r2 = *((int32_t *)&data[152]);
    
    group->vel_lo = *((uint8_t *)&data[89]);
    group->vel_hi = *((uint8_t *)&data[90]);
    
    group->select_type = *((uint8_t *)&data[168]);
    group->select_number = *((uint8_t *)&data[169]);
    group->select_group = *((int32_t *)&data[164]);
    group->select_lo = *((uint8_t *)&data[170]);
    group->select_hi = *((uint8_t *)&data[171]);
    
    if (is_big_endian) {
        group->decay_time = ym_swap_endian_uint32(group->decay_time);
        group->a1 = ym_swap_endian_int32(group->a1);
        group->h1 = ym_swap_endian_int32(group->h1);
        group->d1 = ym_swap_endian_int32(group->d1);
        group->s1 = ym_swap_endian_int32(group->s1);
        group->r1 = ym_swap_endian_int32(group->r1);
        group->a2 = ym_swap_endian_int32(group->a2);
        group->h2 = ym_swap_endian_int32(group->h2);
        group->d2 = ym_swap_endian_int32(group->d2);
        group->s2 = ym_swap_endian_int32(group->s2);
        group->r2 = ym_swap_endian_int32(group->r2);
        group->select_group = ym_swap_endian_int32(group->select_group);
    }
    
    return group;
}

static ym_exs_sample_t *_ym_exs_sample_create(char *data, uint32_t size, bool is_big_endian) {
    ym_exs_sample_t *sample;
    
    sample = ym_alloc(struct _ym_exs_sample, NULL);
    if (sample == NULL) return NULL;
    sample->id = *((uint32_t *)&data[8]);
    if (is_big_endian) sample->id = ym_swap_endian_uint32(sample->id);
    memcpy(sample->name, &data[20], 64);
    
    sample->length = *((int32_t *)&data[88]);
    sample->sample_rate = *((int32_t *)&data[92]);
    sample->bit_depth = *((uint8_t *)&data[96]);
    
    memcpy(sample->file_path, &data[164], 256);
    if (size > 420) {
        memcpy(sample->file_name, &data[420], 256);
    } else {
        memcpy(sample->file_name, &data[20], 64);
    }
    
    sample->type = *((int32_t *)&data[112]);
    
    if (is_big_endian) {
        sample->length = ym_swap_endian_int32(sample->length);
        sample->sample_rate = ym_swap_endian_int32(sample->sample_rate);
        sample->type = ym_swap_endian_uint32(sample->type);
    }
    
    return sample;
}

static ym_exs_param_t _ym_exs_param_create(char *data, uint32_t size, bool is_big_endian) {
    ym_exs_param_t param;
    int i;
    uint8_t keys[100], key;
    int16_t values[100], value;
    
    param = ym_alloc(struct _ym_exs_param, NULL);
    if (param == NULL) return NULL;
    param->id = *((uint32_t *)&data[8]);
    if (is_big_endian) param->id = ym_swap_endian_uint32(param->id);
    memcpy(param->name, &data[20], 64);
    
    memcpy(keys, &data[88], 100 * sizeof(uint8_t));
    memcpy(values, &data[188], 100 * sizeof(int16_t));
    if (is_big_endian) {
        for (i = 0; i < 100; i++) {
            values[i] = ym_swap_endian_int16(values[i]);
        }
    }
    
    for (i = 0; i < 100; i++) {
        key = keys[i];
        value = values[i];
        switch (key) {
            case 7: param->output_volume = value; break;
            case 8: param->key_scale = value; break;
            case 3: param->pitch_bend_up = value; break;
            case 4: param->pitch_bend_down = value; break;
            case 10: param->mono_mode = value; break;
            case 5: param->voices = value; break;
            case 171: param->unison = value; break;
            case 45: param->transpose = value; break;
            case 14: param->coarse_tune = value; break;
            case 15: param->fine_tune = value; break;
            case 20: param->glide_time = value; break;
            case 72: param->pitcher = value; break;
            case 73: param->pitcher_via_vel = value; break;
            case 44: param->filter_on = value; break;
            case 243: param->filter_type = value; break;
            case 170: param->filter_fat = value; break;
            case 30: param->filter_cutoff = value; break;
            case 29: param->filter_resonance = value; break;
            case 75: param->filter_drive = value; break;
            case 46: param->filter_via_key = value; break;
            case 90: param->level_fixed = value; break;
            case 89: param->level_via_vel = value; break;
            case 60: param->lfo1_decay_delay = value; break;
            case 61: param->lfo1_rate = value; break;
            case 62: param->lfo1_waveform = value; break;
            case 64: param->lfo2_waveform = value; break;
            case 63: param->lfo2_rate = value; break;
            case 167: param->lfo3_rate = value; break;
            case 76: param->env1_attack = value; break;
            case 77: param->env1_attack_via_vel = value; break;
            case 78: param->env1_decay = value; break;
            case 79: param->env1_sustain = value; break;
            case 80: param->env1_release = value; break;
            case 82: param->env2_attack = value; break;
            case 92: param->time_via = value; break;
            case 91: param->time_curve = value; break;
            case 83: param->env2_attack_via_vel = value; break;
            case 84: param->env2_decay = value; break;
            case 81: param->env2_sustain = value; break;
            case 85: param->env2_release = value; break;
            case 95: param->velocity_offset = value; break;
            case 164: param->velocity_random = value; break;
            case 97: param->velocity_x_fade = value; break;
            case 165: param->velocity_x_fade_type = value; break;
            case 166: param->coarse_tune_remote = value; break;
            case 172: param->hold_via = value; break;
            case 163: param->sample_select_random = value; break;
            case 98: param->random_detune = value; break;
                
            case 173: param->destinaion[0] = value; break;
            case 179: param->destinaion[1] = value; break;
            case 185: param->destinaion[2] = value; break;
            case 191: param->destinaion[3] = value; break;
            case 197: param->destinaion[4] = value; break;
            case 203: param->destinaion[5] = value; break;
            case 209: param->destinaion[6] = value; break;
            case 215: param->destinaion[7] = value; break;
            case 221: param->destinaion[8] = value; break;
            case 227: param->destinaion[9] = value; break;
                
            case 174: param->source[0] = value; break;
            case 180: param->source[1] = value; break;
            case 186: param->source[2] = value; break;
            case 192: param->source[3] = value; break;
            case 198: param->source[4] = value; break;
            case 204: param->source[5] = value; break;
            case 210: param->source[6] = value; break;
            case 216: param->source[7] = value; break;
            case 222: param->source[8] = value; break;
            case 228: param->source[9] = value; break;
                
            case 175: param->via[0] = value; break;
            case 181: param->via[1] = value; break;
            case 187: param->via[2] = value; break;
            case 193: param->via[3] = value; break;
            case 199: param->via[4] = value; break;
            case 205: param->via[5] = value; break;
            case 211: param->via[6] = value; break;
            case 217: param->via[7] = value; break;
            case 223: param->via[8] = value; break;
            case 229: param->via[9] = value; break;
                
            case 176: param->amount[0] = value; break;
            case 182: param->amount[1] = value; break;
            case 188: param->amount[2] = value; break;
            case 194: param->amount[3] = value; break;
            case 200: param->amount[4] = value; break;
            case 206: param->amount[5] = value; break;
            case 212: param->amount[6] = value; break;
            case 218: param->amount[7] = value; break;
            case 224: param->amount[8] = value; break;
            case 230: param->amount[9] = value; break;
                
            case 177: param->amount_via[0] = value; break;
            case 183: param->amount_via[1] = value; break;
            case 189: param->amount_via[2] = value; break;
            case 195: param->amount_via[3] = value; break;
            case 201: param->amount_via[4] = value; break;
            case 207: param->amount_via[5] = value; break;
            case 213: param->amount_via[6] = value; break;
            case 219: param->amount_via[7] = value; break;
            case 225: param->amount_via[8] = value; break;
            case 231: param->amount_via[9] = value; break;
                
            case 233: param->invert[0] = value; break;
            case 234: param->invert[1] = value; break;
            case 235: param->invert[2] = value; break;
            case 236: param->invert[3] = value; break;
            case 237: param->invert[4] = value; break;
            case 238: param->invert[5] = value; break;
            case 239: param->invert[6] = value; break;
            case 240: param->invert[7] = value; break;
            case 241: param->invert[8] = value; break;
            case 242: param->invert[9] = value; break;
                
            case 178: param->invert_via[0] = value; break;
            case 184: param->invert_via[1] = value; break;
            case 190: param->invert_via[2] = value; break;
            case 196: param->invert_via[3] = value; break;
            case 202: param->invert_via[4] = value; break;
            case 208: param->invert_via[5] = value; break;
            case 214: param->invert_via[6] = value; break;
            case 220: param->invert_via[7] = value; break;
            case 226: param->invert_via[8] = value; break;
            case 232: param->invert_via[9] = value; break;
                
            case 244: param->bypass[0] = value; break;
            case 245: param->bypass[1] = value; break;
            case 246: param->bypass[2] = value; break;
            case 247: param->bypass[3] = value; break;
            case 248: param->bypass[4] = value; break;
            case 249: param->bypass[5] = value; break;
            case 250: param->bypass[6] = value; break;
            case 251: param->bypass[7] = value; break;
            case 252: param->bypass[8] = value; break;
            case 253: param->bypass[9] = value; break;
                
            default: break;
        }
    }
    return param;
}

static bool _ym_exs_validate(ym_exs_ref exs) {
    long i, zone_count, group_count, sample_count;
    ym_exs_zone_ref zone;
    ym_exs_group_ref group;
    ym_exs_sample_t *sample;
    
    if (exs->header == NULL) return false;
    if (exs->param == NULL) return false;
    zone_count = ym_array_count(exs->zones);
    group_count = ym_array_count(exs->groups);
    sample_count = ym_array_count(exs->samples);
    
    for (i = 0; i < sample_count; i++) {
        sample = (ym_exs_sample_t *)ym_array_get(exs->samples, i);
        if (sample->sample_rate <= 0)
            return false;
        if (sample->length <= 0)
            return false;
    }
    
    for (i = 0; i < group_count; i++) {
        group = (ym_exs_group_ref)ym_array_get(exs->groups, i);
        if (group->key_lo > 127)
            return false;
        if (group->key_hi > 127)
            return false;
        if (group->key_lo > group->key_hi)
            return false;
        if (group->volume < -96 || group->volume > 12)
            return false;
        if (group->pan < -100 || group->pan > 100)
            return false;
        if (group->output < 0 || group->output > 15)
            return false;
        if (group->polyphony > 16)
            return false;
        if (group->exclusive > 16)
            return false;
        if (group->cutoff < -50 || group->cutoff > 50)
            return false;
        if (group->resonance < -50 || group->resonance > 50)
            return false;
        if (group->a1 < -9999 || group->a1 > 9999)
            return false;
        if (group->h1 < -9999 || group->a1 > 9999)
            return false;
        if (group->d1 < -9999 || group->a1 > 9999)
            return false;
        if (group->s1 < -9999 || group->a1 > 9999)
            return false;
        if (group->r1 < -9999 || group->a1 > 9999)
            return false;
        if (group->a2 < -9999 || group->a1 > 9999)
            return false;
        if (group->h2 < -9999 || group->a1 > 9999)
            return false;
        if (group->d2 < -9999 || group->a1 > 9999)
            return false;
        if (group->s2 < -9999 || group->a1 > 9999)
            return false;
        if (group->r2 < -9999 || group->a1 > 9999)
            return false;
        if (group->vel_lo > 127)
            return false;
        if (group->vel_hi > 127)
            return false;
        if (group->vel_lo > group->vel_hi)
            return false;
        if (group->select_type > 7)
            return false;
        if (group->select_lo > 127 || group->select_hi > 127)
            return false;
        if (group->select_lo > group->select_hi)
            return false;
    }
    
    for (i = 0; i < zone_count; i++) {
        zone = (ym_exs_zone_ref)ym_array_get(exs->zones, i);
        if (zone->key > 127)
            return false;
        if (zone->coarse_tuning < -50 || zone->coarse_tuning > 50)
            return false;
        if (zone->fine_tuning < -50 || zone->fine_tuning > 50)
            return false;
        if (zone->volume < -96 || zone->volume > 12)
            return false;
        if (zone->pan < -100 || zone->pan > 100)
            return false;
        if (zone->scale < -6 || zone->scale > 6)
            return false;
        if (zone->key_lo > 127)
            return false;
        if (zone->key_hi > 127)
            return false;
        if (zone->key_lo > zone->key_hi)
            return false;
        if (zone->offset < 0 || zone->offset > 10000)
            return false;
        if (zone->vel_lo > 127)
            return false;
        if (zone->vel_hi > 127)
            return false;
        if (zone->vel_lo > zone->vel_hi)
            return false;
        if (zone->group_index < -1 || zone->group_index >= group_count)
            return false;
        if (zone->sample_index < -1 || zone->sample_index >= sample_count)
            return false;
        
        //        if (zone->sample_index >= 0) {
        //            sample = (ym_exs_sample_t *) ym_array_get(exs->samples, zone->sample_index);
        //            if (zone->sample_start < 0 || zone->sample_start >= sample->length)
        //                return false;
        //            if (zone->sample_end <= 0 || zone->sample_end > sample->length)
        //                return false;
        //            if (zone->sample_start >= zone->sample_end)
        //                return false;
        //            if (zone->loop_on) {
        //                if (zone->loop_start < zone->sample_start || zone->loop_start > zone->sample_end)
        //                    return false;
        //                if (zone->loop_end < zone->sample_start || zone->loop_end > zone->sample_end)
        //                    return false;
        //                if (zone->loop_start >  zone->loop_end)
        //                    return false;
        //            }
        //        }
    }
    
    return true;
}

double ym_exs_time_to_second(int16_t time) {
    double time_table[] = { 0, 0.001, 0.003, 0.009, 0.024, 0.049, 0.092, 0.157, 0.252, 0.384, 0.562, 0.797, 1.097, 1.476, 1.946, 2.519, 3.210, 4.035, 5.009, 6.150, 7.475, 9.004, 10.757, 12.753, 15.015, 17.566, 20.428, 23.627, 27.187, 31.136, 35.500, 40.307, 45.586, 51.368, 57.684, 64.564, 72.043, 80.152, 88.929, 98.406, 108.62, 119.61, 131.41, 144.07, 157.62, 172.11, 187.57, 204.05, 221.59, 240.25, 260.05, 281.05, 303.31, 326.85, 351.75, 387.03, 405.77, 435.00, 465.79, 498.18, 532.23, 568.00, 605.54, 644.91, 686.18, 729.39, 774.61, 821.90, 871.32, 922.94, 976.82, 1033.0, 1091.6, 1152.6, 1216.2, 1282.4, 1351.2, 1422.8, 1497.2, 1574.5, 1654.7, 1737.9, 1824.3, 1913.8, 2006.5, 2102.7, 2202.2, 2305.2, 2411.8, 2522.0, 2636.0, 2753.8, 2875.5, 3001.2, 3130.9, 3264.9, 3403.0, 3545.5, 3692.5, 3844.0, 4000.0, 4160.8, 4326.4, 4496.9, 4672.4, 4852.9, 5038.7, 5229.7, 5426.1, 5628.0, 5835.4, 6048.6, 6267.5, 6492.3, 6723.2, 6960.1, 7023.2, 7452.6, 7708.5, 7970.9, 8239.9, 8515.7, 8798.4, 9088.0, 9384.8, 9688.7, 10000 };
    time = YM_CLAMP(time, 0, 127);
    return time_table[time] / 1000.0;
}

static ym_exs_param_t ym_exs_default_param_create() {
    char data[] = { 1, 1, 0, 4, 48, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 66, 79, 83, 68, 101, 102, 97, 117, 108, 116, 32, 80, 97, 114, 97, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 7, 8, 3, 4, 5, 20, 72, 73, -13, 30, 29, 75, 46, 90, 89, 60, 61, 62, 64, 63, 76, 77, 78, 79, 80, 92, 91, 82, 83, 84, 81, 85, 95, -92, -93, 98, 97, -91, -89, -90, -84, -83, -82, -81, -80, -79, -77, -76, -75, -74, -73, -71, -70, -69, -68, -67, -65, -64, -63, -62, -61, -59, -58, -57, -56, -55, -53, -52, -51, -50, -49, -47, -46, -45, -44, -43, -41, -40, -39, -38, -37, -35, -34, -33, -32, -31, -29, -28, -27, -26, -25, 22, 25, -2, 0, 0, 0, 0, 0, 0, -6, -1, 0, 0, 2, 0, -1, -1, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, -24, 3, 0, 0, 100, 0, 0, 0, -60, -1, 0, 0, 0, 0, 98, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 98, 0, -1, -1, 64, 0, 2, 0, -3, -1, -1, -1, -24, 3, -24, 3, 6, 0, -12, -1, 1, 0, 0, 0, 87, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 2, 0, -24, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
    ym_exs_param_t param = _ym_exs_param_create(data, sizeof(data) / sizeof(char), false);
    return param;
}

#define exs_err(err) do { \
ym_release(exs); \
if (error) *error = err; \
return NULL; \
} while (0)


ym_exs_ref ym_exs_create_from_data(const void *data, size_t data_size, ym_error_code *error) {
    uint32_t magic;
    bool is_big_endian;
    bool is_size_expanded;
    uint32_t header_size;
    long ofs;
    ym_exs_ref exs;
    
    exs = NULL;
    if (error) *error = YM_ERROR_NONE;
    if (data == NULL || data_size == 0) exs_err(YM_ERROR_INVALID_PARAMS);
    if (data_size < 84) exs_err(YM_ERROR_FILE_FORMAT);
    
    magic = *((uint32_t *)&data[16]);
    if (magic != ym_four_cc("TBOS") &&
        magic != ym_four_cc("JBOS") &&
        magic != ym_four_cc("SOBT") &&
        magic != ym_four_cc("SOBJ")) {
        exs_err(YM_ERROR_FILE_FORMAT);
    }
    
    if (magic == ym_four_cc("SOBT") || magic == ym_four_cc("SOBJ")) is_big_endian = true;
    else is_big_endian = false;
    
    header_size = *((uint32_t *)&data[4]);
    if (is_big_endian) header_size = ym_swap_endian_uint32(header_size);
    if (header_size > 0x8000) is_size_expanded = true;
    else is_size_expanded = false;
    
    exs = ym_alloc(struct _ym_exs, _ym_exs_dealloc);
    if (exs == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    
    exs->zones = ym_array_create_for_object();
    if (exs->zones == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    exs->groups = ym_array_create_for_object();
    if (exs->groups == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    exs->samples = ym_array_create_for_object();
    if (exs->samples == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    
    ofs = 0;
    while (ofs + 84 < data_size) {
        uint32_t sig, size, magic, chunk_type;
        
        sig = *((uint32_t *)&data[ofs]);
        size = *((uint32_t *)&data[ofs + 4]);
        magic = *((uint32_t *)&data[ofs + 16]);
        if (is_big_endian) {
            size = ym_swap_endian_uint32(size);
            magic = ym_swap_endian_uint32(magic);
        }
        if (is_size_expanded) {
            if (size > 0x8000) size -= 0x8000;
        }
        
        if (magic != ym_four_cc("TBOS") &&
            magic != ym_four_cc("JBOS")) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
        if (ofs + size + 84 > data_size) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
        
        chunk_type = (sig & 0x0F000000) >> 24;
        switch (chunk_type) {
            case 0x00: { //Header Chunk
                ym_exs_header_ref header;
                
                if (size != 40 && size != 80) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
                header = _ym_exs_header_create((char *)data + ofs, size + 84, is_big_endian);
                if (header == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
                if (exs->header) ym_release(header);
                else (exs->header = header);
            } break;
                
            case 0x01: { //Zone Chunk
                ym_exs_zone_ref zone;
                
                if (size < 104) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
                zone = _ym_exs_zone_create((char *)data + ofs, size + 84, is_big_endian);
                if (zone == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
                ym_array_append(exs->zones, zone);
                ym_release(zone);
            } break;
                
            case 0x02: { //Group Chunk
                ym_exs_group_ref group;
                
                if (size < 96) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
                group = _ym_exs_group_create((char *)data + ofs, size + 84, is_big_endian);
                if (group == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
                ym_array_append(exs->groups, group);
                ym_release(group);
            } break;
                
            case 0x03: { //Sample Chunk
                ym_exs_sample_t *sample;
                
                if (size != 336 && size != 592) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
                sample = _ym_exs_sample_create((char *)data + ofs, size + 84, is_big_endian);
                if (sample == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
                ym_array_append(exs->samples, sample);
                ym_release(sample);
            } break;
                
            case 0x04: { //Param Chunk
                ym_exs_param_t param;
                
                if (size != 304) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
                param = _ym_exs_param_create((char *)data + ofs, size + 84, is_big_endian);
                if (param == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
                if (exs->param) ym_release(param);
                else (exs->param = param);
            } break;
                
            default: { //Unknown Chunk
            } break;
        }
        ofs += size + 84;
    }
    
    if (exs->param == NULL) exs->param = ym_exs_default_param_create();
    if (exs->param == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    
    if (!_ym_exs_validate(exs)) {
        exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
    }
    
    return exs;
}

ym_exs_ref ym_exs_create_from_file(FILE *fd, ym_error_code *error) {
    long length;
    void *data;
    ym_exs_ref exs;
    
    exs = NULL;
    if (error) *error = YM_ERROR_NONE;
    if (fd == NULL) exs_err(YM_ERROR_INVALID_PARAMS);
    if (fseek(fd, 0, SEEK_END) != 0) exs_err(YM_ERROR_FILE_READ);
    
    length = ftell(fd);
    if (length < 0) exs_err(YM_ERROR_FILE_READ);
    if (length == 0) exs_err(YM_ERROR_FILE_FORMAT);
    if (length > 10 * 1024 * 1024) exs_err(YM_ERROR_FILE_FORMAT_CONTENT);
    fseek(fd, 0, SEEK_SET);
    
    data = malloc(length);
    if (data == NULL) exs_err(YM_ERROR_ALLOC_MEMORY);
    
    if (fread(data, length, 1, fd) != 1) {
        free(data);
        exs_err(YM_ERROR_FILE_READ);
    }
    exs = ym_exs_create_from_data(data, length, error);
    free(data);
    return exs;
}

ym_exs_ref ym_exs_create_from_path(const char *path, ym_error_code *error) {
    FILE *fd;
    ym_exs_ref exs;
    
    exs = NULL;
    if (error) *error = YM_ERROR_NONE;
    if (path == NULL) exs_err(YM_ERROR_INVALID_PARAMS);
    fd = fopen(path, "rb");
    if (fd == NULL) exs_err(YM_ERROR_FILE_OPEN);
    exs = ym_exs_create_from_file(fd, error);
    fclose(fd);
    return exs;
}
