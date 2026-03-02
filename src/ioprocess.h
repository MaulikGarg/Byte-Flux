#ifndef IOPROCESS_H
#define IOPROCESS_H

#include <fcntl.h>
#include <unistd.h>

#include "utility.h"

// the primary class responsible for managing file descriptors
// and the Byte Flux <3
class IO_process {
	int m_source_fd {-1}; // source file descriptor
	int m_destination_fd {-1}; // destination file descriptor

	void cleanup();	  // closes all file descriptors

  public:
	std::filesystem::path m_source {}; // the source path of the current IO Process
	std::filesystem::path m_destination {}; // the destination path of the current IO Process
	std::filesystem::path m_temp {}; // the temporary file for this file 
	struct stat m_source_info {}; 
	struct stat m_destination_info {};

	~IO_process() { // primary destructor which calls cleanup() in order to clear FDs
		cleanup();
	}
	
	void open_files();  // opens source and temp file and set their descriptors
	void finalize(); // finalize the current operation with fsync, and renaming from temp
	int get_source_fd() const {
		return m_source_fd;
	}
	int get_destination_fd() const {
		return m_destination_fd;
	}
};

#endif
