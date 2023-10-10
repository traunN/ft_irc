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
	std::cout << "Parsing message" << std::endl;
	if (strncmp(buffer, "JOIN ", 5) == 0)
		server.makeUserJoinChannel(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "LEAVE ", 6) == 0)
		server.makeUserLeaveChannel(std::string(buffer + 6), *this);
	else if (strncmp(buffer, "NICK ", 5) == 0)
		server.changeUsername(std::string(buffer + 5), *this);
	else if (strncmp(buffer, "KICK ", 5) == 0)
		server.kickUserFromChannel(std::string(buffer + 5), *this);
}

void Client::handleMessage(char* message, Server &server) {
	std::map<int, Client> clients = server.getClients();
	std::cout << this->getUsername() << ": " << message << std::endl;
	try {
		// parseMessage needs to be a method in the Client or Server class or a global function
		this->parseMessage(message, server);
	}
	catch (std::exception &e) {
		// returnError needs to be a method in the Server class
		server.returnError(this->getSocket(), e.what());
	}
	
	// Send the chat message to all other clients
	for (std::map<int, Client>::iterator client_it = clients.begin(); client_it != clients.end(); client_it++) {
		int other_client_socket = client_it->second.getSocket();
		if (other_client_socket != this->getSocket())
		{
			// Check if other client socket is in the same channel as the sender
			// sendMsgToSocket needs to be a method in the Server class
			server.sendMsgToSocket(other_client_socket, this->getUsername() + ": " + message + "\n");
		}
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

std::ostream&	operator<<(std::ostream& os, Client& client) {
	os << "Username: " << client.getUsername() << std::endl;
	return (os);
}