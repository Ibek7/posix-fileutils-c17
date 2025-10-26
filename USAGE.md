# Usage Guide

## stat — File metadata inspector

Print detailed file information using the stat() system call.

```bash
./stat file.txt
./stat /usr/bin/ls
./stat . ..  # multiple files
```

Output includes: size, blocks, link count, inode, device, permissions, owner, group, modification time.

## myls — Directory lister

List directory contents with optional long format.

```bash
./myls              # lists current directory
./myls .            # explicit current directory
./myls /tmp         # lists /tmp
./myls -l           # long format of current directory
./myls -l /var/log  # long format of specific directory
```

Long format (-l) displays:
- Permissions (drwxr-xr-x)
- Link count
- Owner and group
- File size
- Modification time
- Filename
- Symlink target (if applicable)

## mytail — Print last N lines

Efficiently read and print the last N lines of a file.

```bash
./mytail 10 file.txt      # last 10 lines
./mytail 5 /var/log/system.log
./mytail 1 script.sh      # last line only
```

The program seeks to the end of the file and reads backward for efficiency.

## myfind — Recursive directory tree walker

Print all files and directories in a tree recursively.

```bash
./myfind            # from current directory
./myfind .          # explicit current directory
./myfind /home      # from /home downward
./myfind /usr/local/bin
```

Prints a depth-first traversal of the entire directory tree.
