#pragma once

#include "Main.hpp"

class Client;

class Channel {
    private:
        bool _isInviteOnly;
        bool _hasPassword;
        bool _hasUsersLimit;

        size_t _userLimit;
		int			_usersCount;
		std::string _name;
		std::string _password;
		std::vector<Client> _clients;
    public:
        Channel(std::string name, std::string password);
        virtual ~Channel(void);

		Client		&operator[](unsigned int index);

		std::string GetName(void);
		int GetUsersCount(void);
		std::string GetPassword(void);
		std::vector<Client> GetClients(void);
		void SetName(std::string name);
		void SetPassword(std::string password);
		void SetClients(std::vector<Client> clients);
		void AddClient(Client client);
};

std::ostream&	operator<<(std::ostream& os, Channel& channel);