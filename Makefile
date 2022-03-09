NAME = ./webserv

SRC = main.cpp check_config_file.cpp extract_info_from_header.cpp response_header.cpp \
	manage_request.cpp server.cpp manage_request_status_and_response.cpp utile.cpp \
	auto_index.cpp check_requested_files.cpp get_occurences_indexes.cpp get_closing_bracket_index.cpp \
	whitespaces.cpp get_server_conf.cpp str_to_int.cpp

SRC_PATH = $(addprefix ./src/,$(SRC))

HEADER_DIR = ./includes/

OBJ = $(SRC:.cpp=.o)

CC = clang++

CFLAGS = -g -std=c++98 -Wall -Wextra -Werror

all:
	@$(CC) $(CFLAGS) $(SRC_PATH) -c -I $(HEADER_DIR)
	@$(CC) $(OBJ) -o $(NAME)

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)
	@/bin/rm -f *~
	@/bin/rm -f *#
	@/bin/rm -f client
	@/bin/rm -rf tests/root

test_setup: all
	@rm -rf tests/root
	@mkdir -p tests/root
	@cp tests/index/* tests/root/
	@cp tests/root/index_example.html tests/root/index_permission.html
	@chmod 777 tests/root/index_example.html
	@chmod 000 tests/root/index_permission.html
	@clang++ -o client tests/client.cpp

re: fclean all
.PHONY: all clean fclean re
