#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void walk(const char *path) {
    printf("%s\n", path);
    struct stat st;
    if (stat(path, &st) == -1) return;
    if (!S_ISDIR(st.st_mode)) return;

    DIR *d = opendir(path);
    if (!d) return;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        char child[4096];
        snprintf(child, sizeof(child), "%s/%s", path, ent->d_name);
        walk(child);
    }
    closedir(d);
}

int main(int argc, char **argv) {
    const char *start = ".";
    if (argc >= 2) start = argv[1];
    walk(start);
    return 0;
}
