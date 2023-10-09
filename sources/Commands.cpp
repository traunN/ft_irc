#include "Commands.hpp"

Commands::Commands(char *command, Server &server)
    : _command(command), _server(server) {
	exec_command();
}

Commands::~Commands(void) {
}

void Commands::exec_command() {
	std::cout << "Command: " << this->_command << std::endl;
	(void)this->_server;
}