#pragma once

#include "Main.hpp"

class Channel;

class Server {
	private:
		std::string _name;
		std::string _password;
		std::vector<Channel> _channels;
		std::vector<Client> _clients;
	public:
		Server(std::string name, std::string password);
		virtual ~Server(void);
		void Run(void);

		std::string GetName(void);
		std::string GetPassword(void);
		std::vector<Channel> GetChannels(void);
		std::vector<Client> GetClients(void);
		void SetName(std::string name);
		void SetPassword(std::string password);
		void SetChannels(std::vector<Channel> channels);
		void SetClients(std::vector<Client> clients);

		void AddChannel(Channel channel);
		void AddClient(Client client);
		void RemoveChannel(Channel channel);
		void RemoveClient(Client client);
};