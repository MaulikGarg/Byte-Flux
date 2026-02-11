#include <fcntl.h>	//for open
#include <unistd.h>	//for close, write

#include <cerrno>	 //for errno
#include <cstring>
#include <iostream>
#include <string>

using std::cout, std::string;

constexpr int permbitmask = 0644;	 // default perms
constexpr int max_read_size = 4096;	 // 4kb i/o size

int main() {
	string source;
	cout << "File Zap 0.01\nEnter the source file path: ";
	std::cin >> source;

	auto validatefd = [](int fd) {
		if (fd < 0)
			throw std::runtime_error(std::strerror(errno));
	};

	int source_fd = open(source.c_str(), O_RDONLY);
	validatefd(source_fd);

	string destination;
	cout << "Enter the destination file path: ";
	std::cin >> destination;

	int dest_fd = open(destination.c_str(), O_CREAT | O_TRUNC | O_WRONLY, permbitmask);
	validatefd(dest_fd);

	char buffer[max_read_size];

	while (true) {
		ssize_t readptr = read(source_fd, buffer, max_read_size);
		if (readptr == 0)
			break;  // eof
		if (readptr == -1)
			throw std::runtime_error(std::strerror(errno));

		ssize_t bytes_written = 0;
		ssize_t bytes_left = readptr;
		while (bytes_left) {
			ssize_t result = write(dest_fd, buffer + bytes_written, bytes_left);
			if (result == -1)
				throw std::runtime_error(std::strerror(errno));
			bytes_written += result;
			bytes_left -= result;
		}
	}

	fsync(dest_fd);
	close(source_fd);
	close(dest_fd);

	cout << "File Copy success. Exiting...";
	return 0;
}
