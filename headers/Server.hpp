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
		std::string getMessage(void);
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

		bool ChannelExists(std::string channel_name);

		void makeUserJoinChannel(std::string channel,Client &client);
		void makeUserLeaveChannel(std::string channel, Client &client);
		void changeNickname(std::string nickname, Client &client);
		void kickUserFromChannel(std::string nickname, Client &client);
		void changeChannelMode(std::string input, Client &client);
		void inviteUserToChannel(std::string input, Client &client);
		void changeChannelTopic(std::string input, Client &client);

		void handleNickname(int client_socket, Client &client);
		void handleUsername(int client_socket, Client &client);
		void handlePassword(int client_socket, std::map<int, Client>::iterator it);
		void returnError(int client_socket, std::string error);
		void sendMsgToSocket(int client_socket, std::string message);
		void sendMsgToUsers(std::string message, Client &client);

		void debug(void);
};

std::ostream&	operator<<(std::ostream& os, Server& server);