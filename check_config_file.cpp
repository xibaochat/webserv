#include "webserv.hpp"

//I create a strucutr contains port, serve_name,
//and a map, key is the int status_code, value is a vector, fst ele is status_message, is corresponding file Example:  status_code->400 vector[0] ->"400 Bad Request" vector[1]->400.html

void open_conf(int ac, char **av, std::ifstream &file, Conf &web_conf)
{
	std::string conf_file;
	if (ac == 1)
		conf_file = "conf/default.conf";
	else
		conf_file = av[1];
	file.open(conf_file.c_str(), std::ios::in);
	if (!file.is_open())
	{
		std::cout << "Cannot open config file" << std::endl;
		exit(EXIT_FAILURE);
	}
	web_conf.set_conf_file(conf_file);
}

void remove_fst_white_space(std::string &line)
{
	std::size_t i = 0;
	while (line[i] && line[i] == ' ')
		i++;
	line.erase(0, i);
}
int find_first_of_whilespace(std::string &line)
{
	int i = 0;
	while (line[i] && line[i] != ' ')
		i++;
	return i;
}

void store_elem_in_vec(std::ifstream &file, std::vector<std::string> &vec)
{
	std::string line;
	int pos = 0;
	while (getline(file, line))
	{
		while(line.length() > 0)
		{
			remove_fst_white_space(line);
			int end = find_first_of_whilespace(line);
			std::string elem = line.substr(0, end - pos);
			vec.push_back(elem);
			line.erase(0, end + 1);
		}
	}
}

void check_port_nb(std::vector<std::string> &vec, Conf &web_conf)
{
	std::vector<std::string>::iterator it;
	if ((it = std::find(vec.begin(), vec.end(), "port")) != vec.end())
	{
		stringstream ss;
		int nb;
		if ((it + 1) != vec.end())
		{
			ss << *(it + 1);
			ss >> nb;
			if (nb > 0)
			{
				web_conf.set_port(nb);
				vec.erase(it, it + 2);
				return ;
			}
		}
	}
	std::cout << "Invalide port number" << std::endl;
	exit(EXIT_FAILURE);
}

void set_server_name(std::vector<std::string> &vec, Conf &web_conf)
{
	std::vector<string>::iterator it;
	if ((it = std::find(vec.begin(), vec.end(), "server_name")) != vec.end())
	{
		std::cout << YELLOW << "|" << *it << NC << "|\n";
		if ((it + 1) != vec.end())
		{
			web_conf.set_server_name((*(it + 1)));
			vec.erase(it, it + 2);
			std::cout << YELLOW << "|" << *(it + 1) << NC << "|\n";
			return ;
		}
	}
	std::cout << "Invalide serve name in conf" << std::endl;
	exit(EXIT_FAILURE);
}

void check_err_page_validity(std::string file)
{
	std::ifstream ifs;
	ifs.open(file.c_str(), std::ios::in);
    if (!ifs.is_open())
    {
        std::cout << "Cannot open " << file << " error file" << std::endl;
        exit(EXIT_FAILURE);
    }
	ifs.close();
}

int get_status_code(std::vector<std::string>::iterator it)
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

void store_err_page_info(std::vector<std::string> &vec, Conf &web_conf)
{
	std::map<int, std::vector<std::string> > map;
	int j;
	std::vector<std::string>::iterator it;
	while ((it = std::find(vec.begin(), vec.end(), "error_page")) != vec.end())
	{
		std::stringstream sstr;
		int status_code;
		if (it + 1 != vec.end() && it + 2 != vec.end())
		{
			status_code = get_status_code(it);
			std::string message;
			for(j = 2; (*(it + j))[(*(it + j)).length() - 1] != '"'; j++)
				message += *(it + j) + " ";
			message += *(it + j);//"400 till Request"
			message.erase(remove(message.begin(), message.end(), '"'), message.end()); //remove "
			map[status_code].push_back((message));
			std::cout << BLUE << message << NC << "\n";
			//scd is err page file
			if (it + j + 1 != vec.end())
			{
				check_err_page_validity((*(it + j + 1)));
				map[status_code].push_back((*(it + j + 1)));
				std::cout << GREEN << (*(it + j + 1)) << NC << "\n";
				vec.erase(it, it + j + 2);
			}
		}
		else
		{
			std::cout << "Invalid conf form" << std::endl;
			exit(EXIT_FAILURE);
		}

	}
	web_conf.set_config_map(map);
}

void check_config_file(int ac, char **av)
{
	Conf web_conf;
	std::vector<std::string> vec;
	std::ifstream file;
	std::string conf_file;
	std::vector<std::string>::iterator it;
	open_conf(ac, av, file, web_conf);//can open file?
	store_elem_in_vec(file, vec);
	for (std::vector<string>::iterator it = vec.begin() ; it != vec.end(); ++it)
		std::cout << "|" << *it << "|";
	check_port_nb(vec, web_conf);
	set_server_name(vec, web_conf);
	store_err_page_info(vec, web_conf);
	file.close();
	if (vec.size() != 0)
	{
		std::cout << "Invalid conf form" << std::endl;
		exit(EXIT_FAILURE);
	}
	//check
	std::cout << web_conf.get_port() << "\n";
	cout << web_conf.get_server_name() << "\n";
	cout << web_conf.get_conf_file() << "\n";

	std::map<int, std::vector<std::string> > mymap = web_conf.get_conf_map();
	for (std::map<int, std::vector<std::string> >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	{
		std::cout << "status code: " << it->first << "message err" << " => " << it->second[0] << "corresponding page " << " => " << it->second[1]<< '\n';
	}

}
