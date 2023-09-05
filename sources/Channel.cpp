#include "Channel.hpp"

Channel::Channel(std::string name, std::string password) : _name(name), _password(password) {
	this->_isInviteOnly = false;
	this->_hasPassword = false;
	this->_hasUsersLimit = false;
	this->_userLimit = 0;
	this->_usersCount = 0;
	this->_clients = std::map<int, Client>();
}

Channel::~Channel(void) {
}

std::string	Channel::GetName(void) {
	return (this->_name);
}

int	Channel::GetUsersCount(void) {
	return (this->_usersCount);
}

std::string	Channel::GetPassword(void) {
	return (this->_password);
}

std::map<int, Client>	Channel::GetClients(void) {
	return (this->_clients);
}

void	Channel::SetName(std::string name) {
	this->_name = name;
}

void	Channel::SetPassword(std::string password) {
	this->_password = password;
}

void	Channel::AddClient(Client client) {
	this->_clients.insert(std::pair<int, Client>(client.GetSocket(), client));
	this->_usersCount++;
}

Client	&Channel::operator[](unsigned int index) {
	std::map<int, Client>::iterator it = this->_clients.begin();
	for (unsigned int i = 0; i < index; i++) {
		it++;
	}
	return (it->second);
}

std::ostream&	operator<<(std::ostream& os, Channel& channel) {
	os << "Channel: " << channel.GetName() << std::endl;
	os << "Password: " << channel.GetPassword() << std::endl;
	os << "Users count: " << channel.GetUsersCount() << std::endl;
	return (os);
}