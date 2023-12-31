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
		std::string _message;
		std::string _temp;
		std::string _password;
		std::vector<Channel> _channels;
		std::map<int, Client> _clients;
		std::vector<Client> _awaitingBot;
		fd_set _readfds;
	public:
		Server(char const *argv1, char const *argv2);
		virtual ~Server(void);

		void Run(void);
		void Init(void);
		void ProcessNewClient(void);
		void CheckActivity(void);
		void closeDisconnectedClients(std::map<int, Client> disconnected_clients);

		std::string getName(void);
		std::string getPassword(void);
		std::string getMessage(void);
		std::vector<Channel> getChannels(void);
		std::map<int, Client> getClients(void);
		void setPassword(std::string password);
		void setChannels(std::vector<Channel> channels);

		bool isServerRunning(int port);

		void AddChannel(Channel channel);
		void AddClient(Client client);
		void RemoveChannel(Channel channel);
		void RemoveClient(Client client);

		bool ChannelExists(std::string channel_name);
		bool isBotConnected(void);

		void makeClientJoinChannel(std::string channel,Client &client);
		void makeClientLeaveChannel(std::string channel, Client &client);
		void changeNickname(std::string nickname, Client &client);
		void kickClientFromChannel(std::string nickname, Client &client);
		void changeChannelMode(std::string input, Client &client);
		void inviteClientToChannel(std::string input, Client &client);
		void changeChannelTopic(std::string input, Client &client);

		void handleNickname(int client_socket, Client &client);
		void handleUsername(int client_socket, Client &client);
		void handleMessage(std::string input, Client &client);
		void handlePassword(int client_socket, std::map<int, Client>::iterator it);
		void returnError(int client_socket, std::string error);
		void sendMsgToSocket(int client_socket, std::string message);
		void sendMsgToClients(std::string target, std::string message, Client &client);
		void notifyChannelJoinStatus(int op, std::string channel, Client &client);
		void notifyChannelAddModeStatus(int op, std::string channel, Client &client);
		void notifyChannelRemoveModeStatus(int op, std::string channel, Client &client);
		std::vector<Channel>::iterator getChannel(std::string channel_name);
		std::map<int, Client>::iterator getClient(std::string client_name);
		Client *getClientBySocket(int socket);
};

std::ostream&	operator<<(std::ostream& os, Server& server);