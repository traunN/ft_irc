#include "Channel.hpp"

Channel::Channel(std::string name, std::string password) : _name(name), _password(password) {
	this->_isInviteOnly = false;
	this->_hasPassword = false;
	this->_hasUsersLimit = false;
	this->_userLimit = 0;
	this->_usersCount = 0;
	this->_clients = std::vector<Client>();
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

void	Channel::AddClient(Client client) {
	this->_clients.push_back(client);
	this->_usersCount++;
}

Client	&Channel::operator[](unsigned int index)
{
	if (_clients.size() == 0)
		throw std::runtime_error("Array is empty");
	if (index >= _clients.size())
		throw std::runtime_error("Array is full");
	return (_clients[index]);
}


std::ostream&	operator<<(std::ostream& os, Channel& channel) {
	os << "Channel " << channel.GetName() << " created\n";
	for (int i = 0; i < channel.GetUsersCount(); i++) {
		os << "Client " << channel.GetClients()[i].GetUsername() << " joined\n";
	}
	return (os);
}
