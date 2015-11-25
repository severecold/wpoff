
#ifndef _UTILS_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int file_read_all(const char *path, char **data, size_t *size);
int file_write_all(const char *path, const char *data, size_t size);
int file_read_uint(const char *path, uint64_t *data, int radix);

#ifdef __cplusplus
}
#endif

#endif

