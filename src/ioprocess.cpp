#include "ioprocess.h"

#include <fcntl.h>

// closes both source and destination file descriptors of an 
// IO_process
void IO_process::cleanup() {
	if (source_fd >= 0) {
		close(source_fd);
		source_fd = -1;
	}
	if (destination_fd >= 0) {
		close(destination_fd);
		destination_fd = -1;
	}
}

// commits any changes made in the current IO_process
// calls cleanup() internally
void IO_process::finalize() {
	if (destination_fd >= 0) {
		if (fsync(destination_fd) < 0) {
			close(destination_fd);
			destination_fd = -1;
			throw_errno();
		}
	}
	cleanup();
}

// responsible for opening the files of the IO process
// ! This function uses O_TRUNC and therefore will overwrite any present destination file
void IO_process::open_files() {
	source_fd = open(source.c_str(), O_RDONLY);
	if (source_fd < 0)
		throw_errno();

	destination_fd = open(destination.c_str(), O_CREAT | O_TRUNC | O_WRONLY, source_info.st_mode & 0777);
	if (destination_fd < 0) {
		throw_errno();
	}
}
