#include "Client.hpp"

Client::Client(void) {
}

Client::Client(int socket, std::string username, std::string password):  _socket(socket), _username(username), _password(password){
	_operator = false;
}

Client::Client(Client const &src) {
	*this = src;
}

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_username = rhs._username;
		this->_password = rhs._password;
		this->_socket = rhs._socket;
	}
	return (*this);
}

Client::~Client(void) {
}

void	Client::setUsername(std::string username) {
	this->_username = username;
}

void	Client::setPassword(std::string password) {
	this->_password = password;
}

std::string	Client::getUsername(void) {
	return (this->_username);
}

std::string Client::getPassword(void) {
	return (this->_password);
}

int	Client::getSocket(void) {
	return (this->_socket);
}

std::ostream&	operator<<(std::ostream& os, Client& client) {
	os << "Username: " << client.getUsername() << std::endl;
	return (os);
}