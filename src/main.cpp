#include <iostream>
#include "copyengine.h"
#include "ioprocess.h"

enum whichpath{
	source,
	destination
};

void get_path(IO_process& process, whichpath path){
	std::string input;
	if(path == source)
		std::cout << "Enter the source file path: ";
	if(path == destination)
		std::cout << "Enter the destination file path: ";

	getline(std::cin, input);
	auto start = input.find_first_not_of(" \t");
	auto end = input.find_last_not_of(" \t");
	input = (start == std::string::npos) ? "" : input.substr(start, end - start + 1);

	if(path == source){
		process.source = input;
		process.source = process.source.lexically_normal();
	}
	else if(path == destination){
		process.destination = input;
		process.destination = process.destination.lexically_normal();
	}
}

int main() {
	// the main process is the process which creates subprocesses if needed and
	// keeps everything maintained.
	IO_process mainprocess;
	try {
		std::cout << "File Zap 0.3\n";

		// get basic user input and validate if source is okay
		get_path(mainprocess, source);
		resolve_source(mainprocess);
		get_path(mainprocess, destination);

		// primary branching starts

		// if the source is a regular file
		if (S_ISREG(mainprocess.source_info.st_mode)) {
			resolve_destination_file(mainprocess);
			copy_file_engine(mainprocess);

			// if the source is a directory
		} else if (S_ISDIR(mainprocess.source_info.st_mode)) {
			resolve_destination_directory_root(mainprocess);
			copy_directory_engine(mainprocess);
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
