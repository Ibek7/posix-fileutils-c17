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

typedef struct {
    char name[256];
    struct stat st;
    char path[4096];
} FileEntry;

static int sort_by_name(const void *a, const void *b) {
    return strcmp(((FileEntry*)a)->name, ((FileEntry*)b)->name);
}

static int sort_by_size(const void *a, const void *b) {
    off_t diff = ((FileEntry*)a)->st.st_size - ((FileEntry*)b)->st.st_size;
    return (diff > 0) - (diff < 0);
}

static int sort_by_time(const void *a, const void *b) {
    time_t diff = ((FileEntry*)a)->st.st_mtime - ((FileEntry*)b)->st.st_mtime;
    return (diff > 0) - (diff < 0);
}

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

static void print_entry_long(FileEntry *ent) {
    print_mode(ent->st.st_mode);
    printf(" %lu", (unsigned long)ent->st.st_nlink);
    struct passwd *pw = getpwuid(ent->st.st_uid);
    struct group  *gr = getgrgid(ent->st.st_gid);
    printf(" %s %s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
    printf(" %8lld", (long long)ent->st.st_size);
    char tbuf[64];
    struct tm lt;
    localtime_r(&ent->st.st_mtime, &lt);
    strftime(tbuf, sizeof(tbuf), " %b %d %H:%M", &lt);

    if (S_ISLNK(ent->st.st_mode)) {
        char target[PATH_MAX + 1];
        ssize_t len = readlink(ent->path, target, PATH_MAX);
        if (len >= 0) {
            if (len > PATH_MAX) len = PATH_MAX;
            target[len] = '\0';
            printf("%s %s -> %s\n", tbuf, ent->name, target);
            return;
        }
    }

    printf("%s %s\n", tbuf, ent->name);
}

int main(int argc, char **argv) {
    int longflag = 0, sortflag = 0;
    const char *dir = ".";
    int (*sortfn)(const void*, const void*) = sort_by_name;
    int ai = 1;

    while (ai < argc && argv[ai][0] == '-') {
        if (strcmp(argv[ai], "-l") == 0) longflag = 1;
        else if (strcmp(argv[ai], "-S") == 0) { sortflag = 1; sortfn = sort_by_size; }
        else if (strcmp(argv[ai], "-t") == 0) { sortflag = 1; sortfn = sort_by_time; }
        ai++;
    }
    if (ai < argc) dir = argv[ai];

    DIR *d = opendir(dir);
    if (!d) { fprintf(stderr, "opendir %s: %s\n", dir, strerror(errno)); return 2; }

    FileEntry *entries = NULL;
    int count = 0, capacity = 10;
    entries = malloc(capacity * sizeof(FileEntry));

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (count >= capacity) {
            capacity *= 2;
            entries = realloc(entries, capacity * sizeof(FileEntry));
        }
        strncpy(entries[count].name, ent->d_name, 255);
        snprintf(entries[count].path, 4096, "%s/%s", dir, ent->d_name);
        lstat(entries[count].path, &entries[count].st);
        count++;
    }
    closedir(d);

    if (sortflag) qsort(entries, count, sizeof(FileEntry), sortfn);

    for (int i = 0; i < count; i++) {
        if (!longflag) {
            printf("%s\n", entries[i].name);
        } else {
            print_entry_long(&entries[i]);
        }
    }

    free(entries);
    return 0;
}
