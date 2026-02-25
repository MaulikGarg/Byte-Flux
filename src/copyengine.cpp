#include "copyengine.h"
#include <cerrno>


void copy_file_engine(IO_process& process) {
	process.open_files();

	char buffer[max_read_size];

	while (true) {
		ssize_t readptr = read(process.get_source_fd(), buffer, max_read_size);
		if (readptr == 0)
			break;  // eof
		if (readptr == -1 && errno != EINTR)
			throw_errno();

		ssize_t bytes_written = 0;
		ssize_t bytes_left = readptr;
		while (bytes_left) {
			ssize_t result = write(process.get_destination_fd(), buffer + bytes_written, bytes_left);
			if (result == -1 && errno != EINTR)
				throw_errno();
			bytes_written += result;
			bytes_left -= result;
		}
	}
	process.cleanup();
}
