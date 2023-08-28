/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntraun <ntraun@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 15:04:18 by ntraun            #+#    #+#             */
/*   Updated: 2023/08/28 15:57:38 by ntraun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Main.hpp"

int	main(int argc, char **argv)
{
	try {
		utils::ParseArgs(argc, argv);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	std::cout << "Hello World!" << std::endl;
	return (0);
}