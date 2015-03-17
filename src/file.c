#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

int file_from_fd(struct file_t *file, int fd)
{
    *file = (struct file_t){ .fd = fd };

    fstat(fd, &file->st);

    file->mmap = mmap(NULL, file->st.st_size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
    madvise(file->mmap, file->st.st_size, MADV_WILLNEED | MADV_SEQUENTIAL);

    return file->mmap == MAP_FAILED ? -errno : 0;
}

int file_close(struct file_t *file)
{
    close(file->fd);
    return file->mmap != MAP_FAILED ? munmap(file->mmap, file->st.st_size) : 0;
}
