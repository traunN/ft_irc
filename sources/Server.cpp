#include "Server.hpp"

Server::Server(std::string name, std::string password) : _name(name), _password(password) {
	std::cout << "Server created" << std::endl;
}

Server::~Server(void) {
	std::cout << "Server destroyed" << std::endl;
}

void	Server::Run(void) {
	std::cout << "Server running" << std::endl;
}

std::string	Server::GetName(void) {
	return (this->_name);
}

std::string	Server::GetPassword(void) {
	return (this->_password);
}

std::vector<Channel>	Server::GetChannels(void) {
	return (this->_channels);
}

std::vector<Client>	Server::GetClients(void) {
	return (this->_clients);
}

void	Server::SetName(std::string name) {
	this->_name = name;
}

void	Server::SetPassword(std::string password) {
	this->_password = password;
}

void	Server::SetChannels(std::vector<Channel> channels) {
	this->_channels = channels;
}

void	Server::SetClients(std::vector<Client> clients) {
	this->_clients = clients;
}

void	Server::AddChannel(Channel channel) {
	this->_channels.push_back(channel);
}

void	Server::AddClient(Client client) {
	this->_clients.push_back(client);
}

void	Server::RemoveChannel(Channel channel) {
	std::vector<Channel>::iterator it = this->_channels.begin();
	while (it != this->_channels.end()) {
		if (it->GetName() == channel.GetName()) {
			this->_channels.erase(it);
			break ;
		}
		it++;
	}
}

void	Server::RemoveClient(Client client) {
	std::vector<Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end()) {
		if (it->GetNickname() == client.GetNickname()) {
			this->_clients.erase(it);
			break ;
		}
		it++;
	}
}

std::ostream&	operator<<(std::ostream& os, Server& server) {
	os << "Name: " << server.GetName() << std::endl;
	os << "Password: " << server.GetPassword() << std::endl;
	os << "Channels: " << std::endl;
	std::vector<Channel> channels = server.GetChannels();
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		os << *it << std::endl;
	}
	os << "Clients: " << std::endl;
	std::vector<Client> clients = server.GetClients();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		os << *it << std::endl;
	}
	return (os);
}