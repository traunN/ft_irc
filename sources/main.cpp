#include "Main.hpp"

int	main(int argc, char **argv)
{
	try {
		utils::ParseArgs(argc, argv);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	std::cout << "Hello World!" << std::endl;
	return (0);
}