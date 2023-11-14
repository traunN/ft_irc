#include "Bot.hpp"

int main(int argc, char *argv[]) {
	try {
		if (argc < 3)
			throw std::runtime_error("Usage: ./ircbot <host> <port> [password]");
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	try {
		Bot bot(argv[1], argv[2], argv[3]);
		bot.run();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}