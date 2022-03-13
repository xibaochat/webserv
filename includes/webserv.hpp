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

# define VALID_CONF_KEYWORDS {"listen", "server_name", "error_page", "location", "client_max_body_size", "index"}

# define MANAGED_FILE_EXTENSIONS {"html", "css", "js", "txt", "xml", "json", "hpp", "cpp", "c", "sh", "txt", "md"}
# define TEXT_BASED_EXTENSIONS {"txt", "xml", "json", "hpp", "cpp", "c", "sh", "txt", "md"}
# define WEB_BASED_EXTENSIONS {"html", "css", "js"}


#define DEFAULT_MAX_BODY_SIZE 10485760
using namespace std;

class Client_Request;
class Conf;

typedef struct s_response cl_response;

typedef struct s_route
{
	s_route()
	{
		this->auto_index = false;
		this->auto_index_time = 0;
		this->acceptable_upload = "off";
		this->acceptable_upload_time = 0;
		this->redirection = "";
		this->path_root = "";
		this->path_upload_root = "";
	}
	std::string path_root;
	std::set<std::string> allow_methods;
	bool auto_index;
	int  auto_index_time;
	std::string	path_upload_root;
	std::string	acceptable_upload;
	int	acceptable_upload_time;
	std::string redirection;
}              route;


std::vector<std::string> extract_words_in_vector(std::string &s);
int check_substring(std::string s, std::string s1);
int count_words(std::string str);
std::string extract_word_from_line(int &end, std::string &line);
std::string get_client_file(char *buffer);
void extract_info_from_rest(Client_Request &obj, std::string full_request);
Conf manage_config_file(std::stringstream &file);
std::string get_time();
std::string response_str(Client_Request &obj);
std::string get_client_file(char *buffer);
void check_err_page_validity(std::string file);
void extract_info_from_first_line(Client_Request &obj, std::string full_request);

void echange_with_client(int &server_fd, struct sockaddr_in &address, Conf &web_conf);

std::map<int, std::string> init_status_code_message_map();

int open_file(std::ifstream &s, std::string path);
void open_conf(int ac, char **av, std::ifstream &file);
std::vector<Conf> get_all_server_conf(int ac, char **av);
void manage_request_status(route &r, Client_Request &obj, Conf &web_conf, cl_response &fd_rep);

void set_length_and_content(std::ifstream &myfile, Client_Request &obj);
void delete_request(Client_Request &obj);
void set_request_status_nb_message(int status_nb, Client_Request &obj);

void set_error(Client_Request &obj, Conf &web_conf, int status_code_nb);
void manage_route(std::stringstream &file, std::string &line, std::map<std::string, route> &m);
int method_is_not_allow(route &r, Client_Request &obj);
int extension_is_not_exist(std::string *mylist, std::string extension, int size);
int file_no_read_permission(route &r, Client_Request &obj);
int file_no_write_permission(std::string filepath, std::string path);
int file_not_exist(Client_Request &obj);
int file_is_web_based(std::string type);
int file_extension_is_managed(std::string type);
int file_is_text_based(std::string type);
std::string get_file_output(Client_Request &o);
std::vector<size_t> get_occurences_end_indexes(std::string s, std::string sub);
std::vector<size_t> get_occurences_indexes(std::string s, std::string sub);
int get_closing_bracket_index(std::string s, int i_start);
int is_whitespace(char c);
void remove_fst_white_space(std::string &line);
int find_first_of_whitespace(std::string &line);
int is_only_whitespace(std::string &s);
int cast_as_int(std::string s);
void show_err_message_and_quite(std::string message);

//contain port, server_name, all err page
class Conf
{
//in fact, no need to put as privee
public:
	std::set<int> port;
	int client_max_body_size;
	std::string server_name;
	std::map<int, std::string>conf_map;//status code, error_page_path
	std::map<std::string, route> m_location;
	std::string default_file;
	Conf(): client_max_body_size(0), default_file("index.html"){};
	~Conf(){};
	Conf(Conf const &s){*this = s;}
	Conf &operator=(Conf const &src)
	{
		this->port = src.port;
		this->client_max_body_size = src.client_max_body_size;
		this->m_location = src.m_location;
		this->server_name = src.server_name;
		this->conf_map = src.conf_map;
		this->default_file = src.default_file;
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
		for (std::set<int>::iterator it=port.begin(); it!=port.end(); ++it)
			std::cout << "port " << *it << std::endl;
		std::cout << "server_name " << this->server_name << std::endl;
		std::cout << "index " << this->default_file << std::endl;
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
	std::string method;
	std::string clean_relative_path;//remove extra / from url
	bool dir_list;
	std::string file;
	std::string origin_path;//in url
	std::string	query_string;
	std::string f_extension;
	int 		status_code_nb;
	std::string status_code_message;
	unsigned long total_nb;
	std::string payload;
	std::string body_response;
	std::map<std::string, std::string> client_request;
	std::map<std::string, std::string> cgi_output;
	std::map<std::string, std::string> custom_headers;
public:
	Client_Request():method("GET"), dir_list(false), file(""), origin_path(""), f_extension(""), status_code_nb(200),status_code_message("200 OK"), total_nb(0), payload(""), body_response(""){}
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
		this->body_response = src.body_response;
		this->cgi_output = src.cgi_output;
		this->custom_headers = src.custom_headers;
		this->client_request = src.client_request;
		return *this;
	}
	std::string get_client_method(){return this->method;}
	std::string get_client_ask_file(){return this->file;}
	std::string get_file_extension(){return this->f_extension;}
	int get_status_code_nb(){return this->status_code_nb;}
	std::string get_status_code_message(){return this->status_code_message;}
	unsigned long get_total_nb(){return this->total_nb;}
	std::string get_body_response(){return this->body_response;}
	std::string	get_query_string(){return this->query_string;}
	std::map<std::string, std::string> get_client_request_map(){return this->client_request;}
	std::map<std::string, std::string> get_cgi_output_map(){return this->cgi_output;}

	void set_client_method(std::string &src){this->method = src;}
	void set_client_file(std::string &src){this->file = src;}
	void set_file_extension(std::string src){this->f_extension = src;}
	void set_status_code_nb(int src){this->status_code_nb = src;}
	void set_status_code_message(std::string &src){this->status_code_message = src;}
	void set_total_nb(unsigned long src){this->total_nb = src;}
	void set_body_response(std::string &src){this->body_response = src;}
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

typedef struct s_response {
	s_response() {
		this->boundary = "";
		this->content_length = -1;
		this->filename = "";
		this->content_type = "";
		this->content = "";
		this->payloads = "";
		this->unparsed_payloads = "";
		this->ready = false;
		this->route_path = "";
		this->file_extension = "";
	}
	bool ready;
	std::string boundary;
	int content_length;
	std::string filename;
	std::string content_type;
	std::string content;
	std::string payloads;
	std::string unparsed_payloads;
	Conf conf;
	std::string route_path;
	std::string file_extension;
	Client_Request obj;
}           cl_response;

#endif
