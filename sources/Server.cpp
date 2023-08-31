#include "Server.hpp"

Server::Server(char const *argv1, char const *argv2)
{
	if (!this->isServerRunning(atoi(argv1)))
	{
		(void)argv1;
		(void)argv2;
		this->_channels = std::vector<Channel>();
		this->_clients = std::vector<Client>();
		this->_opt = 1;
		this->_addrlen = sizeof(this->_address);
		std::stringstream ss(argv1);
		ss >> this->_port;
		this->_password = argv2;
		this->Init();
		std::cout << "Server created" << std::endl;
		this->Run();
	}
	else
	{
		throw std::runtime_error("Server is already running");
	}
}

Server::~Server(void)
{
	// Close all the client sockets
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		close(it->GetSocket());
	}
	close(this->_server_fd);
	// std::cout << "Server destroyed" << std::endl;
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
	// New client connection
	if ((this->_new_socket = accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t *)&this->_addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	std::cout << "New user connected" << std::endl;
	this->_valread = recv(this->_new_socket, this->_buffer, 1024, 0);
	this->_buffer[this->_valread] = '\0';
	// send(this->_new_socket, this->_buffer, 1024, 0);
	// Add the new client socket to the set and vector
	FD_SET(this->_new_socket, &this->_readfds);
	this->_clients.push_back(Client(this->_new_socket, "default", "default"));
	// Update the maximum file descriptor value
	if (this->_new_socket > this->_max_fd)
	{
		this->_max_fd = this->_new_socket;
	}
}

void Server::Run(void)
{
	int max_fd = this->_server_fd;
	while (true)
	{
		FD_ZERO(&this->_readfds);
		FD_SET(this->_server_fd, &this->_readfds);
		// Add child sockets to set
		for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		{
			// Socket descriptor
			int sd = it->GetSocket();
			// If valid socket descriptor then add to read list
			if (sd > 0)
			{
				FD_SET(sd, &this->_readfds);
			}
			// Highest file descriptor number, need it for the select function
			if (sd > max_fd)
			{
				max_fd = sd;
			}
		}
		// Use select to monitor file descriptors for activity
		if (select(max_fd + 1, &this->_readfds, NULL, NULL, NULL) < 0)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		// Check for activity on file descriptors
		for (int fd = 0; fd <= max_fd; fd++)
		{
			if (FD_ISSET(fd, &this->_readfds))
			{
				// Handle activity on the file descriptor
				if (fd == this->_server_fd)
				{
					// Activity on the server socket
					this->ProcessNewClient();
					std::vector<Client>::iterator it = this->_clients.begin();
					while (it != this->_clients.end())
					{
						if (it->GetSocket() == this->_new_socket)
						{
							std::cout << it->GetUsername() << ": " << this->_buffer;
							break;
						}
						it++;
					}
					memset(this->_buffer, 0, 1024);
				}
				else
				{
					// Activity from an existing client
					// Handle the client's request
					this->_valread = recv(fd, this->_buffer, 1024, 0);
					if (this->_valread > 0)
					{
						// Data received
						std::vector<Client>::iterator it = this->_clients.begin();
						while(it != this->_clients.end())
						{
							if (it->GetSocket() == fd)
							{
								std::cout << it->GetUsername() << ": " << this->_buffer;
								break;
							}
							it++;
						}
						memset(this->_buffer, 0, 1024);
						// Handle client request here
						// Example: Send response back to the client
						// send(fd, this->_buffer, strlen(this->_buffer) + 1, 0);
					}
					else if (this->_valread == 0)
					{
						// Connection closed by the client
						std::cout << "Client disconnected" << std::endl;
						// Close the socket and remove it from the set and vector
						close(fd);
						FD_CLR(fd, &this->_readfds);
						std::vector<Client>::iterator it = this->_clients.begin();
						while (it != this->_clients.end())
						{
							if (it->GetSocket() == fd)
							{
								this->_clients.erase(it);
								break;
							}
							it++;
						}
					}
				}
			}
		}
	}
}

void Server::Init(void)
{
	// Creating socket file descriptor
	if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	int flags = fcntl(this->_server_fd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	if (fcntl(this->_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(this->_port);

	// Forcefully attaching socket to the port 8080
	if (bind(this->_server_fd, (struct sockaddr *)&this->_address, sizeof(this->_address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(this->_server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
}

std::string Server::GetPassword(void)
{
	return (this->_password);
}

std::vector<Channel> Server::GetChannels(void)
{
	return (this->_channels);
}

std::vector<Client> Server::GetClients(void)
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

void Server::SetClients(std::vector<Client> clients)
{
	this->_clients = clients;
}

void Server::AddChannel(Channel channel)
{
	this->_channels.push_back(channel);
}

void Server::AddClient(Client client)
{
	this->_clients.push_back(client);
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
	std::vector<Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (it->GetUsername() == client.GetUsername())
		{
			this->_clients.erase(it);
			break;
		}
		it++;
	}
}

std::ostream &operator<<(std::ostream &os, Server &server)
{
	os << "Server Password: " << server.GetPassword() << std::endl;
	os << "Server Channels: " << std::endl;
	std::vector<Channel> channels = server.GetChannels();
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		os << *it << std::endl;
	}
	std::cout << std::endl;
	os << "Clients: " << std::endl;
	std::vector<Client> clients = server.GetClients();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		os << *it << std::endl;
	}
	return (os);
}