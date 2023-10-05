#include "Utils.hpp"

namespace utils {
	void	ParseArgs(int argc, char **argv) {
		if (argc != 3)
			throw std::invalid_argument("Invalid number of arguments \nUsage: ./ircserv <port> <password>");
		std::stringstream ss(argv[1]); //Use stringstream to convert string to int
		int port;
		ss >> port;
		if (port < 194 || port > 9999)
			throw std::invalid_argument("Invalid port number");
		std::string password = argv[2];
		if (password.length() < 1 /*do we check for strong password or not?*/)
			throw std::invalid_argument("Invalid password length");
	}
}	