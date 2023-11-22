#include "Bot.hpp"
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

static std::string trimWhitespace(const std::string& str);

Bot::Bot(std::string host, std::string password) {
	this->_host = host;
	this->_port = 0;
	this->_password = password;
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
}

Bot::~Bot(void) {
	close(this->_socket);
}

void Bot::Run(void) {
	struct sockaddr_in myaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(this->_port);
	if (inet_pton(AF_INET, this->_host.c_str(), &myaddr.sin_addr.s_addr) <= 0) {
		throw std::runtime_error("inet_pton() failed");
	}
	if (connect(this->_socket, (struct sockaddr*)&myaddr, sizeof(myaddr))) {
		throw std::runtime_error("connect() failed");
	}
	std::cout << "Connected to server " << this->_host << ":" << this->_port << std::endl;
	// create specific msg to send to server so it can read it like : this->_message[0] == 'B' && this->_message[1] == 'O' && this->_message[2] == 'T' && this->_message[3] == '\0'
	send(this->_socket, "BOT", 4, 0);
	int valread;
	std::string message;
	while (1) {
		char* buffer = new char[1024];
		memset(buffer, 0, 0);
		valread = recv(this->_socket, buffer, 1024, MSG_DONTWAIT);
		if (valread > 0) {
			message = buffer;
		 	std::istringstream iss(message);
			std::string nickname, command, args;
			iss >> nickname >> command;
			std::getline(iss, args);
			args = trimWhitespace(args);
			if (command == "TIMER" && args == "") {
				std::cout << "User " << nickname << " requested time" << std::endl;
				std::string str = getTime();
				send(this->_socket, str.c_str(), str.length(), 0);
			}
			else if (command == "PING" && args == "") {
				send(this->_socket, "PONG", 4, 0);
			}
			else if (command == "RPS") {
				if (args == "ROCK" || args == "PAPER" || args == "SCISSORS") {
					std::cout << "User " << nickname << " requested to play Rock, Paper, Scissors and played " << args << std::endl;
					std::string userMove = args;
					std::string botMove = getRPSMove();
					std::string result = getRPSResult(userMove, botMove);
					std::cout << "Result: " << result << std::endl;
					send(this->_socket, result.c_str(), result.length(), 0);
				}
				else {
					std::cout << "User " << nickname << " requested to play Rock, Paper, Scissors but played " << args << std::endl;
					send(this->_socket, "ERROR", 5, 0);
				}
			}
			else
			{
				send(this->_socket, "INVALID COMMAND", 16, 0);
			}
		}
		delete [] buffer;
	}
}

void	Bot::ParseArgs(int argc, char **argv) {
	if (argc != 4)
		throw std::runtime_error("Usage: ./ircbot <host> <port> <password>");
	std::stringstream ss(argv[2]); //Use stringstream to convert string to int
	ss >> this->_port;
	if (this->_port < 194 || this->_port > 9999)
		throw std::invalid_argument("Invalid port number");
	std::string password = argv[3];
	if (password.length() < 1 /*do we check for strong password or not?*/)
		throw std::length_error("Invalid password length");
}

std::string Bot::getTime(void) {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"Today's date is %d-%m-%Y and current time is %H:%M:%S",timeinfo);
	std::string str(buffer);

	return str;
}

std::string Bot::getRPSMove(void) {
	std::string move;
	int random = rand() % 3;
	switch (random) {
		case 0:
			move = "ROCK";
			break;
		case 1:
			move = "PAPER";
			break;
		case 2:
			move = "SCISSORS";
			break;
		default:
			move = "ERROR";
			break;
	}

	return move;
}

std::string Bot::getRPSResult(std::string userMove, std::string botMove) {
	std::string result;

	if (userMove == botMove)
		result = "TIE";
	else if (userMove == "ROCK" && botMove == "PAPER")
		result = "LOSE";
	else if (userMove == "ROCK" && botMove == "SCISSORS")
		result = "WIN";
	else if (userMove == "PAPER" && botMove == "ROCK")
		result = "WIN";
	else if (userMove == "PAPER" && botMove == "SCISSORS")
		result = "LOSE";
	else if (userMove == "SCISSORS" && botMove == "ROCK")
		result = "LOSE";
	else if (userMove == "SCISSORS" && botMove == "PAPER")
		result = "WIN";
	else
		result = "ERROR";

	return ("RPS result[" + result + " (BOT played " + botMove + ")]");
}

static std::string trimWhitespace(const std::string& str) {
	const std::string whitespace = " \t\n\r\f\v";
	std::string::size_type strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return "";
	std::string::size_type strEnd = str.find_last_not_of(whitespace);
	std::string::size_type strRange = strEnd - strBegin + 1;
	return str.substr(strBegin, strRange);
}
