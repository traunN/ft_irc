# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ntraun <ntraun@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/22 15:02:15 by ntraun            #+#    #+#              #
#    Updated: 2023/08/28 15:58:15 by ntraun           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

green = \033[32m
reset = \033[0m

SRCDIR = sources/
OBJDIR = .objects/

SRC =	$(SRCDIR)main.cpp\
		$(SRCDIR)Utils.cpp\
		# Client.cpp\
		# Channel.cpp\
		# Server.cpp\
		# Commands.cpp\
		
OBJ = $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRC))

INCLUDES = -I headers/

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

cc = c++ $(CFLAGS)

all: $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@$(cc) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJ)
	@$(cc) $^ -o $@
	@echo "$(green)$(NAME)$(reset) Created!"

$(OBJ): | $(OBJDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)
	@echo "Cleaned!"

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re