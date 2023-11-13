#include "Bot.hpp"

int main(int argc, char *argv[]) {
	try {
		if (argc < 3)
			throw std::runtime_error("Usage: ./ircbot <host> <port> [password]");

		Bot bot(argv[1], argv[2], argc == 4 ? argv[3] : "");
		// bot.run();

		return (0);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
}