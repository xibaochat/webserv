NAME = webserv

SRCS_PATH = ./src

SRC = main.cpp check_config_file.cpp extract_info_from_header.cpp response_header.cpp \
	manage_request.cpp server.cpp manage_request_status_and_response.cpp utile.cpp \
	auto_index.cpp check_requested_files.cpp get_occurences_indexes.cpp get_closing_bracket_index.cpp \
	whitespaces.cpp get_server_conf.cpp str_to_int.cpp

SRC_PATH = $(addprefix $(SRCS_PATH)/,$(SRC))

OBJ = $(SRC:.cpp=.o)

CC = clang++

HEADER_DIR = includes

CFLAGS = -std=c++98  -Wall -Wextra -Werror

OBJS_DIR    = ./objects

OBJS =${SRC:%.cpp=${OBJS_DIR}/%.o}

BLINK=\e[5m
RED=\033[91m
ORANGE=\e[38;5;202m
PURPLE=\e[38;5;57m
BLUE=\033[94m
DARK_YELLOW=\033[0;33m
YELLOW=\033[93m
GREEN=\033[0;32m
NC=\033[0;0m

GREEN       = \033[33;32m
YELLOW      = \033[33;33m
RED         = \033[33;31m
WHITE       = \033[33;37m
MAGENTA     = \e[95m

ARC_RED     = \e[41m
ARC_ORANGE  = \e[101m
ARC_YELLOW  = \e[43m
ARC_GREEN   = \e[102m
ARC_BLUE    = \e[44m
ARC_MAGENTA = \e[45m
ARC_NC      = \e[49m


${OBJS_DIR}/%.o: ${SRCS_PATH}/%.cpp
	    @mkdir -p ${OBJS_DIR}
		@printf "${BLUE}WEBSERV${NC}:        ${DARK_YELLOW}Compilation...      ${YELLOW}%-40.40s${NC}\r" $(notdir $<)
		@${CC} ${FLAGS}  -I ${HEADER_DIR}  -c $< -o $@

${NAME}: header  ${OBJS} nyancat
	@printf "${BLUE}WEBSERV${NC}:    ${GREEN}Completed         ${YELLOW}------いいですね------${NC}\r\n"
	@$(CC) $(OBJS) -o $(NAME)
	@printf "\n${NC}⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤\n"
	@printf "${BLUE}WEBSERV${NC}:    ${GREEN}Ready             ${YELLOW}-------頑張ります------        ${NC}\n"
	@printf "⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤\n"
	@echo "\n\n${RED}███╗   ███╗██╗ █████╗  ██████╗    ███╗   ███╗██╗ █████╗  ██████╗"
	@echo "${ORANGE}████╗ ████║██║██╔══██╗██╔═══██╗   ████╗ ████║██║██╔══██╗██╔═══██╗"
	@echo "${YELLOW}██╔████╔██║██║███████║██║   ██║   ██╔████╔██║██║███████║██║   ██║"
	@echo "${GREEN}██║╚██╔╝██║██║██╔══██║██║   ██║   ██║╚██╔╝██║██║██╔══██║██║   ██║"
	@echo "${BLUE}██║ ╚═╝ ██║██║██║  ██║╚██████╔╝   ██║ ╚═╝ ██║██║██║  ██║╚██████╔╝"
	@echo "${PURPLE}╚═╝     ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝    ╚═╝     ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝ ${NC}\n\n"


all: ${NAME}

nyancat:
	@ echo "$(MAGENTA)                             "
	@ echo "          ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄         "
	@ echo "        ▄▀░░░░░░░░░░░░▄░░░░░░░▀▄       "
	@ echo "        █░░▄░░░░▄░░░░░░░░░░░░░░█       "
	@ echo "$(ARC_RED)        $(ARC_NC)█░░░░░░░░░░░░▄█▄▄░░▄░░░█ ▄▄▄   "
	@ echo "$(ARC_ORANGE) ▄▄▄▄▄  $(ARC_NC)█░░░░░░▀░░░░▀█░░▀▄░░░░░█▀▀░██  "
	@ echo "$(ARC_YELLOW) $(ARC_NC)██▄▀██$(ARC_YELLOW)▄$(ARC_NC)█░░░▄░░░░░░░██░░░░▀▀▀▀▀░░░░██  "
	@ echo "$(ARC_GREEN)  ▀$(ARC_NC)██▄▀██░░░░░░░░▀░██▀░░░░░░░░░░░░░▀██ "
	@ echo "$(ARC_BLUE)    ▀$(ARC_NC)████░▀░░░░▄░░░██░░░$(GREEN)▄█$(MAGENTA)░░░░▄░$(GREEN)▄█$(MAGENTA)░░██ "
	@ echo "$(ARGC_MAGENTA)       ▀$(ARC_NC)█░░░░▄░░░░░██░░░░▄░░░▄░░▄░░░██ "
	@ echo "       ▄█▄░░░░░░░░░░░▀▄░░▀▀▀▀▀▀▀▀░░▄▀  "
	@ echo "      █▀▀█████████▀▀▀▀████████████▀    "
	@ echo "      ████▀   ███▀      ▀███  ▀██▀     "
	@ echo "\n       ♥♥♥  Maobe's Makefile  ♥♥♥\n"
	@echo "\t\t    ${GREEN}${BLINK}KITTEN PARTY READY !!${NC}\n\n"

header:
	@printf "${NC}⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤\n"
	@printf "PROJECT       ${NC}STATUS            FILE          ${NC}\n"
	@printf "⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤⏤\n"

clean:
	@/bin/rm -f -r $(OBJS_DIR)
	@printf "${BLUE}PROJECT${NC}:      ${GREEN}Cleaned${NC}\n"

fclean: clean
	@/bin/rm -f $(NAME)
	@/bin/rm -f *~
	@/bin/rm -f *#


re: fclean all
.PHONY: all clean fclean re
