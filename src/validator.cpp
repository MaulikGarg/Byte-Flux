#include "validator.h"

#include "utility.h"

namespace fs = std::filesystem;

void resolve_source(IO_process& process) {
	if (stat(process.source.c_str(), &process.source_info) == -1)
		throw_errno("In resolve_source()");	 // bad source
}

void resolve_destination_file(IO_process& process) {
	bool dest_exists = (stat(process.destination.c_str(), &process.destination_info) == 0);
	std::string context = "In resolve_destination_files()";

	if (dest_exists) {
		bool dest_file_exists = true;																	// represents the acutal process destination file, not user path
		if (S_ISDIR(process.destination_info.st_mode)) {										// if the given path is a directory, correct it for a file
			process.destination = process.destination / process.source.filename();		// resolve actual name
			if (stat(process.destination.c_str(), &process.destination_info) == -1) {	// stat true destination
				dest_file_exists = false;																// destination file doesn't exist at all
				if (errno != ENOENT)																		// some other error other than file not existing
					throw_errno(context);
			}
		}
		if (dest_file_exists)																																						 // verify if source and destination refer to the same file
			if (process.source_info.st_dev == process.destination_info.st_dev && process.source_info.st_ino == process.destination_info.st_ino)	 // if both src and dst are the same
				throw_error("Source & Destination must be different.");
	}

	else {  // since destination path doesn't exist, check if parent is appropriate
		resolve_destination_parent(process.destination);
	}
}

void resolve_destination_directory_root(IO_process& process) {
	bool dest_exists = (stat(process.destination.c_str(), &process.destination_info) == 0);
	std::string context = "resolve_destination_directory_root";
	if (dest_exists) {
		// if destination is not a directory
		if (!S_ISDIR(process.destination_info.st_mode))
			throw_error("Destination is not a directory");
		process.destination = process.destination / process.source.filename();

		if (mkdir(process.destination.c_str(), process.source_info.st_mode & 0777) != 0)
			throw_errno();
	} else {
		resolve_destination_parent(process.destination);
		// since directory parent is valid, safely create the actual destination directory
		if (mkdir(process.destination.c_str(), process.source_info.st_mode & 0777) != 0) {
			throw_errno(context);
		};
	}
}

void resolve_destination_parent(std::filesystem::path& destination) {
	fs::path parent = destination.parent_path().empty() ? "." : destination.parent_path();	 // get the appropriate parent name
	std::string context = "In resolve_destination_parent()";
	struct stat parent_info;
	if (stat(parent.c_str(), &parent_info) == -1) {
		if (errno == ENOENT)	 // even parent doesn't exist
			throw_error("Parent for destination doesn't exist.");
		else
			throw_errno(context);
	}
	if (!S_ISDIR(parent_info.st_mode))	// if parent exists but it is not a directory
		throw_error("Parent path is not a directory.");
}
