#include "Server.hpp"

Server::Server(char const *argv1, char const *argv2)
{
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
		try {
			this->Init();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return ;
		}
		std:: cout << "Server is running on port " << this->_port << std::endl;
		try {
			this->Run();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return ;
		}
	}
	else
		throw std::runtime_error("Server is already running");
}

Server::~Server(void)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		close(it->second.GetSocket());
	close(this->_server_fd);
}

bool Server::isServerRunning(int port)
{
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

void Server::ProcessNewClient(void)
{
	int new_socket = accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t *)&this->_addrlen);

	if (new_socket < 0)
		throw std::runtime_error("accept");
	else
	{
		char user_nickname[1024];
		send(new_socket, "Enter your nickname: ", strlen("Enter your nickname: "), 0);
		int valread = recv(new_socket, user_nickname, 1024, 0);
		if (valread == 0)
		{
			close(new_socket);
			throw std::runtime_error("recv");
			return;
		}
		//replace "\n" with "\0"
		user_nickname[valread - 1] = '\0';
		std::cout << "New User id " << new_socket << " connected" << std::endl;
		// Set the new socket as non-blocking
		int flags = fcntl(new_socket, F_GETFL, 0);
		if (flags < 0)
		{
			close(new_socket);
			throw std::runtime_error("fcntl");
			return;
		}
		if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			
			close(new_socket);
			throw std::runtime_error("fcntl");
			return;
		}
		Client *client = new Client(new_socket, user_nickname, "1");
		this->_clients.insert(std::pair<int, Client>(new_socket, *client));
	}
}

void Server::Run(void)
{
	int max_fd = this->_server_fd;

	while (true)
	{
		FD_ZERO(&this->_readfds);
		FD_SET(this->_server_fd, &this->_readfds);
		// Add client sockets to set
		for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		{
			int client_socket = it->second.GetSocket();
			FD_SET(client_socket, &this->_readfds);
			if (client_socket > max_fd)
				max_fd = client_socket;
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

void Server::CheckActivity(void)
{
	std::map<int, Client> disconnected_clients;
	int client_socket_sender;

	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		int client_socket = it->second.GetSocket();
		if (FD_ISSET(client_socket, &this->_readfds))
		{
			client_socket_sender = client_socket;
			// Check if it was for closing, and also read the incoming message
			int valread;
			valread = recv(client_socket, this->_buffer, 1024, 0);
			if (valread == 0)
				disconnected_clients.insert(std::pair<int, Client>(client_socket, it->second));
			else
			{
				// Set the string terminating NULL byte on the end of the data read
				this->_buffer[valread] = '\0';
				std::cout << this->_clients[client_socket].GetUsername() << ": " << this->_buffer;
				// Send message to all clients
				for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
				{
					int client_socket = it->second.GetSocket();
					if (client_socket != client_socket_sender)
					{
						std::string message = this->_clients[client_socket_sender].GetUsername() + ": " + this->_buffer;
						send(client_socket, message.c_str(), message.length(), 0);
					}
				}
			}
		}
	}
	// Close disconnected clients
	for (std::map<int, Client>::iterator it = disconnected_clients.begin(); it != disconnected_clients.end(); it++)
	{
		std::cout << disconnected_clients[it->first].GetUsername() << " disconnected" << std::endl;
		close(it->first);
		this->_clients.erase(it->second.GetSocket());
	}
}

void Server::Init(void)
{
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

std::string Server::GetPassword(void)
{
	return (this->_password);
}

std::vector<Channel> Server::GetChannels(void)
{
	return (this->_channels);
}

std::map<int, Client> Server::GetClients(void)
{
	return (this->_clients);
}

void Server::SetPassword(std::string password)
{
	this->_password = password;
}

void Server::SetChannels(std::vector<Channel> channels)
{
	this->_channels = channels;
}

void Server::AddChannel(Channel channel)
{
	this->_channels.push_back(channel);
}

void Server::AddClient(Client client)
{
	this->_clients.insert(std::pair<int, Client>(client.GetSocket(), client));
}

void Server::RemoveChannel(Channel channel)
{
	std::vector<Channel>::iterator it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		if (it->GetName() == channel.GetName())
		{
			this->_channels.erase(it);
			break;
		}
		it++;
	}
}

void Server::RemoveClient(Client client)
{
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (it->second.GetUsername() == client.GetUsername())
		{
			this->_clients.erase(it);
			break;
		}
		it++;
	}
}

std::ostream &operator<<(std::ostream &os, Server &server)
{
	os << "Password: " << server.GetPassword() << std::endl;
	os << "Channels count: " << server.GetChannels().size() << std::endl;
	os << "Clients count: " << server.GetClients().size() << std::endl;
	return (os);
}