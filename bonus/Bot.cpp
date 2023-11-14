#include "Bot.hpp"

Bot::Bot(std::string host, std::string port, std::string password) {
	this->_host = host;
	this->_port = port;
	this->_password = password;
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
}

Bot::~Bot(void) {
	close(this->_socket);
}

void Bot::run(void) {
	struct sockaddr_in myaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(8080);
	if (inet_pton(AF_INET, this->_host.c_str(), &myaddr.sin_addr.s_addr) <= 0) {
		throw std::runtime_error("inet_pton() failed");
	}
	if (connect(this->_socket, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
		throw std::runtime_error("connect() failed");
	}
	std::cout << "Connecting to " << this->_host << ":" << this->_port << std::endl;
	// std::string pass = "PASS 1\r\n";
	// std::string nick = "NICK bot\r\n";
	// std::string user = "USER bot\r\n";
	// usleep(500);
	// send(this->_socket, pass.c_str(), pass.length(), 0);
	// usleep(500);
	// send(this->_socket, nick.c_str(), nick.length(), 0);
	// usleep(500);
	// send(this->_socket, user.c_str(), user.length(), 0);
	// usleep(500);
	while (1) {
		continue;
	}
}

namespace utils {
	void	ParseArgs(int argc, char **argv) {
		if (argc != 4)
			throw std::runtime_error("Usage: ./ircbot <host> <port> <password>");
		std::stringstream ss(argv[1]); //Use stringstream to convert string to int
		int port;
		ss >> port;
		if (port < 194 || port > 9999)
			throw std::invalid_argument("Invalid port number");
		std::string password = argv[2];
		if (password.length() < 1 /*do we check for strong password or not?*/)
			throw std::length_error("Invalid password length");
	}
}
