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

namespace utils
{
	void	ParseArgs(int argc, char **argv)
	{
		(void)argv;
		if (argc != 3)
			throw std::invalid_argument("Invalid number of arguments");
	}
}