#include "Client.hpp"

Client::Client(void) {
}

Client::Client(std::string username, std::string password): _username(username), _password(password){
}

Client::Client(int socket, std::string username, std::string password): _username(username), _password(password){
	this->_socket = socket;
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

// void	Client::SetNickname(std::string nickname) {
// 	this->_nickname = nickname;
// }

void	Client::SetUsername(std::string username) {
	this->_username = username;
}

// void	Client::SetRealname(std::string realname) {
// 	this->_realname = realname;
// }

void	Client::SetPassword(std::string password) {
	this->_password = password;
}

// void	Client::SetHostname(std::string hostname) {
// 	this->_hostname = hostname;
// }

// std::string	Client::GetNickname(void) {
// 	return (this->_nickname);
// }

std::string	Client::GetUsername(void) {
	return (this->_username);
}

// std::string	Client::GetRealname(void) {
// 	return (this->_realname);
// }

std::string Client::GetPassword(void) {
	return (this->_password);
}

int	Client::GetSocket(void) {
	return (this->_socket);
}

// std::string	Client::GetHostname(void) {
// 	return (this->_hostname);
// }

std::ostream&	operator<<(std::ostream& os, Client& client) {
	// os << "Nickname: " << client.GetNickname() << std::endl;
	os << "Username: " << client.GetUsername() << std::endl;
	// os << "Realname: " << client.GetRealname() << std::endl;
	// os << "Password: " << client.GetPassword() << std::endl;
	// os << "Hostname: " << client.GetHostname() << std::endl;
	return (os);
}