#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_DEPTH 100

static int max_depth = -1;

static void walk(const char *path, int depth) {
    if (max_depth >= 0 && depth > max_depth) return;
    
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
        walk(child, depth + 1);
    }
    closedir(d);
}

int main(int argc, char **argv) {
    const char *start = ".";
    int ai = 1;

    while (ai < argc) {
        if (strcmp(argv[ai], "-maxdepth") == 0 && ai + 1 < argc) {
            max_depth = atoi(argv[++ai]);
            ai++;
        } else if (argv[ai][0] != '-') {
            start = argv[ai];
            ai++;
        } else {
            ai++;
        }
    }

    walk(start, 0);
    return 0;
}
