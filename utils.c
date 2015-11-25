
#include "utils.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int file_read_all(const char *path, char **data, size_t *size) {
    int rc, fd;
    struct stat fs;
    size_t nbrd;

    rc = stat(path, &fs);
    if (rc)
        return rc;
    *size = fs.st_size;
    *data = malloc(fs.st_size);
    if (*data == NULL)
        return -1;
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        free(*data);
        *data = NULL;
        return -1;
    }
    nbrd = 0;
    while (nbrd < fs.st_size) {
        int tmp;

        tmp = read(fd, *data + nbrd, fs.st_size - nbrd);
        if (tmp < 0) {
            if (errno == -EINTR)
                continue;
            break;
        }
        nbrd += tmp;
    }
    close(fd);
    if (nbrd < fs.st_size) {
        free(*data);
        *data = NULL;
        return -1;
    }

    return 0;
}

int file_write_all(const char *path, const char *data, size_t size) {
    int rc, fd;
    size_t nbwr;

    fd = open(path, O_CREAT | O_WRONLY, 0644);
    if (fd < 0)
        return -1;
    nbwr = 0;
    rc = ftruncate(fd, nbwr);
    if (rc)
        goto fail_truncate;
    while (nbwr < size) {
        int tmp;

        tmp = write(fd, data + nbwr, size - nbwr);
        if (tmp < 0) {
            if (errno == -EINTR)
                continue;
            break;
        }
        nbwr += tmp;
    }
fail_truncate:
    close(fd);

    return nbwr == size ? 0 : -1;
}

int file_read_uint(const char *name, uint64_t *data, int radix) {
    int fd;
    char buff[64];

    fd = open(name, O_RDONLY);
    if (fd < 0)
        return -1;
    memset(buff, 0, sizeof(buff));
    read(fd, buff, sizeof(buff));
    close(fd);
    *data = strtoull(buff, NULL, radix);
    return 0;
}

