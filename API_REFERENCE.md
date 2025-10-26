# API & System Call Reference

## stat(2) System Call

```c
#include <sys/stat.h>
int stat(const char *restrict path, struct stat *restrict statbuf);
int lstat(const char *restrict path, struct stat *restrict statbuf);
```

**Used in:** stat.c, myls.c, myfind.c

**Key fields from `struct stat`:**
- `st_size`: File size in bytes
- `st_blksize`: Preferred I/O block size
- `st_blocks`: Number of 512-byte blocks allocated
- `st_nlink`: Number of hard links
- `st_mode`: File type and permissions
- `st_uid` / `st_gid`: Owner and group IDs
- `st_mtime`: Last modification time

## Directory Operations

```c
#include <dirent.h>
DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
```

**Used in:** myls.c, myfind.c

**struct dirent:**
- `d_name`: Filename (null-terminated)
- `d_ino`: Inode number
- `d_type`: File type (DT_REG, DT_DIR, etc)

## File I/O Operations

```c
#include <fcntl.h>
#include <unistd.h>

int open(const char *path, int oflag);
ssize_t read(int fildes, void *buf, size_t nbyte);
off_t lseek(int fildes, off_t offset, int whence);
int close(int fildes);
```

**Used in:** mytail.c

**Key flags:**
- `O_RDONLY`: Open for reading
- `SEEK_SET`: Beginning of file
- `SEEK_END`: End of file
- `SEEK_CUR`: Current position

## User & Group Information

```c
#include <pwd.h>
#include <grp.h>

struct passwd *getpwuid(uid_t uid);
struct group *getgrgid(gid_t gid);
```

**Used in:** stat.c, myls.c

**Returns:**
- `struct passwd`: Contains `pw_name`, `pw_uid`
- `struct group`: Contains `gr_name`, `gr_gid`

## String Utilities

```c
#include <string.h>

char *strncpy(char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strerror(int errnum);
```

## Time Utilities

```c
#include <time.h>

struct tm *localtime_r(const time_t *restrict timer,
                       struct tm *restrict result);
size_t strftime(char *restrict s, size_t maxsize,
                const char *restrict format,
                const struct tm *restrict timeptr);
```

## Memory Management

```c
#include <stdlib.h>

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void qsort(void *base, size_t nel, size_t width,
           int (*compar)(const void *, const void *));
```

**Used in:** myls.c (for dynamic arrays and sorting)

## Symlink Operations

```c
#include <unistd.h>

ssize_t readlink(const char *restrict path,
                 char *restrict buf, size_t bufsize);
```

**Used in:** myls.c

**Returns:** Number of bytes written to buffer, or -1 on error

## Error Handling

All system calls that can fail return -1 and set `errno`. The pattern used:

```c
if (function_call(...) == -1) {
    fprintf(stderr, "function: %s\n", strerror(errno));
    return 2;  // Exit with error code
}
```

## Permission Constants

From `<sys/stat.h>`:
- `S_IRUSR`, `S_IWUSR`, `S_IXUSR`: User read/write/execute
- `S_IRGRP`, `S_IWGRP`, `S_IXGRP`: Group read/write/execute
- `S_IROTH`, `S_IWOTH`, `S_IXOTH`: Other read/write/execute

## File Type Macros

From `<sys/stat.h>`:
- `S_ISREG(m)`: Regular file
- `S_ISDIR(m)`: Directory
- `S_ISLNK(m)`: Symbolic link
- `S_ISCHR(m)`: Character device
- `S_ISBLK(m)`: Block device
- `S_ISFIFO(m)`: FIFO (named pipe)
- `S_ISSOCK(m)`: Socket
