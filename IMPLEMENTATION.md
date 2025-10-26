# Implementation Notes

## Design Decisions & Key Features

### stat.c
- **API used**: stat() system call
- **Output**: structured key-value format for readability
- **Features**:
  - Displays file type (regular, directory, symlink, etc.)
  - Unix-style permission string (drwxr-xr-x)
  - Numeric and human-readable owner/group names
  - ISO 8601 timestamp format
  - Handles multiple file arguments

### myls.c
- **API used**: opendir(), readdir(), closedir(), lstat()
- **Modes**:
  - Plain: simple filename listing
  - Long (-l): detailed with permissions, owner, size, time
- **Features**:
  - lstat() for symlink detection (not following symlinks)
  - readlink() to display symlink targets
  - Defaults to CWD if no directory given
  - Efficient directory enumeration

### mytail.c
- **API used**: open(), lseek(), read(), close()
- **Algorithm**: backward seek + line counting
- **Features**:
  - Efficient for large files (doesn't load entire file)
  - Buffer-based reading (4KB chunks)
  - Backward scan from EOF to find N line breaks
  - Handles edge cases: empty files, files smaller than buffer

### myfind.c
- **API used**: stat(), opendir(), readdir(), closedir()
- **Algorithm**: recursive depth-first traversal
- **Features**:
  - Prints all files and subdirectories
  - Skips "." and ".." entries
  - Defaults to CWD
  - Builds full paths for recursive display

## Error Handling
- All programs check return values from system calls
- Report errors to stderr with descriptive messages
- Return non-zero exit code on failure
- Graceful degradation (e.g., skip unreadable files)

## Compilation & Standards
- Compiled with: gcc -Wall -Wextra -std=c17
- Zero compiler warnings
- Follows POSIX standards
- Compatible with macOS and Linux

## Testing
- Smoke test suite: tests/run_smoke.sh
- Validates core functionality of each program
- Can be extended with unit tests as needed
