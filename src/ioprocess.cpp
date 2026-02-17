#include "ioprocess.h"

void IO_process::cleanup() {
	if (source_fd >= 0) {
		close(source_fd);
		source_fd = -1;
	}
	if (destination_fd >= 0) {
		if (fsync(destination_fd) < 0) {
			close(destination_fd);
			throw_errno();
		}
		close(destination_fd);
		destination_fd = -1;
	}
}

void IO_process::open_files() {
	source_fd = open(source.c_str(), O_RDONLY);
	if (source_fd < 0)
		throw_errno();

	destination_fd = open(destination.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (destination_fd < 0) {
		close(source_fd);
		source_fd = -1;
		throw_errno();
	}
}
