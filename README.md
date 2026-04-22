# dfpp

**A pretty disk usage CLI utility for Linux, inspired by [duf](https://github.com/muesli/duf), written in C++23.**

```
╭────────────┬────────────┬───────────┬───────────┬────────────────────┬─────────┬────────────────╮
│ 3 local devices                                                                                 │
├────────────┬────────────┬───────────┬───────────┬────────────────────┬─────────┬────────────────┤
│ MOUNTED ON │      TOTAL │      USED │     AVAIL │        USE%        │ TYPE    │ DEVICE         │
├────────────┼────────────┼───────────┼───────────┼────────────────────┼─────────┼────────────────┤
│ /          │  184.9 GiB │ 110.1 GiB │  74.8 GiB │ ███████░░░░░ 59.5% │ ext4    │ /dev/nvme0n1p4 │
│ /boot      │ 1022.0 MiB │  63.9 MiB │ 958.1 MiB │ ▌░░░░░░░░░░░  6.3% │ vfat    │ /dev/nvme0n1p6 │
│ /data      │  931.5 GiB │ 477.3 GiB │ 454.2 GiB │ ██████░░░░░░ 51.2% │ fuseblk │ /dev/nvme1n1p2 │
╰────────────┴────────────┴───────────┴───────────┴────────────────────┴─────────┴────────────────╯
```

## Features

- Human-readable sizes (B / KiB / MiB / GiB / TiB)
- Unicode progress bars (`█▌░`) with color-coded usage levels
- 256-color ANSI — consistent look across terminal themes
- Box-drawn table with auto-sized columns
- Automatic filtering of pseudo-filesystems and duplicate mounts
- Directory arguments with longest-prefix mount matching
- No external dependencies — pure C++ standard library and POSIX

## Build

```bash
make
```

Requires GCC with C++23 support (GCC 14+).

## Usage

```bash
./dfpp                  # all real filesystems
./dfpp /data .          # only the filesystems owning these paths
./dfpp ~/projects       # relative and absolute paths both work
```

## How it works

Reads `/proc/mounts`, calls `statvfs()` on each real block device, and renders the results in a single formatted table.

```
/proc/mounts → parse → filter → statvfs() → format → render
```

## Project structure

```
types.hpp        FileSystemInfo data model
fs_info.hpp/cpp  mount parsing, statvfs, filtering, path matching
format.hpp/cpp   size formatting, color codes, progress bars
table.hpp/cpp    box-drawn table rendering
main.cpp         CLI driver
```

## TODO

- [ ] CLI flags for size units, showing all filesystems, disabling color, JSON output
- [ ] Sorting by size, usage, name
- [ ] macOS support

## Linux only (for now)

`dfpp` reads `/proc/mounts`, which is Linux-specific.
