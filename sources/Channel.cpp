#include "../headers/Channel.hpp"
#include "../headers/Client.hpp"

Channel::Channel(std::string name, Client &creator, std::string password) {
	this->_name = name;
	this->_password = password;
	this->_creator = creator.getUsername();
	this->invite_only = false;
	this->restrict_topic = false;
	this->has_password = false;
	this->has_clientlimit = false;
	this->client_limit = 1024;
	this->client_count = 0;
	this->addClient(creator);
	this->addOp(creator);
	this->_topic = "";
	this->_mode = "";
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

void Channel::setClientLimit(size_t limit) {
	this->client_limit = limit;
}

void Channel::setPassword(std::string password) {
	this->_password = utils::hashPassword(password);
}

std::string const	&Channel::getName(void) const {
	return (this->_name);
}

std::string const	&Channel::getPassword(void) const {
	return (this->_password);
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

std::map<std::string, Client *> &Channel::getClients(void) {
	return (this->_clients);
}

size_t	Channel::getClientLimit(void) const {
	return (this->client_limit);
}

Client	*Channel::getClient(std::string nickname) {
	if (this->_clients.find(nickname) == this->_clients.end())
		throw std::runtime_error("Client not found");
	return (this->_clients[nickname]);
}

int Channel::addClient(Client &client) {
	if (isClientInChannel(client))
		return 1;
	else if (this->isFull())
		return 2;
	else if (this->invite_only && !isInvited(client))
		return 3;
	this->_clients.insert(std::pair<std::string, Client *>(client.getUsername(), &client));
	this->client_count++;
	return 0;
}

void Channel::removeClient(Client &client) {
	if (!isClientInChannel(client))
		return ;
	this->_clients.erase(client.getUsername());
	this->client_count--;
}

void Channel::addOp(Client &client) {
	if (!isClientInChannel(client))
		return ;
	else if (isOp(client))
		return ;
	this->op_clients.insert(client.getUsername());
}

void Channel::removeOp(Client &client) {
	if (!isClientInChannel(client))
		return ;
	else if (!isOp(client))
		return ;
	this->op_clients.erase(client.getUsername());
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
	if (this->op_clients.find(client.getUsername()) != this->op_clients.end())
		return (true);
	return (false);
}

bool Channel::isInvited(Client &client) const {
	if (this->invited_clients.find(client.getUsername()) != this->invited_clients.end())
		return (true);
	return (false);
}

bool Channel::isClientInChannel(Client &client) const {
	if (this->_clients.find(client.getUsername()) != this->_clients.end())
		return (true);
	return (false);
}

bool Channel::isFull(void) const {
	if (!this->hasClientLimit())
		return false;
	else if (this->client_count >= this->client_limit)
		return true;
	return false;
}

void Channel::addInvited(std::string username) {
	this->invited_clients.insert(username);
}

int Channel::addMode(std::string mode, std::string arg) {
	utils::trimWhitespace(arg);
	if (mode == "i") {
		this->invite_only = true;
		return 0;
	}
	else if (mode == "t") {
		this->restrict_topic = true;
		return 1;
	}
	else if (mode == "k") {
		this->has_password = true;
		this->setPassword(arg);
		return 2;
	}
	else if (mode == "l") {
		this->has_clientlimit = true;
		this->setClientLimit(utils::stringToInt(arg));
		return 3;
	}
	else if (mode == "o") {
		this->addOp(*getClient(arg));
		return 4;
	}
	else
		throw std::runtime_error("Invalid mode, use MODE <#channel> <+/-mode> (i : invite only, t: topic, k: password, o: give/take op, l: client limit)");
	return -1;
}

int Channel::removeMode(std::string mode) {
	if (mode == "i") {
		this->invite_only = false;
		return 0;
	}
	else if (mode == "t") {
		this->restrict_topic = false;
		return 1;
	}
	else if (mode == "k") {
		this->has_password = false;
		return 2;
	}
	else if (mode == "l") {
		this->has_clientlimit = false;
		return 3;
	}
	else
		throw std::runtime_error("Invalid mode, use MODE <#channel> <+/-mode> (i : invite only, t: topic, k: password, o: give/take op, l: client limit)");
	return -1;
}

std::ostream&	operator<<(std::ostream& os, Channel& channel) {
	os << "Channel name: " << channel.getName() << std::endl;
	os << "Channel topic: " << channel.getTopic() << std::endl;
	os << "Channel creator: " << channel.getCreator() << std::endl;
	os << "Channel modes: " << channel.getModes() << std::endl;
	return (os);
}