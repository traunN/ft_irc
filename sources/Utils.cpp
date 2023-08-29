#include "Utils.hpp"

namespace utils {
	void	ParseArgs(int argc, char **argv) {
		std::stringstream ss(argv[1]); //Use stringstream to convert string to int
		int port;
		ss >> port;
		std::string password = argv[2];
		if (argc != 3)
			throw std::invalid_argument("Invalid number of arguments \nUsage: ./ircserv <port> <password>");
		if (port < 194 || port > 7000)
			throw std::invalid_argument("Invalid port number");
		if (password.length() < 1 /*do we check for strong password or not?*/)
			throw std::invalid_argument("Invalid password length");
	}
}