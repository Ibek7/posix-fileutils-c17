#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

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

static void print_entry_long(const char *dirpath, const char *name) {
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s", dirpath, name);
    struct stat st;
    if (lstat(path, &st) == -1) {
        printf("%s\n", name);
        return;
    }
    print_mode(st.st_mode);
    printf(" %lu", (unsigned long)st.st_nlink);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    printf(" %s %s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
    printf(" %8lld", (long long)st.st_size);
    char tbuf[64];
    struct tm lt;
    localtime_r(&st.st_mtime, &lt);
    strftime(tbuf, sizeof(tbuf), " %b %d %H:%M", &lt);

    /* If this is a symlink, read and display the target */
    if (S_ISLNK(st.st_mode)) {
        char target[PATH_MAX + 1];
        ssize_t len = readlink(path, target, PATH_MAX);
        if (len >= 0) {
            if (len > PATH_MAX) len = PATH_MAX;
            target[len] = '\0';
            printf("%s %s -> %s\n", tbuf, name, target);
            return;
        }
    }

    printf("%s %s\n", tbuf, name);
}

int main(int argc, char **argv) {
    int longflag = 0;
    const char *dir = ".";
    int ai = 1;
    if (argc > 1 && strcmp(argv[1], "-l") == 0) { longflag = 1; ai = 2; }
    if (ai < argc) dir = argv[ai];

    DIR *d = opendir(dir);
    if (!d) { fprintf(stderr, "opendir %s: %s\n", dir, strerror(errno)); return 2; }
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (!longflag) {
            printf("%s\n", ent->d_name);
        } else {
            print_entry_long(dir, ent->d_name);
        }
    }
    closedir(d);
    return 0;
}
