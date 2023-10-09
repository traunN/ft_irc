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
			throw std::length_error("Invalid password length");
	}
	
	bool checkChannelName(std::string channel) {
		if (channel.length() > 50)
			throw std::length_error("Channel name too long");
		else if (channel[0] != '#')
			throw std::invalid_argument("Channel name must start with #");
		return true;
	}

	bool checkUserName(std::string username) {
		if (username.length() > 9)
			throw std::length_error("Username too long");
		return true;
	}
}