#include "validator.h"

#include "utility.h"

namespace fs = std::filesystem;

void resolve_source(IO_process& process) {
	if (stat(process.m_source.c_str(), &process.m_source_info) == -1)
		throw_errno("In resolve_source()");	 // bad source
}

void resolve_destination_file(IO_process& process) {
	bool dest_exists = (stat(process.m_destination.c_str(), &process.m_destination_info) == 0);
	std::string context = "In resolve_destination_files()";

	if (dest_exists) {
		bool dest_file_exists = true;																	// represents the acutal process destination file, not user path
		if (S_ISDIR(process.m_destination_info.st_mode)) {										// if the given path is a directory, correct it for a file
			process.m_destination = process.m_destination / process.m_source.filename();		// resolve actual name
			if (stat(process.m_destination.c_str(), &process.m_destination_info) == -1) {	// stat true destination
				dest_file_exists = false;																// destination file doesn't exist at all
				if (errno != ENOENT)																		// some other error other than file not existing
					throw_errno(context + ", destination exists and is dir, tried to stat canon destination file at: " + process.m_destination.c_str());
			}
		}
		if (dest_file_exists)																																						 // verify if source and destination refer to the same file
			if (process.m_source_info.st_dev == process.m_destination_info.st_dev && process.m_source_info.st_ino == process.m_destination_info.st_ino)	 // if both src and dst are the same
				throw_error("Source & Destination must be different.");
	}

	else {  // since destination path doesn't exist, check if parent is appropriate
		resolve_destination_parent(process.m_destination);
	}
}

void resolve_destination_directory_root(IO_process& process) {
	bool dest_exists = (stat(process.m_destination.c_str(), &process.m_destination_info) == 0);
	std::string context = "resolve_destination_directory_root";
	if (dest_exists) {
		// if destination is not a directory
		if (!S_ISDIR(process.m_destination_info.st_mode))
			throw_error("Destination is not a directory");
		process.m_destination = process.m_destination / process.m_source.filename();

		if (mkdir(process.m_destination.c_str(), process.m_source_info.st_mode & 0777) != 0)
			throw_errno(context + ", for mkdir1 at path: " + process.m_destination.c_str());
	} else {
		resolve_destination_parent(process.m_destination);
		// since directory parent is valid, safely create the actual destination directory
		if (mkdir(process.m_destination.c_str(), process.m_source_info.st_mode & 0777) != 0) {
			throw_errno(context + ", for mkdir2 at path: " + process.m_destination.c_str());
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
			throw_errno(context + ", parent path: " + parent.c_str());
	}
	if (!S_ISDIR(parent_info.st_mode))	// if parent exists but it is not a directory
		throw_error("Parent path is not a directory.");
}
