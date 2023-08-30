#pragma once

#include <iostream>
#include <exception>
#include <string>
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

#include "Utils.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

#define PORT 8080
