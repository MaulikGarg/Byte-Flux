#include <sys/stat.h>
#include <filesystem>
#include <iostream>
#include <string>

#include "copyengine.h"
#include "ioprocess.h"


namespace fs = std::filesystem;

// the source prompt
void get_source(fs::path& source) {
	std::string source_inp;
	std::cout << "Enter the source file path: ";
	getline(std::cin, source_inp);
	source = source_inp;
}

// the destination prompt
void get_destination(fs::path& destination) {
	std::string destination_inp;
	std::cout << "Enter the destination file path: ";
	getline(std::cin, destination_inp);
	destination = destination_inp;
}

int main() {
	// the main process is the process which creates subprocesses if needed and
	// keeps everything maintained.
	IO_process mainprocess; 
	try {
		std::cout << "File Zap 0.02\n";

		// get basic user input and validate if source is okay
		get_source(mainprocess.source);
		resolve_source(mainprocess);
		get_destination(mainprocess.destination);

		// primary branching starts

		// if the source is a regular file
		if (S_ISREG(mainprocess.source_info.st_mode)) {
			resolve_destination_file(mainprocess);
			copy_file_engine(mainprocess);

		// if the source is a directory	
		} else if (S_ISDIR(mainprocess.source_info.st_mode)) {
			resolve_destination_directory_root(mainprocess);
		}
		
		// if the source is not a file or a directory
		else
			throw_error("Unsupported format.");

		std::cout << "File Copy success. Exiting...\n";
		return 0;
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}
