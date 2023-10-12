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
		bool	_is_sic;
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
		bool	getOperator(void);
		bool	getIsSic(void);
		void	setOperator(bool op);
		void	setIsSic(bool is_sic);

		void	handleMessage(char* message, Server &server);
		void	parseMessage(char *buffer, Server &server);
};

std::ostream&	operator<<(std::ostream& os, Client& client);
