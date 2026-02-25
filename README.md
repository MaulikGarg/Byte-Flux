# Byte Flux

A tool for performing various file operations, aimed to achieve a reliable and fast transfer speed.


## Why Byte Flux exists?

File operations using default file managers get very slow, or have unreliable behaviour upon cancelling.
Byte Flux aims to improves said speed by utilizing multiple threads for these operations compared to single thread
pipelines with default file managers, boosting speed significantly, especially when many small files are present.
The project builds file operations from the ground up providing such speed booosts.
This is a student project.


## Current Features

- Prevents accidental overwrites
- Copies a single file 

## Current TO-DO

- Add a temp-name before showing final file
- Preserve permissions better, not simply 0644
- Reject symlinks
- 


## Short Term Planned Features

- Directory copying
- File moving
- Directory moving
- Multiple threads on all operations
- Progress Tracking


## Build Instructions

```bash
Requirements:
- Linux
- CMake
- GCC/Clang

Build:
mkdir build
cd build
cmake ..
make

Run:
./byteflux
```

### About

This project is primarily a learning project exploring filesystem behavior, system calls, and performance characteristics of file operations.


