#include "Client.hpp"

Client::Client(void) {
}

Client::Client(int socket, std::string nickname, std::string username, std::string password):  _socket(socket), _nickname(nickname), _username(username), _password(password){
	_operator = false;
	_is_sic = false;
}

Client::Client(Client const &src) {
	*this = src;
}

Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_password = rhs._password;
		this->_socket = rhs._socket;
		this->_operator = rhs._operator;
		this->_is_sic = rhs._is_sic;
	}
	return (*this);
}

void Client::parseMessage(std::string message, Server &server) {
	// Split the message into tokens for command and arguments
	std::istringstream iss(message);
	std::string command;
	iss >> command;

	if (command == "JOIN") {
		server.makeUserJoinChannel(message, *this);
	} else if (command == "PART") {
		std::string channel;
		iss >> channel;
		server.makeUserLeaveChannel(channel, *this);
	} else if (command == "NICK") {
		std::string nickname;
		iss >> nickname;
		server.changeNickname(nickname, *this);
	} else if (command == "KICK") {
		server.kickUserFromChannel(message, *this);
	} else if (command == "PRIVMSG") {
		server.handleMessage(message, *this);
	} else if (command == "MODE") {
		server.changeChannelMode(message, *this);
	} else if (command == "INVITE") {
		server.inviteUserToChannel(message, *this);
	} else if (command == "TOPIC") {
		server.changeChannelTopic(message, *this);
	} else if (command == "DEBUG") {
		server.debug();
	} else {
		throw std::runtime_error("Invalid command");
	}
}


void Client::handleMessage(std::string message, Server &server) {
	std::map<int, Client> clients = server.getClients();
	try {
		// parseMessage needs to be a method in the Client or Server class or a global function
		this->parseMessage(message, server);
	}
	catch (std::exception &e) {
		// returnError needs to be a method in the Server class
		server.sendMsgToSocket(this->getSocket(), e.what());
	}
}

Client::~Client(void) {
}


void	Client::setUsername(std::string username) {
	this->_username = username;
}

void	Client::setNickname(std::string nickname) {
	this->_nickname = nickname;
}

void	Client::setPassword(std::string password) {
	this->_password = password;
}

std::string	Client::getUsername(void) {
	return (this->_username);
}

std::string	Client::getNickname(void) {
	return (this->_nickname);
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
	os << "Nickname: " << client.getNickname() << std::endl;
	os << "Password: " << client.getPassword() << std::endl;
	os << "is sic ? : " << client.getIsSic() << std::endl;
	return (os);
}