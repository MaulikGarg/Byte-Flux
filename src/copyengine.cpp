#include "copyengine.h"

#include <cerrno>

#include "utility.h"

void copy_file_engine(IO_process& process) {
	process.open_files();

	char buffer[max_read_size];

	while (true) {
		ssize_t readptr;

		// read loop
		while (true) {
			readptr = read(process.get_source_fd(), buffer, max_read_size);

			if (readptr >= 0)
				break;  // eof

			if (errno == EINTR)
				continue;

			throw_errno();
		}

		if (readptr == 0)
			break;

		ssize_t bytes_written = 0;
		// write loop
		while (bytes_written < readptr) {
			ssize_t result = write(process.get_destination_fd(), buffer + bytes_written, readptr - bytes_written);

			if (result > 0) {
				bytes_written += result;
				continue;
			}

			if (result == -1 && errno == EINTR)
				continue;

			throw_errno();
		}
	}
	process.finalize();
	process.cleanup();
}
