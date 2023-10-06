#pragma once

#include "Main.hpp"

class Commands {
    private:
		std::string _command;
		std::string _args;
		// Server _server;
    public:
		// Commands(std::string command, std::string args, Server &server);
        virtual ~Commands(void);

		void	exec_command();
};