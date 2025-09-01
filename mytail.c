#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s n file\n", argv[0]);
        return 2;
    }
    int n = atoi(argv[1]);
    const char *file = argv[2];
    int fd = open(file, O_RDONLY);
    if (fd < 0) { fprintf(stderr, "open %s: %s\n", file, strerror(errno)); return 2; }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == (off_t)-1) { fprintf(stderr, "lseek: %s\n", strerror(errno)); close(fd); return 2; }

    if (file_size == 0) { close(fd); return 0; }

    off_t pos = file_size;
    int lines = 0;
    char buf[BUFSIZE];
    off_t read_size;

    while (pos > 0 && lines <= n) {
        off_t to_read = (pos >= BUFSIZE) ? BUFSIZE : pos;
        pos -= to_read;
        if (lseek(fd, pos, SEEK_SET) == (off_t)-1) break;
        read_size = read(fd, buf, to_read);
        if (read_size <= 0) break;
        for (off_t i = read_size - 1; i >= 0; --i) {
            if (buf[i] == '\n') {
                lines++;
                if (lines > n) { pos += i + 1; goto DONE; }
            }
        }
    }
DONE:
    if (lseek(fd, pos, SEEK_SET) == (off_t)-1) { fprintf(stderr, "lseek: %s\n", strerror(errno)); close(fd); return 2; }
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        ssize_t w = write(STDOUT_FILENO, buf, r);
        if (w != r) break;
    }
    close(fd);
    return 0;
}
