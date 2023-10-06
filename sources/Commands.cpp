#include "Commands.hpp"

// Commands::Commands(std::string command, std::string args, Server &server)
//     : _command(command), _args(args), _server(server) {
// 	exec_command();
// }

Commands::~Commands(void) {
}

void Commands::exec_command() {
	std::cout << "Command: " << this->_command << std::endl;
}