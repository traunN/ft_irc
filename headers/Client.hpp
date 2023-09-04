#pragma once

#include "Main.hpp"

class Client
{
	private:
		int _socket;
		std::string _username;
		std::string _password;
	public:
		Client(void);
		Client(std::string _username, std::string _password);
		Client(int socket, std::string _username, std::string _password);
		Client(Client const &src);
		virtual ~Client(void);

		Client	&operator=(Client const &rhs);
		
		// void	SetNickname(std::string nickname);
		void	SetUsername(std::string username);
		// void	SetRealname(std::string realname);
		void	SetPassword(std::string password);
		// void	SetHostname(std::string hostname);
		// std::string	GetNickname(void);
		std::string	GetUsername(void);
		int			GetSocket(void);
		// std::string	GetRealname(void);
		std::string	GetPassword(void);
		// std::string	GetHostname(void);
};

std::ostream&	operator<<(std::ostream& os, Client& client);
