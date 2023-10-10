#include "Server.hpp"

Server::Server(char const *argv1, char const *argv2) {
	if (!this->isServerRunning(atoi(argv1)))
	{
		(void)argv1;
		(void)argv2;
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

void Server::sendMsgToUsers(std::string message, Client &client) {
	std::vector<std::string> words = utils::split(message, " ");
	std::string first_word = words[1];
	// check if its a specific user
	if (first_word[0] == '@') {
		std::string username = first_word.substr(1);
		for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
			if (client_it->second.getUsername() == username) {
				this->sendMsgToSocket(client_it->second.getSocket(), client.getUsername() + ": " + message + "\n");
				return ;
			}
		}
	}
	// check if its a channel name send to all users in this channel expect client
	else if (first_word[0] == '#') {
		std::string channel_name = first_word;
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel_name) {
				for (std::map<std::string, Client *>::iterator client_it = channel_it->getClients().begin(); client_it != channel_it->getClients().end(); client_it++) {
					this->sendMsgToSocket(client_it->second->getSocket(), client.getUsername() + ": " + message + "\n");
				}
				return ;
			}
		}
	}
}

void Server::returnError(int client_socket, std::string error) {
	this->sendMsgToSocket(client_socket, error + "\n");
}

void Server::handlePassword(int client_socket, std::map<int, Client>::iterator it) {
	if (this->_buffer == this->_password) {
		// Password is correct, prompt for username
		this->sendMsgToSocket(client_socket, "Enter NICK :\n");
		it->second.setPassword(this->_buffer);
	}
	else {
		// Password is incorrect, disconnect the client
		this->returnError(client_socket, "Incorrect password");
		this->sendMsgToSocket(client_socket, "Enter PASS :\n");
		//disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
	}
}

void Server::makeUserJoinChannel(std::string channel, Client &client) {
	if (!this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		Channel new_channel(channel, client, "");
		this->AddChannel(new_channel);
		std::cout << "User " << client.getUsername() << " creates " << channel << std::endl;
	}
	else {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->addClient(client) == 0)
					std::cout << "User " << client.getUsername() << " joins " << channel << std::endl;
			}
		}
	}
}

void Server::makeUserLeaveChannel(std::string channel, Client &client) {
	if (this->ChannelExists(channel) && utils::checkChannelName(channel)) {
		for (std::vector<Channel>::iterator channel_it = this->_channels.begin(); channel_it != this->_channels.end(); channel_it++) {
			if (channel_it->getName() == channel) {
				if (channel_it->isClientInChannel(client)) {
					std::cout << "User " << client.getUsername() << " leaves " << channel << std::endl;
					channel_it->removeClient(client);
				}
			}
			else {
				std::cout << "User " << client.getUsername() << " is not in channel " << channel << std::endl;
			}
		}
	}
	else {
		throw std::invalid_argument("Channel does not exist");
	}
}

void Server::changeUsername(std::string username, Client &client) {
	if (utils::checkUserName(username)) {
		for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
			if (client_it->second.getUsername() == username)
				throw std::invalid_argument("Username already taken");
		}
		std::cout << "User " << client.getUsername() << " changed username to " << username << std::endl;
		client.setUsername(username);
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
						if (client_it->second.getUsername() == nickname) {
							std::cout << "User " << client.getUsername() << " kicked " << nickname << " from " << channel << std::endl;
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

void Server::handleUsername(int client_socket, std::map<int, Client>::iterator it) {
	// Check if the username is already taken
	
	for (std::map<int, Client>::iterator client_it = this->_clients.begin(); client_it != this->_clients.end(); client_it++) {
		if (client_it->second.getUsername() == this->_buffer) {
			this->returnError(client_socket, "Username already taken");
			this->sendMsgToSocket(client_socket, "Enter NICK :\n");
			return ;
		}
	}
	if (strlen(this->_buffer) > 9 || strlen(this->_buffer) < 1) {
		this->returnError(client_socket, "Invalid username lenght");
		this->sendMsgToSocket(client_socket, "Enter NICK :\n");
		return ;
	}
	// Username is valid, set the username and send a welcome message
	this->sendMsgToSocket(client_socket, "Welcome to the chat " + std::string(this->_buffer) + "!\n");
	it->second.setUsername(this->_buffer);
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
				this->_buffer[valread-1] = '\0';
				// If the client hasn't entered their password yet, check the received data against the password
				if (it->second.getPassword() == "")
					this->handlePassword(client_socket, it);
				// If the client has entered their password but not their username, set the received data as the username
				else if (it->second.getUsername() == "")
					this->handleUsername(client_socket, it);
				// If the client has entered both their password and username, handle the received data as a chat message
				else
					it->second.handleMessage(this->_buffer, *this);
			}
		}
	}
	// Close disconnected clients
	for (std::map<int, Client>::iterator it = disconnected_clients.begin(); it != disconnected_clients.end(); it++)
	{
		std::cout << it->second.getUsername() << " disconnected" << std::endl;
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
		if (it->second.getUsername() == client.getUsername())
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

std::ostream &operator<<(std::ostream &os, Server &server) {
	os << "Password: " << server.getPassword() << std::endl;
	os << "Channels count: " << server.getChannels().size() << std::endl;
	os << "Clients count: " << server.getClients().size() << std::endl;
	return (os);
}