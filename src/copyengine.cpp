#include "copyengine.h"

#include <unistd.h>
#include <cerrno>
#include "utility.h"

namespace fs = std::filesystem;
// the primary copy engine for copying a single file around
// the IO_Process sent must have its source and destination values
// as individual files,
// ! this function does not validate these addresses.
void copy_file_engine(IO_process& process) {
	std::string context = "In copy_file_engine()";
	process.open_files();

	// file offset in the source file
	off_t src = 0;
	// file offset in the destination file
	off_t dst = 0;

	ssize_t remaining = process.m_source_info.st_size;

	while (remaining > 0) {
		ssize_t copied = copy_file_range(process.get_source_fd(), &src, process.get_destination_fd(), &dst, remaining, 0);
		if(copied < 0){
			if(errno == EINTR) continue;
			else throw_errno(context);
		}
		remaining -= copied;
	}
	process.finalize();	// commit the changes
}

// the primary copy engine for copying directories around.
// the IO_Process sent must have directories as source and destination.
// ! this function does not validate sent addresses.
void copy_directory_engine(IO_process& process, ThreadPool& pool) {
	std::string context = "In copy_directory_engine()";

	// iterate through all the directories and select current object as "src"
	for (const fs::directory_entry& src : fs::directory_iterator(process.m_source)) {
		// the current src's IO Process
		IO_process current;
		current.m_source = src.path();
		// real destination for current src
		current.m_destination = process.m_destination / src.path().filename();
		// stat the source to obtain permissions
		if (stat(current.m_source.c_str(), &current.m_source_info) == -1)
			throw_errno(context + " , stat on: " + current.m_source.c_str());

		// if src is a file, copy using file engine
		if (src.is_regular_file()) {
			pool.add_process(current);
			// if src is a directory, make that directory at destination then copy it recursively
		} else if (src.is_directory()) {
			if (mkdir(current.m_destination.c_str(), current.m_source_info.st_mode & 0777) != 0)
				throw_errno(context + ", mkdir on: " + current.m_destination.c_str());
			// copies the directory's contents
			copy_directory_engine(current, pool);
		} else if (src.is_symlink()) {
			// if its a symlink, simply skip
			std::cerr << "Skipping symlink: " << src.path() << '\n';
		}
	}
}
