#include "validator.h"

#include <sys/stat.h>

#include <cerrno>

#include "utility.h"

namespace fs = std::filesystem;

void resolve_source(IO_process& process) {
	if (stat(process.source.c_str(), &process.source_info) == -1)
		throw_errno();
}

void resolve_destination_file(IO_process& process) {
	bool dest_exists = (stat(process.destination.c_str(), &process.destination_info) == 0);

	if (dest_exists) {
		bool dest_file_exists = false;
		if (S_ISDIR(process.destination_info.st_mode)) {  // if the given path is a directory, correct it for a file
			process.destination = process.destination / process.source.filename();
			if (stat(process.destination.c_str(), &process.destination_info) == 0)
				dest_file_exists = true;
			else if (errno != ENOENT)
				throw_errno();
		}
		if (dest_file_exists)
			if (process.source_info.st_dev == process.destination_info.st_dev && process.source_info.st_ino == process.destination_info.st_ino)	 // if both src and dst are the same
				throw_error("Source & Destination must be different.");
	}

	else {
		fs::path parent = process.destination.parent_path().empty() ? "." : process.destination.parent_path();  // get the appropriate parent name

		struct stat parent_stat;
		if (stat(parent.c_str(), &parent_stat) == -1) {
			if (errno == ENOENT)
				throw_error("Parent for destination doesn't exist.");
			else
				throw_errno();
		}

		if (!S_ISDIR(parent_stat.st_mode))
			throw_error("Parent path is not a directory.");
	}
}

void resolve_destination_directory_root(IO_process& process) {
}
