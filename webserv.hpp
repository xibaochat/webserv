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
#include <sys/types.h>
#include <sys/wait.h>


#define GREEN       "\033[33;32m"
#define YELLOW      "\033[33;33m"
#define RED         "\033[33;31m"
#define MAGENTA     "\e[95m"
#define BLUE        "\033[1;34m"
#define NC          "\033[0m"
#define ERR_SEND  "Something went wrong when sending response"

using namespace std;
class Client_Request;
class Conf;

std::string extract_word_from_line(int &end, std::string &line);
std::string get_client_file(char *buffer);
void extract_info_from_rest_buffer(Client_Request &o, char *buffer);
Conf manage_config_file(int ac, char **av);
std::string get_time();
std::string response_str(Client_Request &obj);
std::string get_client_file(char *buffer);
void check_err_page_validity(std::string file);
void extract_info_from_first_line_of_buffer(Client_Request &obj, char *buffer, Conf &web_conf);

void echange_with_client(int &server_fd, struct sockaddr_in &address, Conf &web_conf);

std::map<int, std::string> init_status_code_message_map();

int open_file(std::ifstream &s, std::string path);
void manage_request_status(Client_Request &obj, Conf &web_conf);

void set_length_and_content(std::ifstream &myfile, Client_Request &obj);

void send_error_page(int error_code, Client_Request &obj, Conf &web_conf, int &new_socket);
void send_response(Client_Request &obj, int &new_socket);
void manage_root(std::ifstream &file, std::string &line, std::map<std::string, std::string> &m);
//contain port, server_name, all err page
class Conf
{
private:
	int port;
	std::map<std::string, std::string> root;
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
		this->root = src.root;
		this->max_size_request = src.max_size_request;
		this->server_name = src.server_name;
		this->conf_map = src.conf_map;
		return *this;
	}
	void manage_item_value(std::string &item, std::vector<std::string> &vec);
	int get_port()const {return port;}
	std::map<std::string, std::string> get_root() const{return this->root;}
	int get_max_size_request() const{return max_size_request;}
	std::string get_server_name() const {return server_name;}
	std::map<int, std::string> get_conf_err_page_map() const
	{
		return this->conf_map;
	}
	void set_max_size_request(int n){this->max_size_request = n;}
	void set_port(int port){this->port = port;}
	void set_root(std::map<std::string, std::string>&r){this->root = r;}
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
		std::map<std::string, std::string> m = this->get_root();
		for (std::map<std::string, std::string>::iterator it=m.begin(); it!=m.end(); ++it)
			std::cout << "location: " << it->first << " root" << " => " << it->second<< std::endl;
	}
};

//user method, asked file, file status_code
class Client_Request
{
private:
	std::string method;
	std::string file;
	std::string f_extension;
	int 		status_code_nb;
	std::string status_code_message;
	unsigned long total_nb;
	std::string total_line;
	std::map<std::string, std::string> client_request;
	std::map<std::string, std::string> cgi_output;
public:
	Client_Request():method("GET"), file(""), f_extension(""), status_code_nb(200), status_code_message("200 OK"), total_nb(0), total_line(""){}
	~Client_Request(){};
	Client_Request(Client_Request const &src){*this = src;}
	Client_Request &operator=(Client_Request const &src)
	{
		this->method = src.method;
		this->file = src.file;
		this->f_extension = src.f_extension;
		this->status_code_nb = src.status_code_nb;
		this->status_code_message = src.status_code_message;
		this->total_nb = src.total_nb;
		this->total_line = src.total_line;
		return *this;
	}
	std::string get_client_method(){return this->method;}
	std::string get_client_ask_file(){return this->file;}
	std::string get_file_extension(){return this->f_extension;}
	int get_status_code_nb(){return this->status_code_nb;}
	std::string get_status_code_message(){return this->status_code_message;}
	unsigned long get_total_nb(){return this->total_nb;}
	std::string get_total_line(){return this->total_line;}
	std::map<std::string, std::string> get_client_request_map(){return this->client_request;}
	std::map<std::string, std::string> get_cgi_output_map(){return this->cgi_output;}

	void set_client_method(std::string &src){this->method = src;}
	void set_client_file(std::string &src){this->file = src;}
	void set_file_extension(std::string src){this->f_extension = src;}
	void set_status_code_nb(int src){this->status_code_nb = src;}
	void set_status_code_message(std::string &src){this->status_code_message = src;}
	void set_total_nb(unsigned long src){this->total_nb = src;}
	void set_total_line(std::string &src){this->total_line = src;}
	void set_client_request_map(std::map<std::string, std::string> &src)
	{
		this->client_request = src;
	}
	void set_cgi_output_map(std::map<std::string, std::string> src)
	{
		this->cgi_output = src;
	}
	void display_client_request()
	{
		std::map<std::string, std::string> mymap = this->get_client_request_map();
		for (std::map<std::string, std::string>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
			std::cout << YELLOW << "client info key: " << NC << it->first << YELLOW << " value" << NC << " => " << it->second<< std::endl;
	}
};

#endif
