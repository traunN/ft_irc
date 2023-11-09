#pragma once

#include "Main.hpp"

namespace utils {
	void						ParseArgs(int argc, char **argv);
	bool 						checkChannelName(std::string channel);
	bool 						checkUserName(std::string username);
	std::vector<std::string>	split(const std::string& s, std::vector<std::string>& delimiters);
	std::string 				trimWhitespace(const std::string& str);
	std::string 				hashPassword(std::string password);
	bool 						checkPassword(std::string providedPassword, std::string _hashedPassword);
	int 						stringToInt(std::string str);
}