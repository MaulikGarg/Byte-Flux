#ifndef UTILS_H
#define UTILS_H
#include <cerrno>
#include <cstring>
#include <stdexcept>

inline void throw_error(const std::string& msg) {
	throw std::runtime_error(msg);
}

inline void throw_errno(const std::string& context = "") {
	std::string msg = context.empty() ? std::strerror(errno) : context + ": " + std::strerror(errno);
	throw std::runtime_error(msg);
}

#endif
