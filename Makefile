NAME = ./webserv

SRC = main.cpp check_config_file.cpp extract_info_from_header.cpp get_time.cpp  response_header.cpp init_status_code_message_map.cpp manage_request.cpp server.cpp

OBJ = $(SRC:.cpp=.o)

CC = clang++

CFLAGS = #-Wall -Wextra -Werror -std=c++98

all:
	@$(CC) $(CFLAGS) $(SRC) -c
	@$(CC) $(OBJ) -o $(NAME)

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)
	@/bin/rm -f *~
	@/bin/rm -f *#

re: fclean all
