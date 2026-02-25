#ifndef IOPROCESS_H
#define IOPROCESS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <filesystem>

#include "utility.h"

class IO_process {
	int source_fd = -1;
	int destination_fd = -1;

  public:
	std::filesystem::path source;
	std::filesystem::path destination;
	struct stat source_info;
	struct stat destination_info;

	~IO_process() {
		cleanup();
	}
	void open_files();  // opens both files and sets file descriptors
	void finalize(); // finalize the current operation with fsync
	void cleanup();	  // closes all file descriptors
	int get_source_fd() const {
		return source_fd;
	}
	int get_destination_fd() const {
		return destination_fd;
	}
};

#endif
