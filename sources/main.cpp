#include "Main.hpp"

#define PORT 8080

int main(int argc, char const* argv[])
{
	(void)argc;
	(void)argv;
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	int flags = fcntl(server_fd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;	
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	  int max_fd = server_fd;
    std::vector<int> client_sockets; // Vector to store the connected client sockets
    while (true) {
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(server_fd, &readfds);
		for (std::vector<int>::iterator it = client_sockets.begin(); it != client_sockets.end(); ++it) {
			FD_SET(*it, &readfds);
		}
        // Use select to monitor file descriptors for activity
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		}
		
        // Check for activity on file descriptors
        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &readfds)) {
                // Handle activity on the file descriptor
                if (fd == server_fd) {
                    // New client connection
                    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    std::cout << "New user connected" << std::endl;
                    valread = recv(new_socket, buffer, 1024, 0);
					std::cout << buffer;
					bzero(buffer, 1024);
                    // send(new_socket, buffer, 1024, 0);
                    // Add the new client socket to the set and vector
                    FD_SET(new_socket, &readfds);
                    client_sockets.push_back(new_socket);

                    // Update the maximum file descriptor value
                    if (new_socket > max_fd) {
                        max_fd = new_socket;
                    }
                } else {
                    // Activity from an existing client
                    // Handle the client's request
                    valread = recv(fd, buffer, 1024, 0);
					if (valread > 0) {
						// Data received
						std::cout << buffer;
						bzero(buffer, 1024);
						// Handle client request here
						// Example: Send response back to the client
						// send(fd, buffer, strlen(buffer) + 1, 0);
					} else if (valread == 0) {
						// Connection closed by the client
						std::cout << "Connection closed by the client" << std::endl;
						// Close the socket and remove it from the set and vector
						close(fd);
						FD_CLR(fd, &readfds);
						client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), fd), client_sockets.end());
					}
            	}
        	}
    	}
	}
    // Close all the client sockets
    for (int i = 0; i < sizeof(client_sockets) / sizeof(client_sockets[0]); i++) {
    	close(client_sockets[i]);
	}

    // closing the listening socket
    close(server_fd);

    return 0;
}

// try {
	// 	utils::ParseArgs(argc, argv);
	// }
	// catch (std::exception &e) {
	// 	std::cerr << e.what() << std::endl;
	// 	return (1);
	// }
	// try {
	// 	Server server(argv[1], argv[2]);
	// 	server.Run();
	// 	try {
	// 		Channel channel("test", "1");
	// 		Channel channel2("piou", "secretcode");
	// 		server.AddChannel(channel);
	// 		server.AddChannel(channel2);
	// 		Client client("Bob", "1");
	// 		Client client2("leponge", "1");
	// 		server.AddClient(client);
	// 		server.AddClient(client2);
	// 		channel.AddClient(client);
	// 		channel.AddClient(client2);
	// 		// std::cout << server << std::endl;
	// 		std::cout << channel << std::endl;
	// 	}
	// 	catch (std::exception &e) {
	// 		std::cerr << e.what() << std::endl;
	// 		return (1);
	// 	}
	// }
	// catch (std::exception &e) {
	// 	std::cerr << e.what() << std::endl;
	// 	return (1);
	// }