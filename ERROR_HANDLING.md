# Error Handling & Edge Cases

## Overview

This document outlines error handling strategies and edge cases for each utility.

## stat.c

### Error Cases Handled
- File not found (ENOENT)
- Permission denied (EACCES)
- Invalid path (EINVAL)
- Symbolic links (properly resolved)

### Edge Cases
- Very long filenames (PATH_MAX)
- Files larger than 2GB (uses off_t, long long)
- Special files (sockets, pipes, block devices)
- Deleted/renamed files during stat

### Robustness Features
- Checks stat() return value for -1
- Prints errno message with strerror()
- Continues processing on single file failure

## myls.c

### Error Cases Handled
- Directory not accessible (EACCES)
- Directory removed during read (EBADF)
- Invalid directory path (ENOENT)
- Symlink target not readable (lstat succeeds, readlink may fail)

### Edge Cases
- Very large directories (1000+ entries)
- Files with special characters in names
- Symlink loops (handled by safe readlink limit)
- Empty directories (gracefully shows . and ..)

### Robustness Features
- opendir() error checking
- Safe path construction (snprintf with buffer checks)
- readlink() length validation
- Graceful fallback on readlink failure

## mytail.c

### Error Cases Handled
- File not found (ENOENT)
- Permission denied (EACCES)
- File too small for requested lines
- Files without newlines
- Binary files (treats as byte stream)

### Edge Cases
- Empty files (returns immediately)
- Single-line files (works correctly)
- Files with CRLF line endings (counts \n only)
- Very large files (>10GB)
- N = 0 (prints nothing correctly)
- N > file lines (prints entire file)

### Robustness Features
- Checks all system call returns
- Handles lseek failure gracefully
- Buffer-based reading (handles any file size)
- Proper off_t usage for large files

## myfind.c

### Error Cases Handled
- Directory not found (ENOENT)
- Permission denied (EACCES)
- Symbolic link loops (would cause infinite recursion)
- Removed directories during traversal

### Edge Cases
- Very deep directory trees (recursive depth)
- Directories with thousands of entries
- Mixed file/directory entries
- Hidden files and directories (. and .. filtered)

### Robustness Features
- stat() before opendir to verify it's a directory
- Closed directory handles on error
- Safe path construction (prevents buffer overflow)
- Continues on error (prints path but skips on failure)

## Testing Strategy

### Manual Testing
```bash
# Test with various file types
./stat /dev/null /dev/sda /etc/passwd
./myls -l /proc /dev
./mytail 5 /etc/profile
./myfind / -maxdepth 2  # if implemented
```

### Stress Testing
- Large files (>1GB for mytail)
- Deep directories (>1000 levels for myfind)
- Large directory listings (>10k files for myls)
- Special characters in filenames

### Boundary Testing
- N = 0 for mytail
- Empty directories for myls
- Non-existent paths
- Permission denied scenarios

## Security Considerations

1. **Path Traversal**: Paths are constructed safely using snprintf
2. **Buffer Overflows**: Fixed buffers with size checks (4096 byte paths)
3. **Symlink Attacks**: lstat() prevents following dangerous links
4. **Resource Exhaustion**: Recursive functions have practical limits
5. **TOCTOU**: Potential race conditions noted but acceptable for educational code

## Future Improvements

- Add resource limits (max recursion depth for myfind)
- Implement timeout handling
- Add symbolic link depth limit detection
- Parallel directory traversal for large trees
- Add capability to handle ACLs and extended attributes
