#include "Bot.hpp"

Bot::Bot(std::string host, std::string password) {
	this->_host = host;
	this->_port = 0;
	this->_password = password;
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
}

Bot::~Bot(void) {
	close(this->_socket);
}

void Bot::Run(void) {
	struct sockaddr_in myaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(this->_port);
	if (inet_pton(AF_INET, this->_host.c_str(), &myaddr.sin_addr.s_addr) <= 0) {
		throw std::runtime_error("inet_pton() failed");
	}
	if (connect(this->_socket, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
		throw std::runtime_error("connect() failed");
	}
	std::cout << "Connected to server " << this->_host << ":" << this->_port << std::endl;
	// create specific msg to send to server so it can read it like : this->_message[0] == 'B' && this->_message[1] == 'O' && this->_message[2] == 'T' && this->_message[3] == '\0'
	send(this->_socket, "BOT", 4, 0);
	while (1) {

		continue;
	}
}

void	Bot::ParseArgs(int argc, char **argv) {
	if (argc != 4)
		throw std::runtime_error("Usage: ./ircbot <host> <port> <password>");
	std::stringstream ss(argv[2]); //Use stringstream to convert string to int
	ss >> this->_port;
	if (this->_port < 194 || this->_port > 9999)
		throw std::invalid_argument("Invalid port number");
	std::string password = argv[3];
	if (password.length() < 1 /*do we check for strong password or not?*/)
		throw std::length_error("Invalid password length");
}