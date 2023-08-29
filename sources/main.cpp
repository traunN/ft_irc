#include "Main.hpp"
#include <netdb.h>

#define MY_PORT "8080"
#define BACKLOG 10

int	main(int argc, char **argv)
{
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
	(void)argc;
	(void)argv;
	struct sockaddr_storage	their_addr;
	socklen_t	addr_size;
	struct addrinfo hints, *res;
	int	sockfd, new_fd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	getaddrinfo(NULL, MY_PORT, &hints, &res);
	// make a socket, bind it, and listen on it:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, BACKLOG);
	std::cout << "Server fd is " << sockfd << std::endl;
	addr_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
	std::cout << "New fd is " << new_fd << std::endl;
	return (0);
}