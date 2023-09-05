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
		std::map<int, Client> _clients;
		fd_set _readfds;
	public:
		Server(char const *argv1, char const *argv2);
		// Server(std::string name, std::string password);
		virtual ~Server(void);

		void Run(void);
		void Init(void);
		void ProcessNewClient(void);
		void CheckActivity(void);

		std::string getName(void);
		std::string getPassword(void);
		std::vector<Channel> getChannels(void);
		std::map<int, Client> getClients(void);
		void setName(std::string name);
		void setPassword(std::string password);
		void setChannels(std::vector<Channel> channels);

		bool isServerRunning(int port);

		void AddChannel(Channel channel);
		void AddClient(Client client);
		void RemoveChannel(Channel channel);
		void RemoveClient(Client client);
};

std::ostream&	operator<<(std::ostream& os, Server& server);