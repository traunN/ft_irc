#pragma once

#include "Main.hpp"

class Server;

class Commands {
    private:
		char * _command;
		std::string _args;
		Server &_server;
    public:
		Commands(char * command, Server &server);
        virtual ~Commands(void);

		void	exec_command();
};