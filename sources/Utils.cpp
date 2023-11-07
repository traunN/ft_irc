#include "Utils.hpp"
#include <openssl/sha.h>

namespace utils {
	void	ParseArgs(int argc, char **argv) {
		if (argc != 3)
			throw std::invalid_argument("Invalid number of arguments \nUsage: ./ircserv <port> <password>");
		std::stringstream ss(argv[1]); //Use stringstream to convert string to int
		int port;
		ss >> port;
		if (port < 194 || port > 9999)
			throw std::invalid_argument("Invalid port number");
		std::string password = argv[2];
		if (password.length() < 1 /*do we check for strong password or not?*/)
			throw std::length_error("Invalid password length");
	}
	
	bool checkChannelName(std::string channel) {
		if (channel.length() > 50)
			throw std::length_error("Channel name too long");
		else if (channel[0] != '#')
			throw std::invalid_argument("Channel name must start with #");
		return true;
	}

	bool checkUserName(std::string username) {
		if (username.length() > 9)
			throw std::length_error("Username too long");
		return true;
	}

	std::vector<std::string> split(const std::string& s, std::vector<std::string>& delimiters) {
		std::vector<std::string> tokens;
		tokens.push_back(s);
		for (size_t i = 0; i < delimiters.size(); ++i) {
			for (size_t j = 0; j < tokens.size(); ++j) {
				size_t pos = tokens[j].find(delimiters[i]);
				if (pos != std::string::npos) {
					std::string pre = tokens[j].substr(0, pos);
					std::string post = tokens[j].substr(pos + delimiters[i].size(), tokens[j].size() - (pos + delimiters[i].size()));
					tokens[j] = pre;
					tokens.insert(tokens.begin() + j + 1, post);
				}
			}
		}
		return tokens;
	}

	std::string trimWhitespace(const std::string& str) {
		const std::string whitespace = " \t\n\r\f\v";
		std::string::size_type strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return "";
		std::string::size_type strEnd = str.find_last_not_of(whitespace);
		std::string::size_type strRange = strEnd - strBegin + 1;
		return str.substr(strBegin, strRange);
	}

	std::string hashPassword(std::string password) {
		unsigned char hash[SHA256_DIGEST_LENGTH];
		unsigned const char* data = reinterpret_cast<const unsigned char*>(password.c_str());
		SHA256(data, password.size(), hash);

		std::stringstream ss;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
		}
		return (ss.str());
	}

	bool checkPassword(std::string providedPassword, std::string _hashedPassword) {
		std::string pw_hash = hashPassword(providedPassword);
		if (pw_hash != _hashedPassword)
		{
			std::cout << "Wrong password" << std::endl;
			std::cout << "_hashedPassword: " << _hashedPassword << std::endl;
			std::cout << "Hashed password: " << pw_hash << std::endl;
			return false;
		}
		return true;
	}
}