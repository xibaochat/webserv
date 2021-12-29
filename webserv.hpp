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
#include <algorithm>
#define PORT 8080
#define GREEN       "\033[33;32m"
#define YELLOW      "\033[33;33m"
#define RED         "\033[33;31m"
#define MAGENTA     "\e[95m"
#define BLUE        "\033[1;34m"
#define NC          "\033[0m"
using namespace std;
class Client_Request;
std::map<std::string, std::string> extract_info_from_header(Client_Request &o, char *buffer);
void check_config_file(int ac, char **av);
void get_time(std::string &response);
std::string response_str(Client_Request &obj);
std::string get_client_file(char *buffer);

class Conf
{
private:
	int port;
	std::string server_name;
	std::string conf_file;
	std::map<int, std::vector<std::string> >conf_map;//status code, in vector, they are status_message, error_page
public:
	Conf():port(0), conf_file(""), server_name(""){}
	~Conf(){};
	Conf(Conf const &s){*this = s;}
	Conf &operator=(Conf const &src)
	{
		this->port = src.port;
		this->server_name = src.server_name;
		this->conf_file = src.conf_file;
		this->conf_map = src.conf_map;
		return *this;
	}
	int get_port()const {return port;}
	std::string get_server_name() const {return server_name;}
	std::string get_conf_file() const {return conf_file;}
	std::map<int, std::vector<std::string> > get_conf_map() const
	{
		return this->conf_map;
	}
	void set_port(int port){this->port = port;}
	void set_server_name(std::string f){this->server_name = f;}
	void set_conf_file(std::string file){this->conf_file = file;}
	void set_config_map(std::map<int, std::vector<std::string> > src){
		this->conf_map = src;}

};


class Client_Request
{
private:
	std::string method;
	std::string file;
	std::string status_code;
	unsigned long total_nb;
	std::string total_line;

public:
	Client_Request():method(""), file(""), status_code(""), total_nb(0), total_line(""){}
	~Client_Request(){};
	Client_Request(Client_Request const &src){*this = src;}
	Client_Request &operator=(Client_Request const &src)
	{
		this->method = src.method;
		this->file = src.file;
		this->status_code = src.status_code;
		this->total_nb = src.total_nb;
		this->total_line = src.total_line;
		return *this;
	}
	std::string get_client_method(){return this->method;}
	std::string get_client_ask_file(){return this->file;}
	std::string get_status_code(){return this->status_code;}
	unsigned long get_total_nb(){return this->total_nb;}
	std::string get_total_line(){return this->total_line;}

	void set_client_method(std::string &src){this->method = src;}
	void set_client_file(std::string &src){this->file = src;}
	void set_status_code(std::string &src){this->status_code = src;}
	void set_total_nb(unsigned long src){this->total_nb = src;}
	void set_total_line(std::string &src){this->total_line = src;}
};

#endif
