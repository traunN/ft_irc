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
	std::string gen_random(const int len) {
		srand(time(0));
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		std::string tmp_s;
		tmp_s.reserve(len);

		for (int i = 0; i < len; ++i) {
			tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
		}
		return tmp_s;
	}
}	