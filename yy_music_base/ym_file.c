//
//  ym_file.c
//  yy_music
//
//  Created by ibireme on 15/1/16.
//  Copyright (c) 2015 ibireme. All rights reserved.
//

#include "ym_file.h"
#include "ym_util.h"

#if YM_OS_WINDOWS
#define _ym_tell _ftelli64
#define _ym_seek _fseeki64
#elif YM_OS_APPLE
#define _ym_tell ftello
#define _ym_seek fseeko
#else
#define _ym_tell ftello64
#define _ym_seek fseeko64
#endif

struct _ym_file {
    FILE *fd;
    ym_file_mode mode;
    ym_endian endian;
    char *path;
    char *name;
    char *dir;
    bool close_when_dealloc;
};

static void _ym_file_dealloc(ym_file_ref file) {
    if (file->close_when_dealloc) {
        if (file->fd) fclose(file->fd);
    }
    if (file->path) free(file->path);
    if (file->name) free(file->name);
    if (file->dir) free(file->dir);
}

static bool _ym_file_dump_path(ym_file_ref file, const char *path) {
    file->path = strdup(path);
    if (!file->path) return false;
    
    uint64_t len = strlen(file->path);
    if (len == 0) return false;
    
    int index = -1;
    for (int i = (int)len - 1; i >= 0; i--) {
        if (file->path[i] == YM_FILE_DIR_SEPARATOR) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        file->name = strdup(file->path);
        if (!file->name) return false;
    } else {
        file->name = strdup(file->path + index + 1);
        if (!file->name) return false;
        file->dir = strdup(file->path);
        if (!file->dir) return false;
        file->dir[index + 1] = 0;
    }
    
    return true;
}

ym_file_ref ym_file_create(const char *path, ym_file_mode mode, ym_error_code *error) {
#define _err(code) do { \
if (error) *error = code; \
ym_release(file); \
return NULL; \
} while (0)
    
    ym_file_ref file = NULL;
    if (error) *error = YM_ERROR_NONE;
    if (!path || path[0] == 0) _err(YM_ERROR_INVALID_PARAMS);
    if (!(mode & YM_FILE_READ) && !(mode & YM_FILE_WRITE)) _err(YM_ERROR_INVALID_PARAMS);
    
    file = ym_alloc(struct _ym_file, _ym_file_dealloc);
    if (!file) _err(YM_ERROR_ALLOC_MEMORY);
    
    file->close_when_dealloc = true;
    file->mode = mode;
    if ((mode & YM_FILE_READ) && !(mode & YM_FILE_WRITE)) {
        file->fd = fopen(path, "rb");
    } else if (!(mode & YM_FILE_READ) && (mode & YM_FILE_WRITE)) {
        file->fd = fopen(path, "ab");
    } else {
        file->fd = fopen(path, "rb+");
        if (!file->fd) file->fd = fopen(path, "wb+");
    }
    if (!file->fd) _err(YM_ERROR_FILE_OPEN);
    if (!_ym_file_dump_path(file, path)) _err(YM_ERROR_ALLOC_MEMORY);
    
    return file;
    
#undef _err
}

ym_file_mode ym_file_get_mode(ym_file_ref file) {
    return file->mode;
}

const char *ym_file_get_path(ym_file_ref file) {
    return file->path;
}

const char *ym_file_get_name(ym_file_ref file) {
    return file->name;
}

const char *ym_file_get_dir(ym_file_ref file) {
    return file->dir;
}

int64_t ym_file_get_size(ym_file_ref file) {
    clearerr(file->fd);
    int64_t ofs = _ym_tell(file->fd);
    if (ofs == EOF) return -1;
    
    if (_ym_seek(file->fd, 0, SEEK_END) != 0) return -1;
    int64_t size = _ym_tell(file->fd);
    if (size == EOF) return -1;
    
    if (ofs != size) {
        if (_ym_seek(file->fd, ofs, SEEK_SET) != 0) return -1;
    }
    return size;
}

ym_endian ym_file_get_endian(ym_file_ref file) {
    return file->endian;
}

void ym_file_set_endian(ym_file_ref file, ym_endian endian) {
    file->endian = endian;
}

int64_t ym_file_tell(ym_file_ref file) {
    return _ym_tell(file->fd);
}

bool ym_file_seek_set(ym_file_ref file, int64_t ofs) {
    if (ofs < 0) return false;
    if (_ym_seek(file->fd, ofs, SEEK_SET) != 0) return false;
    if (_ym_tell(file->fd) != ofs) return false;
    return true;
}

bool ym_file_seek_end(ym_file_ref file, int64_t ofs) {
    if (ofs > 0) return false;
    if (_ym_seek(file->fd, ofs, SEEK_END) != 0) return false;
    if (_ym_tell(file->fd) < 0) return false;
    return true;
}

bool ym_file_seek_cur(ym_file_ref file, int64_t ofs) {
    if (ofs == 0) return true;
    int64_t before = _ym_tell(file->fd);
    if (_ym_seek(file->fd, ofs, SEEK_CUR) != 0) return false;
    int64_t after = _ym_tell(file->fd);
    if (before == after || after < 0) return false;
    return true;
}

bool ym_file_is_eof(ym_file_ref file) {
    return feof(file->fd) != 0;
}

void ym_file_clear_eof(ym_file_ref file) {
    clearerr(file->fd);
}

void ym_file_flush(ym_file_ref file) {
    fflush(file->fd);
}

uint32_t ym_file_write(ym_file_ref file, const void *buf, uint32_t size) {
    return (uint32_t)fwrite(buf, 1, size, file->fd);
}

uint32_t ym_file_read(ym_file_ref file, void *buf, uint32_t size) {
    return (uint32_t)fread(buf, 1, size, file->fd);
}

bool ym_file_read_uint8(ym_file_ref file, uint8_t *buf) {
    return fread(buf, 1, sizeof(uint8_t), file->fd) == 1;
}

bool ym_file_read_uint16(ym_file_ref file, uint16_t *buf) {
    size_t size = fread(buf, sizeof(uint16_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_uint16(*buf);
    }
    return true;
}

bool ym_file_read_uint32(ym_file_ref file, uint32_t *buf) {
    size_t size = fread(buf, sizeof(uint32_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_uint32(*buf);
    }
    return true;
}

bool ym_file_read_uint64(ym_file_ref file, uint64_t *buf) {
    size_t size = fread(buf, sizeof(uint64_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_int64(*buf);
    }
    return true;
}

bool ym_file_read_int8(ym_file_ref file, int8_t *buf) {
    return fread(buf, sizeof(int8_t), 1, file->fd) == 1;
}

bool ym_file_read_int16(ym_file_ref file, int16_t *buf) {
    size_t size = fread(buf, sizeof(int16_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_int16(*buf);
    }
    return true;
}

bool ym_file_read_int32(ym_file_ref file, int32_t *buf) {
    size_t size = fread(buf, sizeof(int32_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_int32(*buf);
    }
    return true;
}

bool ym_file_read_int64(ym_file_ref file, int64_t *buf) {
    size_t size = fread(buf, sizeof(int64_t), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_int64(*buf);
    }
    return true;
}

bool ym_file_read_float(ym_file_ref file, float *buf) {
    size_t size = fread(buf, sizeof(float), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_float(*buf);
    }
    return true;
}

bool ym_file_read_double(ym_file_ref file, double *buf) {
    size_t size = fread(buf, sizeof(double), 1, file->fd);
    if (size != 1) return false;
    if (ym_endian_need_swap(file->endian)) {
        *buf = ym_swap_endian_double(*buf);
    }
    return true;
}

bool ym_file_read_fourcc(ym_file_ref file, uint32_t *buf) {
    return fread(buf, sizeof(uint32_t), 1, file->fd) == 1;
}

bool ym_file_read_vlq32(ym_file_ref file, uint32_t *buf) {
    uint32_t num = 0;
    uint8_t tmp = 0;
    for (int i = 0; i < 4; i++) {
        if (fread(&tmp, 1, sizeof(uint8_t), file->fd) != 1) return false;
        num += tmp & 0x7F;
        if ((tmp & 0x80) == 0) break;
        if (i == 3) return false;
        num <<= 7;
    }
    *buf = num;
    return true;
}

bool ym_file_read_vlq64(ym_file_ref file, uint64_t *buf) {
    uint32_t num = 0;
    uint8_t tmp = 0;
    for (int i = 0; i < 8; i++) {
        if (fread(&tmp, 1, sizeof(uint8_t), file->fd) != 1) return false;
        num += tmp & 0x7F;
        if ((tmp & 0x80) == 0) break;
        if (i == 7) return false;
        num <<= 7;
    }
    *buf = num;
    return true;
}

bool ym_file_write_uint8(ym_file_ref file, uint8_t val) {
    return fwrite(&val, sizeof(uint8_t), 1, file->fd) == 1;
}

bool ym_file_write_uint16(ym_file_ref file, uint16_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_uint16(val);
    }
    size_t size = fwrite(&val, sizeof(uint16_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_uint32(ym_file_ref file, uint32_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_uint32(val);
    }
    size_t size = fwrite(&val, sizeof(uint32_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_uint64(ym_file_ref file, uint64_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_uint64(val);
    }
    size_t size = fwrite(&val, sizeof(uint64_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_int8(ym_file_ref file, int8_t val) {
    return fwrite(&val, sizeof(int8_t), 1, file->fd) == 1;
}

bool ym_file_write_int16(ym_file_ref file, int16_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_int16(val);
    }
    size_t size = fwrite(&val, sizeof(int16_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_int32(ym_file_ref file, int32_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_int32(val);
    }
    size_t size = fwrite(&val, sizeof(int32_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_int64(ym_file_ref file, int64_t val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_int64(val);
    }
    size_t size = fwrite(&val, sizeof(int64_t), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_float(ym_file_ref file, float val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_float(val);
    }
    size_t size = fwrite(&val, sizeof(float), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_double(ym_file_ref file, double val) {
    if (ym_endian_need_swap(file->endian)) {
        val = ym_swap_endian_double(val);
    }
    size_t size = fwrite(&val, sizeof(double), 1, file->fd);
    if (size != 1) return false;
    return true;
}

bool ym_file_write_fourcc(ym_file_ref file, uint32_t val) {
    return fwrite(&val, sizeof(uint32_t), 1, file->fd) == 1;
}

bool ym_file_write_vlq32(ym_file_ref file, uint32_t val) {
    uint32_t buf;
    if (val > 0x0FFFFFFFUL) return false;
    buf = val & 0x7F;
    while ((val >>= 7)) {
        buf = (buf << 8) | ((val & 0x7F) | 0x80);
    }
    while (true) {
        uint8_t tmp = buf & 0xFF;
        if (fwrite(&tmp, 1, sizeof(uint8_t), file->fd) != 1) return false;
        if (buf & 0x80) buf >>= 8;
        else break;
    }
    return true;
}

bool ym_file_write_vlq64(ym_file_ref file, uint64_t val) {
    uint64_t buf;
    if (val > 0x00FFFFFFFFFFFFFFULL) return false;
    buf = val & 0x7F;
    while ((val >>= 7)) {
        buf = (buf << 8) | ((val & 0x7F) | 0x80);
    }
    while (true) {
        uint8_t tmp = buf & 0xFF;
        if (fwrite(&tmp, 1, sizeof(uint8_t), file->fd) != 1) return false;
        if (buf & 0x80) buf >>= 8;
        else break;
    }
    return true;
}
