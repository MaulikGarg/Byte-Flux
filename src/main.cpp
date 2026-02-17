#include <filesystem>
#include <iostream>
#include <string>

#include "copyengine.h"
#include "validator.h"
#include "ioprocess.h"

namespace fs = std::filesystem;

void get_source(fs::path& source) {
	std::string source_inp;
	std::cout << "Enter the source file path: ";
	getline(std::cin, source_inp);
	source = source_inp;
}

void get_destination(fs::path& destination) {
	std::string destination_inp;
	std::cout << "Enter the destination file path: ";
	getline(std::cin, destination_inp);
	destination = destination_inp;
}

int main() {
	IO_process mainprocess; // every operation is a process in itself
	try {

		std::cout << "File Zap 0.01\n";

		get_source(mainprocess.source);
		resolve_source(mainprocess);
		get_destination(mainprocess.destination);

		if (fs::is_regular_file(mainprocess.source_info)) {
			resolve_destination_file(mainprocess);
			copy_file_engine(mainprocess);
		} else if (fs::is_directory(mainprocess.source_info)) {
			resolve_destination_directory_root(mainprocess);
		} else
			throw_error("Unsupported format.");

		std::cout << "File Copy success. Exiting...\n";
		return 0;
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}
