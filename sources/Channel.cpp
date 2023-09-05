#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(std::string name, Client &creator, std::string password) : _name(name), _password(password), _creator(creator.GetUsername()), client_count(0) {
	this->addClient(creator);
	this->addOp(creator);
	this->invite_only = false;
	this->restrict_topic = false;
	this->has_password = false;
	this->has_clientlimit = false;
	this->client_limit = 0;
}

Channel::~Channel(void) {
}

void Channel::setName(std::string name) {
	this->_name = name;
}

void Channel::setTopic(std::string topic) {
	this->_topic = topic;
}

void Channel::setCreator(std::string creator) {
	this->_creator = creator;
}

void Channel::setModes(std::string modes) {
	this->_mode = modes;
}

std::string const	&Channel::getName(void) const {
	return (this->_name);
}

std::string const	&Channel::getTopic(void) const {
	return (this->_topic);
}

std::string const	&Channel::getCreator(void) const {
	return (this->_creator);
}

std::string const	&Channel::getModes(void) const {
	return (this->_mode);
}

void Channel::addClient(Client &client) {
	this->_clients.insert(std::pair<std::string, Client *>(client.GetUsername(), &client));
	this->client_count++;
}

void Channel::removeClient(Client &client) {
	this->_clients.erase(client.GetUsername());
	this->client_count--;
}

void Channel::addOp(Client &client) {
	this->op_clients.insert(client.GetUsername());
}

void Channel::removeOp(Client &client) {
	this->op_clients.erase(client.GetUsername());
}

bool Channel::isInviteOnly(void) const {
	return (this->invite_only);
}

bool Channel::isRestrictedTopic(void) const {
	return (this->restrict_topic);
}

bool Channel::hasPassword(void) const {
	return (this->has_password);
}

bool Channel::hasClientLimit(void) const {
	return (this->has_clientlimit);
}

bool Channel::isOp(Client &client) const {
	return (this->op_clients.find(client.GetUsername()) != this->op_clients.end());
}

bool Channel::isInvited(Client &client) const {
	return (this->invited_clients.find(client.GetUsername()) != this->invited_clients.end());
}

bool Channel::isClientInChannel(Client &client) const {
	return (this->_clients.find(client.GetUsername()) != this->_clients.end());
}

bool Channel::isFull(void) const {
	return (this->client_count >= this->client_limit);
}

void Channel::addMode(std::string mode) {
	if (mode == "i")
		this->invite_only = true;
	else if (mode == "t")
		this->restrict_topic = true;
	else if (mode == "k")
		this->has_password = true;
	else if (mode == "l")
		this->has_clientlimit = true;
}

void Channel::removeMode(std::string mode) {
	if (mode == "i")
		this->invite_only = false;
	else if (mode == "t")
		this->restrict_topic = false;
	else if (mode == "k")
		this->has_password = false;
	else if (mode == "l")
		this->has_clientlimit = false;
}

std::ostream&	operator<<(std::ostream& os, Channel& channel) {
	os << "Channel name: " << channel.getName() << std::endl;
	os << "Channel topic: " << channel.getTopic() << std::endl;
	os << "Channel creator: " << channel.getCreator() << std::endl;
	os << "Channel modes: " << channel.getModes() << std::endl;
	return (os);
}