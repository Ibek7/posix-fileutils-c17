# Architecture Guide

## System Overview

The POSIX File Utilities project implements four core command-line tools that interact with the filesystem through standard POSIX system calls. Each utility is independently compiled and deployed as a standalone binary.

```
┌─────────────────────────────────────────────┐
│        User (CLI Interface)                 │
└────────┬────────────────────────────────────┘
         │
    ┌────┴────────┬────────────┬────────────┐
    │             │            │            │
┌───▼──┐      ┌──▼──┐      ┌──▼──┐      ┌─▼────┐
│ stat │      │myls │      │mytail│     │myfind│
└───┬──┘      └──┬──┘      └──┬──┘      └─┬────┘
    │            │            │          │
    └────────────┼────────────┼──────────┘
                 │            │
         ┌───────▼────────────▼────────┐
         │   POSIX System Call Layer   │
         ├─────────────────────────────┤
         │ stat/lstat, open/read/seek, │
         │ opendir/readdir, readlink   │
         └───────────┬─────────────────┘
                     │
         ┌───────────▼─────────────────┐
         │   Linux Kernel (VFS)        │
         │ (File System Operations)    │
         └─────────────────────────────┘
```

## Module Design

### stat.c - File Metadata Retriever
**Purpose:** Display detailed file information using stat() system call.

**Key Functions:**
- `print_mode(mode_t mode)` – Formats permission bits into rwx notation
- `main(argc, argv)` – Iterates arguments, calls stat(), prints metadata

**Data Flow:**
```
Filename → stat() syscall → struct stat → parse & print → User
```

**POSIX APIs Used:**
- `stat(path, &st)` – Retrieves file metadata
- `getpwuid(uid)` – Resolves user name
- `getgrgid(gid)` – Resolves group name
- `localtime(&mtime)` – Formats modification time

### myls.c - Directory Lister
**Purpose:** List directory contents with optional formatting and sorting.

**Key Functions:**
- `long_format()` – Prints long format with permissions, owner, size, date
- `sort_by_size()`, `sort_by_time()` – Comparators for qsort()
- `main(argc, argv)` – Parses flags, reads directory, sorts, prints

**Data Structure:**
```c
typedef struct {
    struct stat st;
    char name[256];
    char link_target[256];  // For symlinks
} FileEntry;
```

**Data Flow:**
```
Directory Path → opendir() → readdir() loop → lstat() for each → 
qsort(entries) → print → User
```

**POSIX APIs Used:**
- `opendir(path)`, `readdir(dir)`, `closedir(dir)` – Directory enumeration
- `lstat(path, &st)` – File metadata (doesn't follow symlinks)
- `readlink(path, target)` – Read symlink target
- `qsort()` – Sort entries by size/time

**Supported Flags:**
- `-l` – Long format (permissions, owner, size, mtime)
- `-S` – Sort by file size (largest first)
- `-t` – Sort by modification time (newest first)

### mytail.c - Last-N-Lines Printer
**Purpose:** Efficiently display the last N lines of a file.

**Algorithm:** Backward Scan
1. Seek to EOF
2. Read file in 4KB blocks from end backward
3. Count newlines until reaching N
4. Print from found position to EOF

**Data Flow:**
```
File Path → stat(size) → open() → lseek(EOF) → read(blocks backward) →
scan newlines → print from position → close() → User
```

**POSIX APIs Used:**
- `open(path, O_RDONLY)` – Open file read-only
- `lseek(fd, offset, SEEK_END)` – Seek to end or position
- `read(fd, buffer, size)` – Read data
- `close(fd)` – Close file descriptor

**Performance Characteristics:**
- O(file_size) worst case (if N > file lines)
- O(N + block_size) average case (efficient for large files)
- Memory: Fixed 4KB buffer, O(1) space

### myfind.c - Recursive Directory Tree Walker
**Purpose:** Recursively traverse directory tree, printing all file paths.

**Key Functions:**
- `walk(path, depth)` – Recursive directory walker with depth tracking
- `main(argc, argv)` – Parses arguments, initiates walk

**Data Structure:** Simple stack-based recursion (implicitly via call stack)

**Data Flow:**
```
Starting Path → walk(path, depth=0) → check depth limit → 
opendir() → readdir() loop → stat() for each → 
recurse if directory → walk(subdir, depth+1) → print paths → User
```

**POSIX APIs Used:**
- `opendir(path)`, `readdir(dir)`, `closedir(dir)` – Directory traversal
- `stat(path, &st)` – Check if entry is directory
- `S_ISDIR(st.st_mode)` – Directory type macro

**Supported Flags:**
- `-maxdepth N` – Limit recursion depth (e.g., `-maxdepth 2` for top 2 levels)

**Complexity:**
- Time: O(N) where N = total files in tree
- Space: O(D) where D = max recursion depth (call stack)

## Compilation Architecture

**Build System:** GNU Makefile
```makefile
Targets:
  stat     → gcc stat.c -o stat -Wall -Wextra -std=c17
  myls     → gcc myls.c -o myls -Wall -Wextra -std=c17
  mytail   → gcc mytail.c -o mytail -Wall -Wextra -std=c17
  myfind   → gcc myfind.c -o myfind -Wall -Wextra -std=c17
  all      → Build all utilities
  clean    → Remove binaries and object files
```

**Compiler Flags:**
- `-std=c17` – Use C17 standard (supports modern features)
- `-Wall -Wextra` – Enable all common warnings (zero tolerance)
- No `-O` flag (unoptimized for clarity in debug scenarios)

## Testing Architecture

### Smoke Tests (tests/run_smoke.sh)
Basic functionality validation for all utilities:
- stat: Verifies output format and fields
- myls: Lists directory, checks entry presence
- mytail: Reads file, verifies line count
- myfind: Recursively finds paths

### Extended Tests (scripts/extended_tests.sh)
Advanced scenario coverage:
- myls sorting: Verify `-S` and `-t` flags
- myfind depth limiting: Test `-maxdepth` flag
- stat special files: Handle symlinks, devices

### Performance Benchmarks (scripts/performance_bench.sh)
Measure efficiency metrics:
- stat: 1000 iterations, average latency
- myls: 100 iterations on /usr/bin, sort overhead
- mytail: 50 iterations on 100MB file
- myfind: Traverse /usr/share, total time

## Error Handling Strategy

**Three-Layer Approach:**

1. **Argument Validation** (main function)
   - Check argc count
   - Validate flag syntax
   - Exit with usage message on error

2. **POSIX Error Checking**
   - Check return values (< 0 for errors)
   - Print `perror()` with system message
   - Exit gracefully (1 = error, 0 = success)

3. **Edge Case Handling**
   - Empty directories (print nothing, exit 0)
   - Large files (use 4KB buffers, lseek)
   - Permission denied (skip or report, continue)
   - Broken symlinks (readlink succeeds, but target missing)

**Example Pattern:**
```c
int fd = open(path, O_RDONLY);
if (fd < 0) {
    perror("open");
    return 1;  // Exit with error
}
// Use fd...
close(fd);
return 0;
```

## Memory Management

**Stack-Based Allocation:**
- FileEntry array in myls: 1024 entries × ~520 bytes = ~512KB
- Read buffers: 4KB for mytail
- struct stat: ~144 bytes per file

**No Heap Allocation Used** (by design)
- Utilities are simple, single-purpose tools
- Stack allocation simplifies error handling
- Prevents memory leaks and fragmentation

**Resource Cleanup:**
- All open file descriptors closed before exit
- All opendir() calls matched with closedir()
- Signal handlers: Not used (simplicity priority)

## Dependency Graph

```
External Dependencies:
├── libc (POSIX system calls)
│   ├── sys/stat.h (stat, lstat)
│   ├── dirent.h (opendir, readdir)
│   ├── fcntl.h (open, O_RDONLY)
│   ├── unistd.h (read, lseek)
│   ├── pwd.h (getpwuid)
│   └── grp.h (getgrgid)
├── stdlib.h (malloc, qsort, strtol)
├── string.h (strcat, strcmp)
├── stdio.h (printf, perror)
└── time.h (localtime, ctime)

Internal Dependencies:
├── stat.c (standalone)
├── myls.c (standalone)
├── mytail.c (standalone)
└── myfind.c (standalone)
```

## Design Principles

1. **Separation of Concerns** – Each utility does one job well
2. **POSIX Compliance** – Use standard system calls (portable)
3. **Error Resilience** – Handle common errors gracefully
4. **Efficiency** – Use appropriate algorithms (backward scan for tail, qsort for ls)
5. **Clarity** – Minimal code, clear intent, helpful error messages
6. **Testability** – Simple interfaces, deterministic behavior

## Performance Optimization Notes

- **stat**: Direct system call, O(1) complexity
- **myls**: O(N log N) due to qsort; file I/O dominates runtime
- **mytail**: O(K + B) where K = target lines, B = buffer size; efficient for large files
- **myfind**: O(N) where N = total files; recursive depth can reach stack limit on very deep trees

**Bottlenecks:**
- Network filesystems (NFS): stat/lstat latency
- Large directories: readdir() iteration slow
- Deep trees: Recursion stack overhead

## Future Enhancement Opportunities

- **stat**: Add `-c` format string (like GNU stat)
- **myls**: Implement `-R` for recursive listing
- **mytail**: Add `-f` follow mode (watch file growth)
- **myfind**: Add `-type f`, `-name` pattern matching
- **All**: Parallel processing for multi-core systems
