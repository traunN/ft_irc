#pragma once

#include "Main.hpp"

class Client;

class Channel {
    private:
        bool _isInviteOnly;
        bool _hasPassword;
        bool _hasUsersLimit;

        size_t _userLimit;
		std::string _name;
		std::string _password;
		std::vector<Client> _clients;
    public:
        Channel(std::string name, std::string password);
        virtual ~Channel(void);

		std::string GetName(void);
		std::string GetPassword(void);
		std::vector<Client> GetClients(void);
		void SetName(std::string name);
		void SetPassword(std::string password);
		void SetClients(std::vector<Client> clients);
};

std::ostream&	operator<<(std::ostream& os, Channel& channel);