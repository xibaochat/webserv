#ifndef WEBSERVE_HPP
# define WEBSERVE_HPP

# include <sstream>
# include <stdio.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <cstring>
# include <iostream>
# include <fstream>
# include <experimental/filesystem>
# include <string>
# include <dirent.h>
# include <time.h>
# include <map>
# include <vector>
# include <ctime>
# include <algorithm>
# include <sys/types.h>
# include <sys/wait.h>
# include <pwd.h>
# include <grp.h>
# include <sys/stat.h>
# include <set>
# include <sstream>

# define GREEN       "\033[33;32m"
# define YELLOW      "\033[33;33m"
# define RED         "\033[33;31m"
# define MAGENTA     "\e[95m"
# define BLUE        "\033[1;34m"
# define NC          "\033[0m"
# define ERR_SEND  "Something went wrong when sending response"
# define AUTO_ON     "autoindex on;"
# define AUTO_OFF    "autoindex off;"
# define UPLOAD_DIR	"/tmp/"
# define UPLOAD_ON	"upload on;"
# define UPLOAD_OFF	"upload off;"
# define	UPLOAD_DEFAUT	"on"


# define VALID_CONF_KEYWORDS {"listen", "server_name", "error_page", "location"}


using namespace std;
typedef struct s_route
{
	s_route()
	{
		this->auto_index = false;
		auto_index_time = 0;
		acceptable_upload_time = 0;
	}
	std::string path_root;
	std::set<std::string> allow_methods;
	bool auto_index;
	int  auto_index_time;
	std::string	path_upload_root;
	std::string	acceptable_upload;
	int	acceptable_upload_time;
}              route;

class Client_Request;
class Conf;

std::vector<std::string> extract_words_in_vector(std::string &s);
int check_substring(std::string s, std::string s1);
int count_words(std::string str);
std::string extract_word_from_line(int &end, std::string &line);
std::string get_client_file(char *buffer);
void extract_info_from_rest_buffer(Client_Request &o, char *buffer);
Conf manage_config_file(std::stringstream &file);
std::string get_time();
std::string response_str(Client_Request &obj);
std::string get_client_file(char *buffer);
void check_err_page_validity(std::string file);
void extract_info_from_first_line_of_buffer(Client_Request &obj, char *buffer);

void echange_with_client(int &server_fd, struct sockaddr_in &address, Conf &web_conf);

std::map<int, std::string> init_status_code_message_map();

int open_file(std::ifstream &s, std::string path);
void open_conf(int ac, char **av, std::ifstream &file);
std::vector<Conf> get_all_server_conf(int ac, char **av);
void manage_request_status(route &r, Client_Request &obj, Conf &web_conf);

void set_length_and_content(std::ifstream &myfile, Client_Request &obj);
void delete_request(Client_Request &obj);
void set_request_status_nb_message(int status_nb, Client_Request &obj);

void set_error(Client_Request &obj, Conf &web_conf, int status_code_nb);
void send_response(Client_Request &obj, int &new_socket);
void manage_route(std::stringstream &file, std::string &line, std::map<std::string, route> &m);
int method_is_not_allow(route &r, Client_Request &obj);
int extension_is_not_exist(std::string *mylist, std::string extension, int size);
int file_no_permission(route &r, Client_Request &obj);
int file_not_exist(Client_Request &obj);
int file_is_text_based(std::string type);
std::string get_file_output(Client_Request &o);
std::vector<size_t> get_occurences_indexes(std::string s, std::string sub);
int get_closing_bracket_index(std::string s, int i_start);
int is_whitespace(char c);
void remove_fst_white_space(std::string &line);
int find_first_of_whitespace(std::string &line);
int is_only_whitespace(std::string &s);

//contain port, server_name, all err page
class Conf
{
//in fact, no need to put as privee
public:
	std::set<int> port;
	int client_max_body_size;
	std::string server_name;
	std::map<int, std::string>conf_map;//status code, error_page_path
public:
	std::map<std::string, route> m_location;
	Conf(){};
	~Conf(){};
	Conf(Conf const &s){*this = s;}
	Conf &operator=(Conf const &src)
	{
		this->port = src.port;
//		this->client_max_body_size = src.client_max_body_size;
		this->m_location = src.m_location;
		this->server_name = src.server_name;
		this->conf_map = src.conf_map;
		return *this;
	}

	std::set<int> get_port()const {return port;}
	std::map<std::string, route> get_m_location() const{return this->m_location;}
	int get_client_max_body_size() const{return client_max_body_size;}
	std::string get_server_name() const {return server_name;}
	std::map<int, std::string> get_conf_err_page_map() const
	{
		return this->conf_map;
	}
	void set_client_max_body_size(int n){this->client_max_body_size = n;}
	void set_port(std::set<int> port){this->port = port;}
	void set_m_location(std::map<std::string, route> &r){this->m_location = r;}
	void set_server_name(std::string f){this->server_name = f;}
	void set_config_map(std::map<int, std::string> src){
		this->conf_map = src;}
	void display_conf_file_debug()
	{


		std::cout << MAGENTA << "##### PARSING SERVER CONF #####\n\n" << NC;
		for (std::set<int>::iterator it=port.begin(); it!=port.end(); ++it)
			std::cout << "port " << *it << std::endl;
		std::cout << "server_name " << this->server_name << std::endl;
		std::map<int, std::string> mymap = this->get_conf_err_page_map();
		for (std::map<int, std::string>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
			std::cout << "status code: " << it->first << " corresponding page path" << " => " << it->second<< std::endl;
		std::map<std::string, route> m = this->get_m_location();
		for (std::map<std::string, route>::iterator it=m.begin(); it!=m.end(); ++it)
		{
			std::cout << "location: " << RED << it->first << "  ";
			cout <<  NC << " path_root" << " => " << BLUE << it->second.path_root << NC <<  "methods => \n";
			cout << "autoindex " << it->second.auto_index << "\n";
			std::set<string>::iterator itt;
			for (itt=it->second.allow_methods.begin(); itt!=it->second.allow_methods.end(); ++itt)
				std::cout << YELLOW << *itt << NC << "\n";
		}
		std::cout << MAGENTA << "##### END OF PARSING #####\n\n" << NC;
	}
};

//user method, asked file, file status_code
class Client_Request
{
public:
	bool dir_list;
	std::string origin_path;//in url
	std::string method;
	std::string clean_relative_path;//remove extra / from url
	std::string file;
	std::string	query_string;
	std::string f_extension;
	int 		status_code_nb;
	std::string status_code_message;
	unsigned long total_nb;
	std::string total_line;
	std::map<std::string, std::string> client_request;
	std::map<std::string, std::string> cgi_output;
public:
	Client_Request():method("GET"), file(""), dir_list(false), f_extension(""), status_code_nb(200), status_code_message("200 OK"), total_nb(0), total_line(""), origin_path(""){}
	~Client_Request(){};
	Client_Request(Client_Request const &src){*this = src;}
	Client_Request &operator=(Client_Request const &src)
	{
		this->method = src.method;
		this->file = src.file;
		this->origin_path = src.origin_path;
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
	std::string	get_query_string(){return this->query_string;}
	std::map<std::string, std::string> get_client_request_map(){return this->client_request;}
	std::map<std::string, std::string> get_cgi_output_map(){return this->cgi_output;}

	void set_client_method(std::string &src){this->method = src;}
	void set_client_file(std::string &src){this->file = src;}
	void set_file_extension(std::string src){this->f_extension = src;}
	void set_status_code_nb(int src){this->status_code_nb = src;}
	void set_status_code_message(std::string &src){this->status_code_message = src;}
	void set_total_nb(unsigned long src){this->total_nb = src;}
	void set_total_line(std::string &src){this->total_line = src;}
	void set_query_string(std::string &src){this->query_string = src;}
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
