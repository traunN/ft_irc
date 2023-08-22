# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ntraun <ntraun@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/22 14:42:37 by ntraun            #+#    #+#              #
#    Updated: 2023/08/22 14:45:16 by ntraun           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_irc

CC = clang++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp\
		Client.cpp\
		Server.cpp\
		Channel.cpp\
		Utils.cpp\
