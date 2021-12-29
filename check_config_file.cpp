#include "webserv.hpp"

//i create a strucutr contains port, serve_name,
//and a map, key is the int status_code, value is a vector, fst ele is status_message, scd //is corresponding file
void check_config_file(int ac, char **av)
{
	char *path;
	std::string line;
	Conf web_conf;
	std::vector<std::string> vec;
	std::ifstream file;
	std::string conf_file;
	if (ac == 1)
	{
		file.open("conf/default.conf", std::ios::in);
		conf_file = "conf/default.conf";
	}
	else
	{
		path = av[1];
		int len = strlen(av[1]);
		char *conf_f = (char *)malloc(sizeof(char) * (len + 1));
		conf_f[len] = '\0';
		strcpy(conf_f, av[1]);
		conf_file = conf_f;
		file.open(path, std::ios::in);
	}

	if (!file.is_open())
	{
		std::cout << "No config file" << std::endl;
		exit(EXIT_FAILURE);
	}
	web_conf.set_conf_file(conf_file);
	std::size_t pos = 0;
	while (getline(file, line))
	{
		while((pos = line.find_first_not_of(' ')) != std::string::npos)
		{
			std::size_t end = line.find_first_of(' ');
			if (end != std::string::npos)
			{
				std::string elem = line.substr(pos, end - pos);
				vec.push_back(elem);
				line.erase(0, end + 1);
			}
			else
			{
				vec.push_back(line.substr(0, line.length()));
				line.erase(0, line.length());
			}
		}
	}
	for (std::vector<string>::iterator it = vec.begin() ; it != vec.end(); ++it)
		std::cout << "|" << *it << "|";
	std::vector<std::string>::iterator it;
	if ((it = std::find(vec.begin(), vec.end(), "port")) != vec.end())
	{
		stringstream ss;
		int nb;
		ss << *(it + 1);
		ss >> nb;
		web_conf.set_port(nb);
		vec.erase(it, it + 3);
	}
	std::cout << RED << "BOID0\n";
	if ((it = std::find(vec.begin(), vec.end(), "server_name")) != vec.end())
	{
		web_conf.set_server_name((*(it + 1)));
		vec.erase(it, it + 2);
	}
	std::cout << RED << "BOID1\n";
	int j;
	while ((it = std::find(vec.begin(), vec.end(), "error_page")) != vec.end())
	{
		stringstream sstr;
		int status_code;
		sstr << *(it+1);
		sstr >> status_code;
		std::string message;
		for(j = 2; (*(it + j))[(*(it + j)).length() - 1] != '"'; j++)
			message += *(it + j) + " ";
		message += *(it + j);
		web_conf.get_conf_map()[status_code].push_back((message));
		std::cout << BLUE << message << NC << "\n";
		web_conf.get_conf_map()[status_code].push_back((*(it + j + 1)));
		std::cout << BLUE << (*(it + j + 1)) << NC << "\n";
		vec.erase(it, it + j + 2);
	}
	file.close();
	for (std::vector<string>::iterator it = vec.begin() ; it != vec.end(); ++it)
		std::cout << ' ' << *it;
}
