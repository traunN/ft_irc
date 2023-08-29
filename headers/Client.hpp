#pragma once

#include <iostream>
#include <exception>
#include <string>

class Client
{
	private:
		std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _password;
		std::string _hostname;
	public:
		Client(void);
		virtual ~Client(void);
		void	SetNickname(std::string nickname);
		void	SetUsername(std::string username);
		void	SetRealname(std::string realname);
		void	SetPassword(std::string password);
		void	SetHostname(std::string hostname);
		std::string	GetNickname(void);
		std::string	GetUsername(void);
		std::string	GetRealname(void);
		std::string	GetPassword(void);
		std::string	GetHostname(void);
};

std::ostream&	operator<<(std::ostream& os, Client& client);
