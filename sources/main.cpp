#include "Main.hpp"
#include <netdb.h>
#include <fcntl.h>
#include <algorithm>

#define PORT 8080

int main(int argc, char const* argv[])
{
	(void)argc;
	(void)argv;
	Server server(argv[1], argv[2]);
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