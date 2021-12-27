#ifndef WEBSERVE_HPP
# define WEBSERVE_HPP

#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>

#define PORT 8080
#define GREEN       "\033[33;32m"
#define YELLOW      "\033[33;33m"
#define RED         "\033[33;31m"
#define MAGENTA     "\e[95m"
#define BLUE        "\033[1;34m"
#define NC          "\033[0m"
using namespace std;
std::map<std::string, std::string> extract_info_from_header(char *buffer);
void get_time(std::string &response);

#endif
