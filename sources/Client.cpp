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

void Client::parseMessage(char *buffer, Server &server) {
	if (strncmp(buffer, "JOIN ", 5) == 0)
		server.makeUserJoinChannel(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "LEAVE ", 6) == 0)
		server.makeUserLeaveChannel(std::string(buffer + 6), *this);
	else if (strncmp(buffer, "NICK ", 5) == 0)
		server.changeUsername(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "KICK ", 5) == 0)
		server.kickUserFromChannel(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "PRIVMSG ", 8) == 0)
		server.sendMsgToUsers(std::string(buffer + 7), *this);
	else if (strncmp(buffer, "MODE ", 5) == 0)
		server.changeChannelMode(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "INVITE ", 7) == 0)
		server.inviteUserToChannel(std::string(buffer + 7), *this);
	//else if (strncmp(buffer, "TOPIC ", 6) == 0)
	else 
		throw std::runtime_error("Invalid command");
}

void Client::handleMessage(char* message, Server &server) {
	std::map<int, Client> clients = server.getClients();
	// std::cout << this->getUsername() << ": " << message << std::endl;
	try {
		// parseMessage needs to be a method in the Client or Server class or a global function
		this->parseMessage(message, server);
	}
	catch (std::exception &e) {
		// returnError needs to be a method in the Server class
		server.returnError(this->getSocket(), e.what());
	}
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

bool	Client::getOperator(void) {
	return (this->_operator);
}

bool	Client::getIsSic(void) {
	return (this->_is_sic);
}

void	Client::setOperator(bool op) {
	this->_operator = op;
}

void	Client::setIsSic(bool is_sic) {
	this->_is_sic = is_sic;
}

std::ostream&	operator<<(std::ostream& os, Client& client) {
	os << "Username: " << client.getUsername() << std::endl;
	return (os);
}