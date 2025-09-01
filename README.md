# posix-fileutils-c17 — Homework

This folder contains simple implementations of stat, ls, tail, and find for the homework.

Files:
- `stat.c` - prints file metadata using stat()
- `myls.c` - lists a directory; supports `-l` for long format
- `mytail.c` - prints last N lines efficiently by seeking from the end
- `myfind.c` - recursively prints files and directories starting at a path
- `Makefile` - build targets: `make`, `make clean`

Build:
```sh
make
```

Examples:
```sh
./stat somefile
./myls -l .
./mytail 10 file.txt
./myfind /path/to/dir
```

Limitations: these are minimal teaching implementations. Error handling and edge cases are intentionally small.

Tests:
```
./tests/run_smoke.sh
```

Notes:
- `myls` now uses `lstat()` and will display symlink targets in long mode (e.g., "name -> target").
