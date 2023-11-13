# **************************************************************************** #

green = \033[32m
reset = \033[0m

SRCDIR = sources/
BONUSDIR = bonus/
OBJDIR = .objects/
BONUSOBJDIR = .objects/bonus/

# **************************************************************************** #

NAME = ircserv

SRC =	$(SRCDIR)main.cpp\
		$(SRCDIR)Utils.cpp\
		$(SRCDIR)Client.cpp\
		$(SRCDIR)Channel.cpp\
		$(SRCDIR)Server.cpp\

OBJ = $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRC))

# **************************************************************************** #

NAME_BONUS = ircbot

BOTSRC = $(BONUSDIR)main.cpp\
		 $(BONUSDIR)Bot.cpp\

BONUSOBJ = $(patsubst $(BONUSDIR)%.cpp, $(BONUSOBJDIR)%.o, $(BOTSRC))

# **************************************************************************** #

CXX = c++ $(CXXFLAGS)

CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98

INCLUDES = -I headers/

BONUSINCLUDES = -I bonus/

# **************************************************************************** #

all: $(NAME)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@$(CXX) $(INCLUDES) -c $< -o $@

$(BONUSOBJDIR)%.o: $(BONUSDIR)%.cpp
	@$(CXX) $(BONUSINCLUDES) -c $< -o $@

$(NAME): $(OBJ)
	@$(CXX) $^ -o $@ -lssl -lcrypto
	@echo "$(green)$(NAME)$(reset) Created!"

$(NAME_BONUS): $(BONUSOBJ)
	@$(CXX) $^ -o $@ -lssl -lcrypto
	@echo "$(green)$(NAME_BONUS)$(reset) Created!"

$(OBJ): | $(OBJDIR)

$(BONUSOBJ): | $(BONUSOBJDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(BONUSOBJDIR):
	@mkdir $(BONUSOBJDIR)

bonus: $(NAME) $(NAME_BONUS)

clean:
	@rm -rf $(OBJDIR)
	@echo "Cleaned!"

fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(NAME_BONUS)

re: fclean all

.PHONY: all bonus clean fclean re
