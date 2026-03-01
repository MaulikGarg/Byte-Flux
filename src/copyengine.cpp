#include "copyengine.h"
#include "ioprocess.h"
#include "utility.h"
namespace fs = std::filesystem;
// the primary copy engine for copying a single file around
// the IO_Process sent must have its source and destination values
// as individual files,
// ! this function does not validate these addresses.
void copy_file_engine(IO_process& process) {
	process.open_files();

	char buffer[max_read_size];

	while (true) {
		ssize_t readptr;

		// read loop
		while (true) {
			readptr = read(process.get_source_fd(), buffer, max_read_size);

			if (readptr >= 0)	 // appropriate number of bytes have been read
				break;

			if (errno == EINTR)	// any interrupt that may have happened
				continue;

			throw_errno();
		}

		if (readptr == 0)	 // end of file is reached
			break;

		ssize_t bytes_written = 0;
		// write loop
		while (bytes_written < readptr) {
			ssize_t result = write(process.get_destination_fd(), buffer + bytes_written, readptr - bytes_written);

			if (result > 0) {
				bytes_written += result;
				continue;
			}

			if (result == -1 && errno == EINTR)	 // regular interrupt recieved
				continue;

			throw_errno();
		}
	}
	process.finalize();	// commit the changes
}

// the primary copy engine for copying directories around.
// the IO_Process sent must have directories as source and destination.
// ! this function does not validate sent addresses.
void copy_directory_engine(IO_process& process) {
	std::string context = "In copy_directory_engine()";
	for (const fs::directory_entry& src : fs::directory_iterator(process.source)) {
		IO_process current;
		current.source = src.path();
		current.destination = process.destination / src.path().filename();
		if (stat(current.source.c_str(), &current.source_info) == -1)
			throw_errno(context + " , stat on: " + current.source.c_str());
		if (src.is_regular_file()) {
			copy_file_engine(current);
		} else if (src.is_directory()) {
			if (mkdir(current.destination.c_str(), current.source_info.st_mode & 0777) != 0)
				throw_errno(context + ", mkdir on: " + current.destination.c_str());
			copy_directory_engine(current);
		}
	}
}
