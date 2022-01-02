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

#define GREEN       "\033[33;32m"
#define YELLOW      "\033[33;33m"
#define RED         "\033[33;31m"
#define MAGENTA     "\e[95m"
#define BLUE        "\033[1;34m"
#define NC          "\033[0m"


using namespace std;
class Client_Request;
class Conf;


std::map<std::string, std::string> extract_info_from_header(Client_Request &o, char *buffer);
Conf manage_config_file(int ac, char **av);
void get_time(std::string &response);
std::string response_str(Client_Request &obj);
std::string get_client_file(char *buffer);
void check_err_page_validity(std::string file);

std::map<int, std::string> init_status_code_message_map();

//contain port, server_name, all err page
class Conf
{
private:
	int port;
	int max_size_request;
	std::string server_name;
	std::map<int, std::string>conf_map;//status code, error_page_path
public:
	Conf():port(0), max_size_request(3000), server_name(""){}
	~Conf(){};
	Conf(Conf const &s){*this = s;}
	Conf &operator=(Conf const &src)
	{
		this->port = src.port;
		this->max_size_request = src.max_size_request;
		this->server_name = src.server_name;
		this->conf_map = src.conf_map;
		return *this;
	}
	int get_port()const {return port;}
	int get_max_size_request() const{return max_size_request;}
	std::string get_server_name() const {return server_name;}
	std::map<int, std::string> get_conf_err_page_map() const
	{
		return this->conf_map;
	}
	void set_max_size_request(int n){this->max_size_request = n;}
	void set_port(int port){this->port = port;}
	void set_server_name(std::string f){this->server_name = f;}
	void set_config_map(std::map<int, std::string> src){
		this->conf_map = src;}
	void display_conf_file_debug()
	{
		std::cout << "port " << this->get_port() << std::endl;
		std::cout << "max_size_request " << this->get_max_size_request() << std::endl;
		std::cout << "server_name " << this->get_server_name() << std::endl;
		std::map<int, std::string> mymap = this->get_conf_err_page_map();
		for (std::map<int, std::string>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
			std::cout << "status code: " << it->first << " corresponding page path" << " => " << it->second<< std::endl;
	}
};

//user method, asked file, file status_code
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
