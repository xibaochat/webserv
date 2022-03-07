#include "webserv.hpp"


/*
** Extract and return configuration filepath from arguments or
** return default conf filepath if not arguments are provided
**
** :param (int) ac: number of program's arg
** :param (char **) av: program's arguments
** :return (std::string) conf_file: path of configuration file
*/
std::string get_conf_filepath(int ac, char **av)
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


/*
** Open configuration file (includes management of default conf file)
** and store it in given `file` parameter
**
** :param (int) ac: number of program's arg
** :param (char **) av: program's arguments
** :param (std::ifstream) file: where the open file will be stored
** :return: X
*/
void open_conf(int ac, char **av, std::ifstream &file)
{
	std::string conf_file = get_conf_filepath(ac, av);

	file.open(conf_file.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cout << "Cannot open config file" << std::endl;
		exit(EXIT_FAILURE);
	}
}


/*
** Read and return configuration file as string.
** Will try to use argument as conf file or default one if nothing is provided.
**
** :param (int) ac: number of program's arg
** :param (char **) av: program's arguments
** :return (std::string) : file content
*/
std::string get_conf_file_as_string(int ac, char **av)
{
	std::stringstream file_content;
	std::ifstream f;
	open_conf(ac, av, f);
    file_content << f.rdbuf();
	return (file_content.str());
}


/*
** Found and return the beginning (=index) of all server configuration (=index of `{` + 1)
**
** :param (std::string) s_content: configuration file
** :return (std::vector<size_t>) : all indexes of servers' configuration
*/
std::vector<size_t> get_server_occurences_indexes(std::string s_content)
{
    std::vector<size_t> indexes = get_occurences_end_indexes(s_content, "server{");
	for (int x = 100; x > 0; x--)
	{
		std::string tmp(x, ' ');
		std::vector<size_t> tmp_v = get_occurences_end_indexes(s_content, "server" + tmp + "{");
		indexes.insert(indexes.end(), tmp_v.begin(), tmp_v.end());
	}
	return (indexes);
}

/*
** Extract servers' conf from given string and store the parsed
** configuration in a `vector` of `Conf`
**
** :param (std::string) s_content: configuration file
** :param (std::vector<size_t>): all indexes of servers' configuration
** :return (std::vector<Conf>): all servers' conf as `Conf` objects
 */
std::vector<Conf> get_vector_of_server_conf(std::string s_content, std::vector<size_t> server_indexes)
{
	int closing_i;
	Conf curr_web_conf;
	std::vector<Conf> web_conf_vector;

    for (std::size_t i = 0; i != server_indexes.size(); ++i)
    {
		closing_i = get_closing_bracket_index(s_content, server_indexes[i]);
		std::stringstream curr_server_content;
		curr_server_content << s_content.substr(server_indexes[i], closing_i - server_indexes[i] - 1);
		curr_web_conf = manage_config_file(curr_server_content);
		web_conf_vector.push_back(curr_web_conf);
	}
	return (web_conf_vector);
}


/*
** 1. Manage configuration filepath based on program's arguments or default conf file
** 2. Extract all individual server configuration from conf file
** 3. Parse each server configuration and store them in `Conf` objects
*
** :param (int) ac: number of program's arg
** :param (char **) av: program's arguments
** :return (std::vector<Conf>): all servers' conf as `Conf` objects
*/
std::vector<Conf> get_all_server_conf(int ac, char **av)
{
	std::string s_content = get_conf_file_as_string(ac, av);
    std::vector<size_t> server_indexes = get_server_occurences_indexes(s_content);
	return (get_vector_of_server_conf(s_content, server_indexes));
}
