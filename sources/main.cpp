#include <iostream>

int	main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	try {
		int port = std::stoi(argv[1]);
		std::string password = argv[2];
		//launch ircserv
	}
	catch (std::exception &e) {
		//if fail
		//log error
		return 1;
	}
	return 0;
}