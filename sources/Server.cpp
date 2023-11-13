#include "Server.hpp"

Server::Server(char const *argv1, char const *argv2) {
	this->_server_fd = 0;
	this->_max_fd = 0;
	this->_new_socket = 0;
	this->_valread = 0;
	if (!this->isServerRunning(atoi(argv1))) {
		this->_channels = std::vector<Channel>();
		this->_clients = std::map<int, Client>();
		this->_opt = 1;
		this->_addrlen = sizeof(this->_address);
		std::stringstream ss(argv1);
		ss >> this->_port;
		// this->_password = argv2;
		this->_password = utils::hashPassword(argv2);
		try {
			this->Init();
		}
		catch (std::exception &e) {
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
	this->sendMsgToSocket(new_socket, "Enter PASS :");
	// Add the new client to the map with an empty username and password
	Client client(new_socket, "", "", "");
	this->_clients.insert(std::pair<int, Client>(new_socket, client));
}

void Server::Run(void) {
	int max_fd = this->_server_fd;

	while (true) {
		FD_ZERO(&this->_readfds);
		FD_SET(this->_server_fd, &this->_readfds);
		// Add client sockets to set
		for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++) {
			int client_socket = it->second.getSocket();
			if (client_socket > 0) {
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
	message = message + "\n";
	int bytes_sent = send(client_socket, message.c_str(), message.length(), 0);
	if (bytes_sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
		throw std::runtime_error("send");
}

// REMEMBER TO PROMPT FOR USERNAME WHEN USER connect

std::vector<Channel>::iterator Server::getChannel(std::string channel_name) {
	std::vector<Channel>::iterator it = this->_channels.begin();
	while (it != this->_channels.end()) {
		if (it->getName() == channel_name)
			return (it);
		it++;
	}
	return _channels.end();
}

std::map<int, Client>::iterator Server::getClient(std::string client_name) {
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end()) {
		if (it->second.getNickname() == client_name)
			return (it);
		it++;
	}
	return _clients.end();
}

void Server::sendMsgToUsers(std::string target, std::string message, Client &client) {
	// check if its a channel name send to all users in this channel expect client
	if (target[0] == '#') {
		std::string channel_name = target;
		std::vector<Channel>::iterator channel_it = this->getChannel(channel_name);
		if (channel_it == this->_channels.end())
			throw std::invalid_argument("Channel does not exist");
		if (!channel_it->isClientInChannel(client))
			throw std::invalid_argument("You are not in this channel");
		for (std::map<std::string, Client *>::iterator client_it = channel_it->getClients().begin(); client_it != channel_it->getClients().end(); client_it++) {
			// cout buffer from client
			if (channel_it->isClientInChannel(*client_it->second))
				if (!client_it->second->getIsSic() || client_it->second->getSocket() != client.getSocket())
					sendMsgToSocket(client_it->second->getSocket(), channel_name + "\t <" + client.getNickname() + "> : " + message);
		}
		return ;
	}
	// if its not a channel its a user
	else {
		std::string username = target;
		std::map<int, Client>::iterator client_it = this->getClient(username);
		if (client_it == this->_clients.end())
			throw std::invalid_argument("User does not exist");
		this->sendMsgToSocket(client_it->second.getSocket(), client.getNickname() + ": " + message);
			return ;
	}
}

void Server::handleMessage(std::string input, Client &client) {
	std::string message;
	std::string target;
	std::stringstream ss(input);
	ss.ignore(8); //we skip the command
	ss >> target;
	std::getline(ss, message);
	if (message.length() > 512)
		throw std::invalid_argument("Message too long");
	sendMsgToUsers(target, message, client);
}

void Server::handlePassword(int client_socket, std::map<int, Client>::iterator it) {
	std::vector <std::string> delimiters;
	delimiters.push_back(" ");
	delimiters.push_back("\n");
	std::vector<std::string> words = utils::split(this->_message, delimiters);
	std::string first_word = words[0];
	// FROM SIC CLIENT
	if (first_word == "PASS") {
		std::string enteredPassword = words[1];
		enteredPassword.erase(std::remove_if(enteredPassword.begin(), enteredPassword.end(), ::isspace), enteredPassword.end());
		std::string storedPassword = this->_password;
		storedPassword.erase(std::remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());
		if (utils::checkPassword(enteredPassword, storedPassword)) {
			// Password is correct, prompt for username
			it->second.setPassword(enteredPassword);
			sendMsgToSocket(client_socket, "Enter NICK");
			std::string nick_key = words[2];
			nick_key.erase(std::remove_if(nick_key.begin(), nick_key.end(), ::isspace), nick_key.end());
			if (nick_key == "NICK")
			{
				std::string nickname = words[3];
				nickname.erase(std::remove_if(nickname.begin(), nickname.end(), ::isspace), nickname.end());
				// while nickname is not set prompt for nickname
				// clear message
				this->_message = nickname;
				this->handleNickname(client_socket, it->second);
				if (it->second.getNickname() == "")
				{
					throw std::invalid_argument("Nickname already taken");
					return ;
				}
				this->_message = nickname;
				this->handleUsername(client_socket, it->second);
				if (it->second.getUsername() == "")
				{
					throw std::invalid_argument("Username already taken");
					return ;
				}
				it->second.setIsSic(true);
			}
		}
		else {
			sendMsgToSocket(client_socket, "Incorrect password");
			throw std::invalid_argument("Incorrect password");
		}
	}
	else {
		// entering password manually using buffer
		std::string enteredPassword = this->_message;
		enteredPassword.erase(std::remove_if(enteredPassword.begin(), enteredPassword.end(), ::isspace), enteredPassword.end());
		std::string storedPassword = this->_password;
		storedPassword.erase(std::remove_if(storedPassword.begin(), storedPassword.end(), ::isspace), storedPassword.end());
		if (utils::checkPassword(enteredPassword, storedPassword)) {
			it->second.setPassword(enteredPassword);
			sendMsgToSocket(client_socket, "Enter NICK :");
			this->_message = "";
		}
		else {
			sendMsgToSocket(client_socket, "Incorrect password");
			sendMsgToSocket(client_socket, "Enter PASS :");
			this->_message = "";
		}
	}
}

void Server::notifyChannelJoinStatus(int op, std::string channel, Client &client) {
	switch (op) {
		case 0:
			sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " joins " + channel);
			if (this->getChannel(channel)->getTopic() != "")
				sendMsgToSocket(client.getSocket(), "Channel topic: " + this->getChannel(channel)->getTopic());
			break;
		case 1:
			sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " is already in " + channel);
			break;
		case 2:
			sendMsgToSocket(client.getSocket(), "Channel " + channel + " is full");
			break;
		case 3:
			sendMsgToSocket(client.getSocket(), "Channel " + channel + " is invite only");
			break;
		default:
			break;
	}
}

void Server::makeUserJoinChannel(std::string message, Client &client) {
	std::string channel;
	std::string password;
	std::stringstream ss(message);

	ss.ignore(5); //we ignore the command
	ss >> channel;
	ss >> password;
	if (!this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		Channel new_channel(channel, client, "");
		this->AddChannel(new_channel);
		sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " creates " + channel);
	}
	else {
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (channel_it->getName() == channel) {
			if (channel_it->hasPassword() && !utils::checkPassword(password, channel_it->getPassword()))
				throw std::invalid_argument("Invalid password");
			int op = channel_it->addClient(client);
			notifyChannelJoinStatus(op, channel, client);
		}
	}
}

void Server::makeUserLeaveChannel(std::string channel, Client &client) {
	if (this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (channel_it->isClientInChannel(client)) {
			sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " leaves " + channel);
			channel_it->removeClient(client);
		}
		else {
			sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " is not in " + channel);
		}
	}
	else {
		throw std::invalid_argument("Channel does not exist");
	}
}

void Server::changeNickname(std::string username, Client &client) {
	if (utils::checkUserName(username)) {
		if (this->getClient(username) != this->_clients.end()) {
				throw std::invalid_argument("Nickname already taken");
		}
		sendMsgToSocket(client.getSocket(), "User " + client.getNickname() + " changed nickname to " + username);
		client.setNickname(username);
	}
}

void Server::inviteUserToChannel(std::string input, Client &client) {
	std::string channel;
	std::string nickname;

	std::stringstream ss(input);
	ss.ignore(7); //we ignore the command
	ss >> channel;
	ss >> nickname;
	if (!this->ChannelExists(channel))
		throw std::invalid_argument("Channel does not exist");
	if (nickname.length() > 50)
		throw std::invalid_argument("Invalid nickname");
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) {
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (!channel_it->isClientInChannel(client))
			throw std::invalid_argument("You are not in this channel");
		if (channel_it->isOp(client)) {
			channel_it->addInvited(nickname);
			sendMsgToSocket(client.getSocket(), "You invited " + nickname + " to join " + channel);
			std::map<int, Client>::iterator client_it = this->getClient(nickname);
			sendMsgToSocket(client_it->second.getSocket(), client.getNickname() + " invited you to join " + channel);
		}
		else
			throw std::invalid_argument("You are not op in this channel, you can not invite users");
	}
}

void Server::notifyChannelRemoveModeStatus(int op, std::string channel, Client &client) {
	switch (op) {
		case 0:
			sendMsgToSocket(client.getSocket(), "Mode invite only for " + channel + " unset");
			break;
		case 1:
			sendMsgToSocket(client.getSocket(), "Mode topic for " + channel + " unset");
			break;
		case 2:
			sendMsgToSocket(client.getSocket(), "Mode password for " + channel + " unset");
			break;
		case 3:
			sendMsgToSocket(client.getSocket(), "Mode client limit for " + channel + " unset");
			break;
		case 4:
			sendMsgToSocket(client.getSocket(), "Mode op for " + channel + " unset");
			break;
		default:
			break;
	}
}

void Server::notifyChannelAddModeStatus(int op, std::string channel, Client &client) {
	switch (op) {
		case 0:
			sendMsgToSocket(client.getSocket(), "Mode invite only for " + channel + " set");
			break;
		case 1:
			sendMsgToSocket(client.getSocket(), "Mode topic for " + channel + " set");
			break;
		case 2:
			sendMsgToSocket(client.getSocket(), "Mode password for " + channel + " set");
			break;
		case 3:
			sendMsgToSocket(client.getSocket(), "Mode client limit for " + channel + " set");
			break;
		case 4:
			sendMsgToSocket(client.getSocket(), "Mode op for " + channel + " set");
			break;
		default:
			break;
	}
}

void Server::changeChannelMode(std::string input, Client &client) {
	std::string channel;
	std::string mode;
	std::string arg;
	int op = -1;

	std::stringstream ss(input);
	ss.ignore(5);
	ss >> channel;
	ss >> mode;
	ss.ignore(1);
	getline(ss, arg); // we get the rest of the line to send to the addMode function
	if (!this->ChannelExists(channel))
		throw std::invalid_argument("Channel does not exist");
	if (mode.length() < 2 || mode.length() > 3)
		throw std::invalid_argument("Invalid mode, use MODE <#channel> <+/-mode> (i : invite only, t: topic, k: password, o: give/take op, l: client limit)");
	if (utils::checkModeArg(mode, arg) == false)
		throw std::invalid_argument("Invalid mode argument");
	if (this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (channel_it->isOp(client)) {
			if (mode[0] == '+')
				op = channel_it->addMode(mode.substr(1), arg);
			else if (mode[0] == '-')
				op = channel_it->removeMode(mode.substr(1));
			else
				throw std::invalid_argument("Invalid mode, use MODE <#channel> <+/-mode> (i : invite only, t: topic, k: password, o: give/take op, l: client limit)");
		}
		else {
			throw std::invalid_argument("You are not op in this channel, you can not change its mode");
		}
	}
	if (op != -1) {
		if (mode[0] == '+')
			notifyChannelAddModeStatus(op, channel, client);
		else if (mode[0] == '-')
			notifyChannelRemoveModeStatus(op, channel, client);
	}
}

void Server::changeChannelTopic(std::string input, Client &client) {
	std::string channel;
	std::string topic;

	std::stringstream ss(input);
	ss.ignore(6);
	ss >> channel;
	getline(ss, topic);
	if (topic.length() > 50)
		throw std::invalid_argument("Invalid topic");
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) {
		if (!this->getChannel(channel)->isClientInChannel(client))
			throw std::invalid_argument("You are not in this channel");
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (!channel_it->getTopic().empty() && topic.empty())
			sendMsgToSocket(client.getSocket(), channel_it->getTopic());
		else if (channel_it->isRestrictedTopic() && !channel_it->isOp(client))
			throw std::invalid_argument("This channel has a restricted topic, you can not change it");
		else if (channel_it->getTopic().empty() && topic.empty())
			throw std::invalid_argument("This channel has no topic");
		else if (topic.empty())
			throw std::invalid_argument("Invalid topic");
		else {
			channel_it->setTopic(utils::trimWhitespace(topic));
		}
	}
	else {
		throw std::invalid_argument("Channel does not exist");
	}
}

void Server::kickUserFromChannel(std::string input, Client &client) {
	std::string channel;
	std::string nickname;

	std::stringstream ss(input);
	ss.ignore(5);
	ss >> channel;
	ss >> nickname;
	if (!this->ChannelExists(channel))
		throw std::invalid_argument("Channel does not exist");
	if (nickname == client.getNickname())
		throw std::invalid_argument("You can not kick yourself");
	if (utils::checkChannelName(channel) && this->ChannelExists(channel)) { 
		if (!this->getChannel(channel)->isClientInChannel(client))
			throw std::invalid_argument("You are not in this channel");
		std::vector<Channel>::iterator channel_it = this->getChannel(channel);
		if (channel_it->isOp(client)) {
			std::map<int, Client>::iterator client_it = this->getClient(nickname);
			if (client_it == this->_clients.end())
				throw std::invalid_argument("User does not exist");
			if (!channel_it->isClientInChannel(client_it->second))
				throw std::invalid_argument("User is not in this channel");
			sendMsgToSocket(client_it->second.getSocket(), client.getNickname() + " kicked you from " + channel);
			channel_it->removeClient(client_it->second);
		}
	}
	else {
		throw std::invalid_argument("You are not op in this channel");
	}
}

void Server::handleNickname(int client_socket, Client &client) {
	// Check if the username is already taken
	if (this->_message.length() > 9 || this->_message.length() < 1) {
		sendMsgToSocket(client_socket, "Invalid username lenght");
		sendMsgToSocket(client_socket, "Enter NICK :");
		return ;
	}
	for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
		if (client_it->second.getNickname() == this->_message) {
			sendMsgToSocket(client_socket, "Nickname already taken");
			sendMsgToSocket(client_socket, "Enter NICK :");
			return ;
		}
	}
	// Nickname is valid, set the username and send a welcome message
	client.setNickname(this->_message);
	// Ask for a username
	sendMsgToSocket(client_socket, "Enter USER :");
}

void Server::handleUsername(int client_socket, Client &client) {
	// Check if the username is already taken
	if (this->_message.length() > 9 || this->_message.length() < 1) {
		sendMsgToSocket(client_socket, "Invalid username lenght");
		sendMsgToSocket(client_socket, "Enter USER :");
		return ;
	}
	for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
		if (client_it->second.getUsername() == this->_message) {
			sendMsgToSocket(client_socket, "Username already taken");
			sendMsgToSocket(client_socket, "Enter USER :");
			return ;
		}
	}
	client.setUsername(this->_message);
	sendMsgToSocket(client_socket, "Welcome to the chat " + client.getNickname() + "!");
	std::cout << "New client " << client.getNickname() << " connected" << std::endl;
}

void Server::CheckActivity(void) {
	std::map<int, Client> disconnected_clients;
	//int client_socket_sender;
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++) {
		int client_socket = it->second.getSocket();
		if (FD_ISSET(client_socket, &this->_readfds)) {
			//client_socket_sender = client_socket;
			// Check if it was for closing, and also read the incoming message
			int valread;
			char *buffer = new char[1024];
			// if there is smthing in buffer join it at start of new buffer
			valread = recv(client_socket, buffer, 1024, MSG_DONTWAIT);
			if (this->_temp != "") {
				std::string temp = this->_temp;
				this->_temp = "";
				this->_message = temp + buffer;
			}
			else if (this->_temp == "")
				this->_message = buffer;
			delete[] buffer;
			// MAYBE SHOULD STORE BUFFER IN AN STD STRING MESSAGE SO ITS EASIER INSTANTLY
			if (valread == 0)
				disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
			else if (valread < 0) {
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					continue;
				else
					disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
			}
			else {
				//remove \n at end of message
				size_t newlinePos = this->_message.find('\n');
				if (newlinePos != std::string::npos) {
					// whats after my new line
					if (this->_message[newlinePos + 1] != 'N')
						this->_message = this->_message.substr(0, newlinePos);
				}
				else {
					this->_temp = this->_message;
					continue;
				}
				if (it->second.getPassword() == "") {
					try {
						this->handlePassword(client_socket, it);
					}
					catch (std::exception &e) {
						disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
					}
				}
				// If the client has entered their password but not their username, set the received data as the username
				else if (it->second.getNickname() == "") {
					this->handleNickname(client_socket, it->second);
				}
				else if (it->second.getUsername() == "") {
					this->handleUsername(client_socket, it->second);
				}
				// If the client has entered both their password and username, handle the received data as a chat message
				else {
					it->second.handleMessage(this->_message, *this);
				}
			}
		}
	}
	// Close disconnected clients
	this->closeDisconnectedClients(disconnected_clients);
}

void Server::closeDisconnectedClients(std::map<int, Client> disconnected_clients) {
	for (std::map<int, Client>::iterator it = disconnected_clients.begin(); it != disconnected_clients.end(); it++) {
		std::cout << "User " << it->second.getNickname() << " disconnected" << std::endl;
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++)
		{
			if (channel_it->isClientInChannel(it->second))
			{
				channel_it->removeClient(it->second);
				sendMsgToSocket(it->second.getSocket(), "User " + it->second.getNickname() + " leaves " + channel_it->getName());
			}
		}
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

std::string Server::getMessage(void) {
	return (this->_message);
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
	while (it != this->_channels.end()) {
		if (it->getName() == channel.getName()) {
			this->_channels.erase(it);
			break;
		}
		it++;
	}
}

void Server::RemoveClient(Client client) {
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end()) {
		if (it->second.getNickname() == client.getNickname()) {
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

void	Server::debug() {
	int client_number = 1;
	std::cout << "##############################" << std::endl;
	std::cout << "CHANNELS IN SERVER : " << std::endl;
	for (unsigned int i = 0; i < this->_channels.size(); i++) {
		std::cout << "\t\t -  " << this->_channels[i].getName() << std::endl;
		std::cout << "Clients in channel " << this->_channels[i].getName() << " : " << std::endl;
		std::map<std::string, Client *> clients = this->_channels[i].getClients();
		for (std::map<std::string, Client *>::iterator it = clients.begin(); it != clients.end(); it++)
			std::cout << "\t\t - " << it->second->getNickname() << std::endl;
	}
	std::cout << "CLIENTS IN SERVER : " << std::endl;
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++) {
		std::cout << "\t - Client n." << client_number << " : " << it->second.getNickname() << std::endl;
		client_number++;
	}
	std::cout << "##############################" << std::endl;
}

std::ostream &operator<<(std::ostream &os, Server &server) {
	os << "Password: " << server.getPassword() << std::endl;
	os << "Channels count: " << server.getChannels().size() << std::endl;
	os << "Clients count: " << server.getClients().size() << std::endl;
	return (os);
}