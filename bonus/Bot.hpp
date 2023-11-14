#pragma once

#include <iostream>
#include <iomanip>
#include <exception>
#include <cstring>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


namespace utils {
	
}

class Bot {
	private:
		int _socket;
		std::string _host;
		std::string _port;
		std::string _password;
	public:
		Bot(std::string host, std::string port, std::string password);
		~Bot(void);
		void run(void);
};
