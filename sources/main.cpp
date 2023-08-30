#include "Main.hpp"
#include <netdb.h>

#define MY_PORT "8080"
#define BACKLOG 10

int	main(int argc, char **argv)
{
	try {
		utils::ParseArgs(argc, argv);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	try {
		Server server(argv[1], argv[2]);
		server.Run();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}