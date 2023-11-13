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