NAME = ./webserv

SRC = main.cpp check_config_file.cpp extract_info_from_header.cpp response_header.cpp manage_request.cpp server.cpp manage_request_status_and_response.cpp utile.cpp auto_index.cpp check_requested_files.cpp

OBJ = $(SRC:.cpp=.o)

CC = clang++

CFLAGS = -g -std=c++98 #-Wall -Wextra -Werror

all:
	@$(CC) $(CFLAGS) $(SRC) -c
	@$(CC) $(OBJ) -o $(NAME)

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)
	@/bin/rm -f *~
	@/bin/rm -f *#
	@/bin/rm -f client
	@/bin/rm -rf test_this/root

test_setup: all
	@rm -rf test_this/root
	@mkdir -p test_this/root
	@cp test_this/index/* test_this/root/
	@cp test_this/root/index_example.html test_this/root/index_permission.html
	@chmod 000 test_this/root/index_permission.html
	@clang++ -o client test_this/client.cpp

re: fclean all
.PHONY: all clean fclean re
