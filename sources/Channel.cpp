#include "Channel.hpp"

Channel::Channel(std::string name, std::string password) : _name(name), _password(password) {
	this->_isInviteOnly = false;
	this->_hasPassword = false;
	this->_hasUsersLimit = false;
	this->_userLimit = 0;
}

Channel::~Channel(void) {
}

std::string	Channel::GetName(void) {
	return (this->_name);
}

std::string	Channel::GetPassword(void) {
	return (this->_password);
}

std::vector<Client>	Channel::GetClients(void) {
	return (this->_clients);
}

void	Channel::SetName(std::string name) {
	this->_name = name;
}

void	Channel::SetPassword(std::string password) {
	this->_password = password;
}

void	Channel::SetClients(std::vector<Client> clients) {
	this->_clients = clients;
}

std::ostream&	operator<<(std::ostream& os, Channel& channel) {
	os << "Channel " << channel.GetName() << " created" << std::endl;
	return (os);
}
