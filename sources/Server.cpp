#include "Server.hpp"

Server::Server(char const *argv1, char const *argv2) {
	if (!this->isServerRunning(atoi(argv1)))
	{
		this->_channels = std::vector<Channel>();
		this->_clients = std::map<int, Client>();
		this->_opt = 1;
		this->_addrlen = sizeof(this->_address);
		std::stringstream ss(argv1);
		ss >> this->_port;
		this->_password = argv2;
		try
		{
			this->Init();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return;
		}
		std::cout << "Server is running on port " << this->_port << std::endl;
	}
	else
		throw std::runtime_error("Server is already running");
}

Server::~Server(void) {
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		close(it->second.getSocket());
	close(this->_server_fd);
}

bool Server::isServerRunning(int port) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return false;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(sock);
		return true;
	}
	close(sock);
	return false;
}

void Server::ProcessNewClient(void) {
	// Use select for non blocking
	int new_socket;
	if ((new_socket = accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t *)&this->_addrlen)) < 0)
		throw std::runtime_error("accept");
	int flags = fcntl(new_socket, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl");
	if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl");
	// Send welcome message
	this->sendMsgToSocket(new_socket, "Enter PASS :\n");
	// Add the new client to the map with an empty username and password
	Client client(new_socket, "", "");
	this->_clients.insert(std::pair<int, Client>(new_socket, client));
}

void Server::Run(void) {
	int max_fd = this->_server_fd;

	while (true)
	{
		FD_ZERO(&this->_readfds);
		FD_SET(this->_server_fd, &this->_readfds);
		// Add client sockets to set
		for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		{
			int client_socket = it->second.getSocket();
			if (client_socket > 0)
			{
				FD_SET(client_socket, &this->_readfds);
				if (client_socket > max_fd)
					max_fd = client_socket;
			}
		}
		// Use select to monitor file descriptors for activity
		if (select(max_fd + 1, &this->_readfds, NULL, NULL, NULL) < 0)
			throw std::runtime_error("select");
		// If something happened on the master socket, then its an incoming connection
		if (FD_ISSET(this->_server_fd, &this->_readfds))
			this->ProcessNewClient();
		else
			this->CheckActivity();
	}
}

void Server::sendMsgToSocket(int client_socket, std::string message) {
	int bytes_sent = send(client_socket, message.c_str(), message.length(), 0);
	if (bytes_sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		throw std::runtime_error("send");
}

// REMEMBER TO PROMPT FOR USERNAME WHEN USER connect

void Server::sendMsgToUsers(std::string message, Client &client) {
	std::vector <std::string> delimiters;
	delimiters.push_back(" ");
	std::vector<std::string> words = utils::split(message, delimiters);
	std::string first_word = words[1];
	std::string full_message;

	for (std::vector<std::string>::iterator it = words.begin() + 2; it != words.end(); it++) {
		full_message += *it;
		full_message += " ";
	}
	// check if its a channel name send to all users in this channel expect client
	if (first_word[0] == '#') {
		std::string channel_name = first_word;
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel_name) {
				for (std::map<std::string, Client *>::iterator client_it = channel_it->getClients().begin(); client_it != channel_it->getClients().end(); client_it++) {
					// cout buffer from client
					if (!client_it->second->getIsSic() || client_it->second->getSocket() != client.getSocket())
						this->sendMsgToSocket(client_it->second->getSocket(), channel_name + "\t <" + client.getNickname() + "> : " + full_message + "\n");
				}
				return ;
			}
		}
	}
	else
	{
		std::string username = first_word;
		for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
			if (client_it->second.getNickname() == username) {
				this->sendMsgToSocket(client_it->second.getSocket(), client.getNickname() + ": " + message + "\n");
				return ;
			}
		}

	}
}

void Server::returnError(int client_socket, std::string error) {
	this->sendMsgToSocket(client_socket, error + "\n");
}

void Server::handlePassword(int client_socket, std::map<int, Client>::iterator it) {
	std::vector <std::string> delimiters;
	delimiters.push_back(" ");
	delimiters.push_back("\n");
	std::vector<std::string> words = utils::split(this->_buffer, delimiters);
	std::string first_word = words[0];
	// FROM SIC CLIENT
	if (first_word == "PASS") {
		std::string enteredPassword = words[1];
		enteredPassword.erase(std::remove_if(enteredPassword.begin(), enteredPassword.end(), ::isspace), enteredPassword.end());
		std::string storedPassword = this->_password;
		storedPassword.erase(std::remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());
		if (enteredPassword == storedPassword) {
			// Password is correct, prompt for username
			this->sendMsgToSocket(client_socket, "Password accepted\n");
			it->second.setPassword(enteredPassword);
			this->sendMsgToSocket(client_socket, "Enter NICK :\n");
			std::string nick_key = words[2];
			nick_key.erase(std::remove_if(nick_key.begin(), nick_key.end(), ::isspace), nick_key.end());
			if (nick_key == "NICK")
			{
				std::string nickname = words[3];
				nickname.erase(std::remove_if(nickname.begin(), nickname.end(), ::isspace), nickname.end());
				// while nickname is not set prompt for nickname
				memset(this->_buffer, 0, 1024);
				strcpy(this->_buffer, nickname.c_str());
				this->handleNickname(client_socket, it);
			}
		}
		else {
			this->returnError(client_socket, "Incorrect password");
			this->sendMsgToSocket(client_socket, "Enter PASS :\n");
			throw std::invalid_argument("Incorrect password");
		}
	}
	else {
		// entering password manually using buffer
		std::string enteredPassword = this->_buffer;
		enteredPassword.erase(std::remove_if(enteredPassword.begin(), enteredPassword.end(), ::isspace), enteredPassword.end());
		std::string storedPassword = this->_password;
		storedPassword.erase(std::remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());
		if (enteredPassword == storedPassword) {
			// Password is correct, prompt for username
			this->sendMsgToSocket(client_socket, "Password accepted\n");
			it->second.setPassword(enteredPassword);
			this->sendMsgToSocket(client_socket, "Enter NICK :\n");
			memset(this->_buffer, 0, 1024);
		}
		else {
			this->returnError(client_socket, "Incorrect password");
			this->sendMsgToSocket(client_socket, "Enter PASS :\n");
			memset(this->_buffer, 0, 1024);
		}
	}
}

void Server::makeUserJoinChannel(std::string channel, Client &client) {
	if (!this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		Channel new_channel(channel, client, "");
		this->AddChannel(new_channel);
		std::cout << "User " << client.getNickname() << " creates " << channel << std::endl;
	}
	else {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->addClient(client) == 0)
					std::cout << "User " << client.getNickname() << " joins " << channel << std::endl;
			}
		}
	}
}

void Server::makeUserLeaveChannel(std::string channel, Client &client) {
	if (this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->isClientInChannel(client)) {
					std::cout << "User " << client.getNickname() << " leaves " << channel << std::endl;
					channel_it->removeClient(client);
				}
			}
			else {
				std::cout << "User " << client.getNickname() << " is not in channel " << channel << std::endl;
			}
		}
	}
	else {
		throw std::invalid_argument("Channel does not exist");
	}
}

void Server::changeNickname(std::string username, Client &client) {
	if (utils::checkUserName(username)) {
		for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
			if (client_it->second.getNickname() == username)
				throw std::invalid_argument("Nickname already taken");
		}
		std::cout << "User " << client.getNickname() << " changed username to " << username << std::endl;
		client.setNickname(username);
	}
}

void Server::inviteUserToChannel(std::string input, Client &client) {
	std::string channel;
	std::string nickname;

	std::stringstream ss(input);
	ss >> channel;
	ss >> nickname;
	if (nickname.length() > 50)
		throw std::invalid_argument("Invalid nickname");
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->isOp(client)) {
					channel_it->addInvited(nickname);
				}
			}
			else {
				throw std::invalid_argument("You are not op in this channel, you can not invite users");
			}
		}
	}
}

void Server::changeChannelMode(std::string input, Client &client) {
	std::string channel;
	std::string mode;
	std::string arg;

	std::stringstream ss(input);
	ss >> channel;
	ss >> mode;
	ss >> arg;
	if (mode.length() < 2 || mode.length() > 3)
		throw std::invalid_argument("Invalid mode, use MODE <#channel> <+/-mode> (i : invite only, t: topic, k: password, o: give/take op, l: client limit)");
	if (this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->isOp(client)) {
					if (mode[0] == '+')
						channel_it->addMode(mode.substr(1), arg);
					else if (mode[0] == '-')
						channel_it->removeMode(mode.substr(1));
					else
						throw std::invalid_argument("Invalid mode");
				}
				else {
					throw std::invalid_argument("You are not op in this channel, you can not change its mode");
				}
			}
		}
	}
}

void Server::changeChannelTopic(std::string input, Client &client) {
	std::string channel;
	std::string topic;

	std::stringstream ss(input);
	ss >> channel;
	getline(ss, topic);
	if (topic.length() > 50)
		throw std::invalid_argument("Invalid topic");
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->isRestrictedTopic() && !channel_it->isOp(client))
					throw std::invalid_argument("This channel has a restricted topic, you can not change it");
				if (topic.empty() && !channel_it->getTopic().empty())
					std::cout << channel_it->getTopic() << std::endl;
				else if (topic.empty() && channel_it->getTopic().empty())
					throw std::invalid_argument("This channel has no topic");
				if (channel_it->isOp(client))
					channel_it->setTopic(utils::trimWhitespace(topic));
			}
			else {
				throw std::invalid_argument("You are not op in this channel, you can not change its topic");
			}
		}
	}
}

void Server::kickUserFromChannel(std::string input, Client &client) {
	std::string channel;
	std::string nickname;

	std::stringstream ss(input);
	ss >> nickname;
	ss >> channel;
	std::cout << nickname << " " << channel << std::endl;
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) { std::cout << "kick : channel : " << channel << std::endl;
				if (channel_it->isOp(client)) {
					for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
						if (client_it->second.getNickname() == nickname) {
							std::cout << "User " << client.getNickname() << " kicked " << nickname << " from " << channel << std::endl;
							channel_it->removeClient(client_it->second);
						}
					}
				}
				else {
					throw std::invalid_argument("You are not op in this channel");
				}
			}
		}
	}
}

void Server::handleNickname(int client_socket, std::map<int, Client>::iterator it) {
	// Check if the username is already taken

	for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
		if (client_it->second.getNickname() == this->_buffer) {
			this->returnError(client_socket, "Nickname already taken");
			this->sendMsgToSocket(client_socket, "Enter NICK :\n");
			return ;
		}
	}
	if (strlen(this->_buffer) > 9 || strlen(this->_buffer) < 1) {
		this->returnError(client_socket, "Invalid username lenght");
		this->sendMsgToSocket(client_socket, "Enter NICK :\n");
		return ;
	}
	// Nickname is valid, set the username and send a welcome message
	it->second.setNickname(this->_buffer);
	it->second.setUsername(this->_buffer);
	// Ask for a username

	this->sendMsgToSocket(client_socket, "Welcome to the chat " + std::string(this->_buffer) + "!\n");
	std::cout << "New client " << this->_buffer << " connected" << std::endl;
}

void Server::CheckActivity(void) {
	std::map<int, Client> disconnected_clients;
	int client_socket_sender;
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++) {
		int client_socket = it->second.getSocket();
		if (FD_ISSET(client_socket, &this->_readfds)) 
		{
			client_socket_sender = client_socket;
			// Check if it was for closing, and also read the incoming message
			int valread;
			valread = recv(client_socket, this->_buffer, 1024, MSG_DONTWAIT);
			if (valread == 0)
				disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
			else if (valread < 0) {
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					continue;
				else
					disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
			}
			else {
				// set the string terminating NULL byte on the end of the data read
				if (this->_buffer[valread - 2] == '\r' && this->_buffer[valread - 1] == '\n')
				{
					it->second.setIsSic(true);
					this->_buffer[valread - 2] = '\0';
				}
				else
				{
					it->second.setIsSic(false);
					this->_buffer[valread - 1] = '\0';
				}
				// If the client hasn't entered their password yet, check the received data against the password
				if (it->second.getPassword() == "")
				{
					try {
						this->handlePassword(client_socket, it);
					}
					catch (std::exception &e) {
						disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
					}
				}
				// If the client has entered their password but not their username, set the received data as the username
				else if (it->second.getNickname() == "")
				{
					if (it->second.getIsSic() && this->_buffer[0] == 'n' && this->_buffer[1] == ' ')
						strcpy(this->_buffer, this->_buffer + 2);
					this->handleNickname(client_socket, it);
				}
				// If the client has entered both their password and username, handle the received data as a chat message
				else
				{
					// if /r /n at the end of buffer remove it
					
					it->second.handleMessage(this->_buffer, *this);
				}
			}
		}
	}
	// Close disconnected clients
	for (std::map<int, Client>::iterator it = disconnected_clients.begin(); it != disconnected_clients.end(); it++)
	{
		std::cout << it->second.getNickname() << " disconnected" << std::endl;
		close(it->first);
		this->_clients.erase(it->second.getSocket());
	}
}

void Server::Init(void) {
	if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket failed");
	int flags = fcntl(this->_server_fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl");
	if (fcntl(this->_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl");
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)))
		throw std::runtime_error("setsockopt");
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(this->_port);
	if (bind(this->_server_fd, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(this->_server_fd, 3) < 0)
		throw std::runtime_error("listen");
}

std::string Server::getPassword(void) {
	return (this->_password);
}

std::vector<Channel> Server::getChannels(void) {
	return (this->_channels);
}

std::map<int, Client> Server::getClients(void) {
	return (this->_clients);
}

void Server::setPassword(std::string password) {
	this->_password = password;
}

void Server::setChannels(std::vector<Channel> channels) {
	this->_channels = channels;
}

void Server::AddChannel(Channel channel) {
	this->_channels.push_back(channel);
}

void Server::AddClient(Client client) {
	this->_clients.insert(std::pair<int, Client>(client.getSocket(), client));
}

void Server::RemoveChannel(Channel channel) {
	std::vector<Channel>::iterator it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		if (it->getName() == channel.getName())
		{
			this->_channels.erase(it);
			break;
		}
		it++;
	}
}

void Server::RemoveClient(Client client) {
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (it->second.getNickname() == client.getNickname())
		{
			this->_clients.erase(it);
			break;
		}
		it++;
	}
}

bool Server::ChannelExists(std::string channel_name) {
	std::vector<Channel>::iterator it = this->_channels.begin();
	while (it != this->_channels.end()) {
		if (it->getName() == channel_name)
			return (true);
		it++;
	}
	return (false);
}

void	Server::debug()
{
	std::cout << "CHANNELS IN SERVER : " << std::endl;
	for (unsigned int i = 0; i < this->_channels.size(); i++)
	{
		std::cout << "Channel " << i << " : " << this->_channels[i].getName() << std::endl;
		std::cout << "Clients in channel " << i << " : " << std::endl;
		std::map<std::string, Client *> clients = this->_channels[i].getClients();
		for (std::map<std::string, Client *>::iterator it = clients.begin(); it != clients.end(); it++)
			std::cout << it->first << std::endl;
	}
	std::cout << "CLIENTS IN SERVER : " << std::endl;
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		std::cout << it->second << std::endl;
}

std::ostream &operator<<(std::ostream &os, Server &server) {
	os << "Password: " << server.getPassword() << std::endl;
	os << "Channels count: " << server.getChannels().size() << std::endl;
	os << "Clients count: " << server.getClients().size() << std::endl;
	return (os);
}