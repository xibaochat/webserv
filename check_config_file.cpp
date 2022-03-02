#include "webserv.hpp"

//I create a strucutr contains port, serve_name,
//and a map, key is the int status_code, value is path of error page, Example:  status_code->400, value ->400.html

/*
** :param (int) ac: number of the server's argument
** :param (char **) av: server's argument
** :return (std::string) conf_file: path of configuration file
*/
std::string get_conf_file(int ac, char **av)
{
	std::string conf_file;
	if (ac == 1)
	{
		conf_file = "conf/default.conf";
		return conf_file;
	}
	conf_file = av[1];
	return conf_file;
}

void open_conf(int ac, char **av, std::ifstream &file)
{
	std::string conf_file = get_conf_file(ac, av);

	file.open(conf_file.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cout << "Cannot open config file" << std::endl;
		exit(EXIT_FAILURE);
	}
}


bool isNumber(const string& str)
{
    return str.find_first_not_of("0123456789") == string::npos;
}

void show_err_message_and_quite(std::string message)
{
	std::cout << message << std::endl;
	exit(EXIT_FAILURE);
}

/*
** Check validity of key, it is in white list or not
**
** :param (std::string) elem: first word extracted that represent key in the line
** :return (bool)
*/

bool invalid_key(std::string &elem)
{
	std::vector<std::string>::iterator it;
	std::string arr[] = VALID_CONF_KEYWORDS;
	std::vector<std::string> key_vec(arr, arr + sizeof(arr)/ sizeof(std::string));
	if ((it = std::find(key_vec.begin(), key_vec.end(), elem)) == key_vec.end())
		return true;
	return false;
}

/*
** Check except key error_page, others are unique or not
** param (std::string) elem: first word(key) extracted key in the line
** std::vector<std::string> &vec: vec contains all elem from configuration file
** return bool to
**

*/
int repeated_key(std::string &elem, std::vector<std::string> &vec)
{
	std::vector<std::string>::iterator it;

	if (elem != "error_page")
	{
		for (it= vec.begin(); it!= vec.end(); ++it)
		{
			if (*it == elem)
				return true;
		}
	}
	return false;
}

/*
**  check key is in white list and it is repeated or not
*/

void check_key_validity(std::string elem, std::vector<std::string> &vec)
{
	std::vector<std::string>::iterator it;

	if (invalid_key(elem))
		show_err_message_and_quite("Key word " + elem + " in file is invalid");
}

bool repeated_status_code(std::string &elem, std::vector<std::string> &vec)
{
	std::vector<std::string>::iterator it;
	for (it= vec.begin(); it!= vec.end(); ++it)
	{
		if ((*it == "error_page") && (it + 1 != vec.end()) && (*(it + 1) == elem))
			return true;
	}
	return false;
}

/*
** param (int)i: Nth of word in line, (std::string)elem: extracted word, vec: vector store all elem)
** check 2nd elem in the line has key: error_page is a nb OR it is already repeated
 */
void check_line_error_page(int i, std::string &elem, std::vector<std::string> &vec)
{
	if (i == 2 && (isNumber(elem) == false))
		show_err_message_and_quite("error_page does not have a valid status code");
	if (i ==2 && repeated_status_code(elem, vec))
		show_err_message_and_quite("Repeated status code in config file");
	if (i == 3)
		check_err_page_validity(elem);
}
/*
** param(std::string): each line from configuration file
** return (std::string) each single elem
 */
std::string extract_word_from_line(int &end, std::string &line)
{
	std::string elem;
	remove_fst_white_space(line);
	end = find_first_of_whitespace(line);
	elem = line.substr(0, end);
	return elem;
}

/*
** param (string)key: elem in whitelist, (int) i: number of elem in the line
** return bool , check line start by key has extra elem or not
*/
bool has_extra_elem_in_line(std::string &key, int &i)
{
	if (key == "location")
		return false;
	if ((key == "error_page" && i != 3) || ((key != "error_page") && i != 2))
	{
		cout << YELLOW << key << " " << i << NC << "\n";
		return true;
	}
	return false;
}

void store_elem_in_vec(std::stringstream &file, std::vector<std::string> &vec, std::map<std::string, route> &m)
{
	std::string line, key, elem;
	int end = 0;

	while (getline(file, line))
	{
		if (line == "")
			continue;
		int i = 0;
		while(line.length() > 0)
		{
			++i;
			elem = extract_word_from_line(end, line);
			if (i == 1) // check key in the white list or not
			{
				check_key_validity(elem, vec);
				key = elem;
			}
			if (key == "location")
			{
				manage_route(file, line, m);
				break;
			}
			if (key == "error_page")
				check_line_error_page(i, elem, vec);
			vec.push_back(elem);
			line.erase(0, end + 1);
		}
		if (has_extra_elem_in_line(key, i))
			show_err_message_and_quite("Starting webserver: failed because of " + key + " in the configuration");
	}
}

/*transfer value in the form string to int*/
int get_transfered_value(std::stringstream &ss, std::vector<std::string>::iterator &it)
{
	int nb;

	 ss << *(it + 1);
	 ss >> nb;
	 return nb;
}
/*
** find (int)value corresponds the key(port, max buffer size) and check its validity
** param: (string&) key in conf
*/
int manage_key_correspond_value(std::string &key, std::vector<std::string> &vec)
{
	std::vector<std::string>::iterator it;
	std::stringstream ss;
	std::string message_error;
	int nb;

	if ((it = std::find(vec.begin(), vec.end(), key)) != vec.end())
	{
		if ((it + 1) != vec.end())
		{
			if (isNumber(*(it + 1)) == false)
			{
				ss << "Invalide " << key << " number";
				message_error = ss.str();
				show_err_message_and_quite(message_error);
			}
			nb = get_transfered_value(ss, it);
			if (nb >= 1 && nb <= 65535)
			{
				vec.erase(it, it + 2);
				return nb;
			}
		}
	}
	return (-1);
}

/*
** check port is number and set value
** param vector: a temp vec contains all elem extracted from conf file,
** (Conf &) web_conf: a structure to store info from confuguration file
 */

void manage_port(std::vector<std::string> &vec, Conf &web_conf)
{
	std::vector<std::string>::iterator it;
	std::string key("listen");
	int nb_port;
	std::stringstream ss;
	std::string message_error;

	if ((it = std::find(vec.begin(), vec.end(), "listen")) == vec.end())
	{
		ss << "No " << key << " number";
		message_error = ss.str();
		show_err_message_and_quite(message_error);
	}
	while ((nb_port = manage_key_correspond_value(key, vec)) != -1)
		web_conf.port.insert(nb_port);
}

/*
** check max buffer size is number and store value to web_conf
*/
// void manage_client_max_body_size(std::vector<std::string> &vec, Conf &web_conf)
// {
// 	 std::vector<std::string>::iterator it;
// 	 std::string key("client_max_body_size");
// 	 int client_max_body_size  = manage_key_correspond_value(key,  vec);
// 	 web_conf.set_client_max_body_size(client_max_body_size);
// }

/*
** Extract and return `server_name` if valid
** param: vec: a temp vec who contains all elem extracted from conf
**
*/
void manage_server_name(std::vector<std::string> &vec, Conf &web_conf)
{
	std::vector<string>::iterator it;
	if ((it = std::find(vec.begin(), vec.end(), "server_name")) != vec.end())
	{
		if ((it + 1) != vec.end())
		{
			web_conf.set_server_name((*(it + 1)));
			vec.erase(it, it + 2);
			return ;
		}
	}
	std::cout << "Invalide serve name in conf" << std::endl;
	exit(EXIT_FAILURE);
}

std::string get_location_path(std::string &line)
{
	int end = 0;
	int i = 0;
	std::string elem, path;
	while (line.length() > 0)
	{
		elem = extract_word_from_line(end, line);
		if (i == 1)
			path = elem;
		if (i == 2 && elem != "{")
			show_err_message_and_quite("[Error]Wrong format in location in config file");
		line.erase(0, end + 1);
		i++;
	}
	if (i != 3)//location is composed of 3 part, -> location PATH {
		show_err_message_and_quite("[Error]Wrong format in location in config file");
	return path;
}

std::string get_root(std::string &line)
{
	int end = 0;
	int i = 0;
	std::string elem, path;
	while (line.length() > 0)
	{
		elem = extract_word_from_line(end, line);
		if (i == 1)
			path = elem;
		line.erase(0, end + 1);
		i++;
	}
	std::string::size_type pos = path.find(';');
    if (pos != std::string::npos)
        path =  path.substr(0, pos);
	if (i != 2)
		show_err_message_and_quite("[Error]Wrong format in location in config file");
	return path;
}

std::set<std::string> set_method(std::string &line)
{
	int i = 0;
	int end = 0;
	std::string elem;
	std::set<std::string> methods_set;
	std::string::size_type pos;
	while (line.length() > 0)
	{
		elem = extract_word_from_line(end, line);
		if (i >= 1)
		{
			pos = elem.find(';');
			if (pos != std::string::npos)
				elem =  elem.substr(0, pos);
			methods_set.insert(elem);
		}
		line.erase(0, end + 1);
		i++;
	}
	return methods_set;
}

void manage_auto_index(int &end, std::string &path, std::string &line, std::map<std::string, route> &m)
{
	if (line != AUTO_ON && line != AUTO_OFF)
	{
		std::cerr << "[ERROR]autoindex in config file is wrong" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (m[path].auto_index_time == 0)
	{
		if (line == "autoindex on;")
			m[path].auto_index = true;
		else
			m[path].auto_index = false;
		m[path].auto_index_time = 1;
		return ;
	}
	std::cerr << "[ERROR]autoindex in config file is doubled" << std::endl;
	exit(EXIT_FAILURE);
}

void manage_acceptable_upload(std::string &path, std::string &line, std::map<std::string, route> &m)
{
	if (line != UPLOAD_ON && line != UPLOAD_OFF)
	{
		std::cerr << "[ERROR]upload in config file is wrong" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (m[path].acceptable_upload_time == 0)
	{
		m[path].acceptable_upload = get_root(line);
		m[path].acceptable_upload_time = 1;
		return ;
	}
	std::cerr << "[ERROR]upload in config file is doubled" << std::endl;
	exit(EXIT_FAILURE);
}

void manage_route(std::stringstream &file, std::string &line, std::map<std::string, route> &m)
{
	int end = 0;
	int i = 0;
	std::string elem, path, root, upload_root;

	path = get_location_path(line);
	while (getline(file, line))
	{
		i = 0;
		if (is_only_whitespace(line))
			continue;
		elem = extract_word_from_line(end, line);
		if (elem == "}")
			break;
		else if (elem == "location")
		{
			path = get_location_path(line);
		}
		else if (elem == "root")
		{
			root = get_root(line);
			m[path].path_root = root;
		}
		else if (elem == "autoindex")
			manage_auto_index(end, path, line, m);
		else if (elem == "AllowMethods")
		{
			if (!m[path].allow_methods.size())
				m[path].allow_methods = set_method(line);
			else
			{
				std::cerr << "[ERROR] AllowMethods is doubled in config" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else if (elem == "upload_dir")
		{
			upload_root = get_root(line);
			m[path].path_upload_root = upload_root;
		}
		else if (elem == "upload")
			manage_acceptable_upload(path, line, m);
		else
		{
			std::cerr << "[ERROR] Inaccpeted element in config" << elem << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

/*
** Check the path of error page can be opened or not
** param: (std::string)path_file : the path of error page
*/

void check_err_page_validity(std::string path_file)
{
	std::ifstream ifs;
	ifs.open(path_file.c_str(), std::ios::in);
    if (!ifs.is_open())
    {
        std::cout << "Cannot open " << path_file << " error file" << std::endl;
        exit(EXIT_FAILURE);
    }
	ifs.close();
}
/*
** Extract error status from given string and check it's validity
** before returning it as Integrer
**
** param (vector::iterator) it: vector contraining string to extract code from
** return (int): extracted status code
**
 */
int extract_status_code(std::vector<std::string>::iterator it)
{
	int status_code;
	std::stringstream sstr;
	sstr << *(it+1);
	sstr >> status_code;
	if (status_code < 200)
	{
		std::cout << "Invalid status code in conf" << std::endl;
		exit(EXIT_FAILURE);
	}
	return status_code;
}

/*
** Extract and store `error_page` conf in a new map,
** then store the map in the given `web_conf` parameter
**
** param (std::vector) vec: vector temporaire containing all extracted elems
** param (Conf &) web_conf: structure containing all info of configuration file
*/
void set_err_page_map(std::vector<std::string> &vec, Conf &web_conf)
{
	std::map<int, std::string> map;
	int status_code;
	std::vector<std::string>::iterator it;
	while ((it = std::find(vec.begin(), vec.end(), "error_page")) != vec.end())
	{
		status_code = extract_status_code(it);
		if (it + 2 != vec.end())
		{
			map[status_code] = (*(it + 2));
			vec.erase(it, it  + 3);
		}
	}
	web_conf.set_config_map(map);
}

/*
** Validate and Extract configuration from conf file
**
** :param (int) ac: number of the server's argument
** :param (char **): server's argument
** :return (Conf) web_conf: object containing all extracted configuration
*/

Conf manage_config_file(std::stringstream &file)
{
	Conf web_conf;
	std::vector<std::string> vec;  // store each word in vec
	std::string conf_file;
	std::vector<std::string>::iterator it;
	std::map<std::string, route> m;

	// Conf extraction
	// open_conf(ac, av, file);//can open file?

	// std::map<std::int, std::string> servers;
	// servers = extract_servers_as_string(file);
	store_elem_in_vec(file, vec, m);
	web_conf.m_location = m;
	manage_port(vec, web_conf);
	manage_server_name(vec, web_conf);
//	manage_client_max_body_size(vec, web_conf);
	set_err_page_map(vec, web_conf);

	if (vec.size() != 0)
	{
		std::cout << "Invalid conf form" << std::endl;
		exit(EXIT_FAILURE);
	}
	web_conf.display_conf_file_debug();
	return web_conf;
}
