#pragma once

#include "Main.hpp"

class Channel;

class Server {
	private:
		int _server_fd;
		int _opt;
		int _new_socket;
		int _valread;
		int _addrlen;
		int _max_fd;
		int _port;
		pthread_t _thread;
		struct sockaddr_in _address;
		char _buffer[1024];
		std::string _password;
		std::vector<Channel> _channels;
		std::vector<Client> _clients;
		fd_set _readfds;
	public:
		Server(char const *argv1, char const *argv2);
		// Server(std::string name, std::string password);
		virtual ~Server(void);

		void Run(void);
		void Init(void);
		void ProcessNewClient(void);

		std::string GetName(void);
		std::string GetPassword(void);
		std::vector<Channel> GetChannels(void);
		std::vector<Client> GetClients(void);
		void SetName(std::string name);
		void SetPassword(std::string password);
		void SetChannels(std::vector<Channel> channels);
		void SetClients(std::vector<Client> clients);

		bool isServerRunning(int port);

		void AddChannel(Channel channel);
		void AddClient(Client client);
		void RemoveChannel(Channel channel);
		void RemoveClient(Client client);
};

std::ostream&	operator<<(std::ostream& os, Server& server);