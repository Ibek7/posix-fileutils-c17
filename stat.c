#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static void print_mode(mode_t mode) {
    char str[11];
    str[0] = S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : S_ISCHR(mode) ? 'c' : S_ISBLK(mode) ? 'b' : S_ISFIFO(mode) ? 'p' : S_ISSOCK(mode) ? 's' : '-';
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
    printf("%s", str);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s path\n", argv[0]);
        return 2;
    }

    for (int i = 1; i < argc; ++i) {
        const char *path = argv[i];
        struct stat st;
        if (stat(path, &st) == -1) {
            fprintf(stderr, "stat %s: %s\n", path, strerror(errno));
            continue;
        }

        printf("%s:\n", path);
        printf("  Size: %lld\n", (long long)st.st_size);
        printf("  Blocks: %lld\n", (long long)st.st_blocks);
        printf("  IO Block: %ld\n", (long)st.st_blksize);
        printf("  Links: %lu\n", (unsigned long)st.st_nlink);
        printf("  Device: %llu\n", (unsigned long long)st.st_dev);
        printf("  Inode: %llu\n", (unsigned long long)st.st_ino);
        printf("  Mode: "); print_mode(st.st_mode); printf(" (%#o)\n", st.st_mode & 07777);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        printf("  Owner: %s (%u)\n", pw ? pw->pw_name : "?", (unsigned)st.st_uid);
        printf("  Group: %s (%u)\n", gr ? gr->gr_name : "?", (unsigned)st.st_gid);

        char tbuf[64];
        struct tm lt;
        localtime_r(&st.st_mtime, &lt);
        strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S %z", &lt);
        printf("  Modified: %s\n", tbuf);

        putchar('\n');
    }

    return 0;
}
