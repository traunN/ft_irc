#pragma once

#include "Main.hpp"

namespace utils {
	void	ParseArgs(int argc, char **argv);
	bool 	checkChannelName(std::string channel);
	bool 	checkUserName(std::string username);
}