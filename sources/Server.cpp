#include "Server.hpp"

void HandleClient(Client *client)
{
	char buffer[1024];
	int valread;
	while(true)
	{
		memset(buffer, 0, 1024);
		valread = recv(client->GetSocket(), buffer, 1024, 0);
		if (valread == 0)
			break;
		else if (valread == -1)
		{
			perror("recv");
			exit(EXIT_FAILURE);
		}
		buffer[valread] = '\0';
		std::cout << client->GetUsername() << ": " << buffer;
	}
	// Client disconnected
	std::cout << "Client id " << client->GetSocket() << " disconnected" << std::endl;
}

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
		this->Init();
		std::cout << "Server created" << std::endl;
		this->Run();
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
    // New client connection
    int new_socket = accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t *)&this->_addrlen);
    
    if (new_socket < 0)
    {
        perror("accept");
    }
    else
    {
        std::cout << "New User id " << new_socket << " connected" << std::endl;
        
        // Set the new socket as non-blocking
        int flags = fcntl(new_socket, F_GETFL, 0);
        if (flags < 0)
        {
            perror("fcntl");
            close(new_socket);
            return;
        }
        if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            perror("fcntl");
            close(new_socket);
            return;
        }
        
        Client *client = new Client(new_socket, utils::gen_random(7), "1");
        this->_clients.insert(std::pair<int, Client>(new_socket, *client));
    }
}

void Server::Run(void)
{
    int max_fd = this->_server_fd;
    std::map <int, int> client_sockets;

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
                    // New client connection
                    this->ProcessNewClient();
                }
                else
                {
                    char buffer[1024];
                    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);

                    if (bytesRead <= 0)
                    {
                        // Connection closed or error occurred
                        if (bytesRead == 0)
						{
                            std::cout << "Client " << fd << " disconnected" << std::endl;
							this->_clients.erase(fd);
						}
                        else
                            perror("recv");
                        // Remove the client from the list
                        // You should implement a function to remove the client from _clients vector.
                    }
                    else
                    {
                        // Process the received data from the client
                        buffer[bytesRead] = '\0';
						std::cout << this->_clients[fd].GetUsername() << ": " << buffer;

                        // You can implement your message handling logic here.
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

std::ostream&	operator<<(std::ostream& os, Server& server) {
	os << "Password: " << server.GetPassword() << std::endl;
	os << "Channels count: " << server.GetChannels().size() << std::endl;
	os << "Clients count: " << server.GetClients().size() << std::endl;
	return (os);
}