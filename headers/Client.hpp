#pragma once

#include "Main.hpp"

class Server;

class Client
{
	private:
		int		_socket;
		std::string _username;
		std::string _password;
		bool	_operator;
	public:
		Client(void);
		Client(int socket, std::string _username, std::string _password);
		Client(Client const &src);
		virtual ~Client(void);

		Client	&operator=(Client const &rhs);
		
		void	setUsername(std::string username);
		void	setPassword(std::string password);
		std::string	getUsername(void);
		int			getSocket(void);
		std::string	getPassword(void);
};

std::ostream&	operator<<(std::ostream& os, Client& client);
