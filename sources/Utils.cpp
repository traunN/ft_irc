/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntraun <ntraun@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/28 15:56:46 by ntraun            #+#    #+#             */
/*   Updated: 2023/08/28 15:58:47 by ntraun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

namespace utils {
	void	ParseArgs(int argc, char **argv) {
		int port = std::atoi(argv[1]);
		std::string password = argv[2];
		if (argc != 3)
			throw std::invalid_argument("Invalid number of arguments \nUsage: ./ircserv <port> <password>");
		if (port < 194 || port > 7000)
			throw std::invalid_argument("Invalid port number");
		if (password.length() < 1 /*do we check for strong password or not?*/)
			throw std::invalid_argument("Invalid password length");
	}
}