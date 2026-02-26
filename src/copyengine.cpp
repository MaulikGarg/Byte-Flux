#include "copyengine.h"

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

			if (readptr >= 0) // appropriate number of bytes have been read
				break;  

			if (errno == EINTR) // any interrupt that may have happened
				continue;

			throw_errno();
		}

		if (readptr == 0) // end of file is reached
			break;

		ssize_t bytes_written = 0;
		// write loop
		while (bytes_written < readptr) {
			ssize_t result = write(process.get_destination_fd(), buffer + bytes_written, readptr - bytes_written);

			if (result > 0) {
				bytes_written += result;
				continue;
			}

			if (result == -1 && errno == EINTR) // regular interrupt recieved
				continue;

			throw_errno();
		}
	}
	process.finalize(); // commit the changes
	process.cleanup(); 
}
