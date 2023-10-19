#pragma once

#include "Main.hpp"

class Server;

class Client
{
	private:
		int		_socket;
		std::string _nickname;
		std::string _username;
		std::string _password;
		bool	_operator;
		bool	_is_sic;
	public:
		Client(void);
		Client(int socket, std::string nickname, std::string username, std::string password);
		Client(Client const &src);
		virtual ~Client(void);

		Client	&operator=(Client const &rhs);
		
		void	setUsername(std::string username);
		void	setPassword(std::string password);
		void	setNickname(std::string nickname);
		std::string	getUsername(void);
		std::string	getNickname(void);
		int			getSocket(void);
		std::string	getPassword(void);
		bool	getOperator(void);
		bool	getIsSic(void);
		void	setOperator(bool op);
		void	setIsSic(bool is_sic);

		void	handleMessage(std::string message, Server &server);
		void	parseMessage(std::string message, Server &server);
};

std::ostream&	operator<<(std::ostream& os, Client& client);
