#ifndef COPY_ENGINE_H
#define COPY_ENGINE_H

#include <unistd.h>	//for close, write

#include <cerrno>	 //for errno
#include <cstring>

#include "ioprocess.h"
#include "utility.h"
#include "validator.h"
#include <filesystem>

constexpr int max_read_size = 1'048'576;	// 1mb i/o size

// copies a single file, handles the file descriptors
void copy_file_engine(IO_process& process);

// copies an entire directory
// the destination directory root must exist
void copy_directory_engine(IO_process& process);
#endif
