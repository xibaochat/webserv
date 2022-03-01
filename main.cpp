#include "webserv.hpp"
#include "server.hpp"

std::map<int, std::string> init_status_code_message_map()
{
    std::map<int, std::string> _map;

	_map[200] = "200 OK";
	_map[204] = "204 No Content";
    _map[400] = "400 Bad Request";
    _map[403] = "403 Forbidden";
    _map[404] = "404 Not Found";
    _map[405] = "405 Method Not Allowed";
    _map[410] = "410 Gone";
    _map[413] = "413 Request Entity Too Large";
    _map[500] = "500 Internal Server Error";
	_map[501] = "501 Not Implemented";
	_map[503] = "503 Service Unavailable";
    return _map;
}

/*
**extract info from configuration file passed as scd argument
**create and init socket, bind and listen, then exchange with user
 */
int main(int ac, char **av)
{
	//###parse nginx conf
	// Conf web_conf = manage_config_file(ac, av);


	// Get conf file as string
	std::stringstream file_content;
	std::ifstream t1("conf/default.conf");
    file_content << t1.rdbuf();
	Conf web_conf = manage_config_file(file_content);
	Server server(web_conf);



	// Get `server` indexes
	std::string s_content = file_content.str();
    std::vector<size_t> indexes = get_occurences_indexes(s_content, "server ");
    std::vector<size_t> tmp_v = get_occurences_indexes(s_content, "server{");
    indexes.insert(indexes.end(), tmp_v.begin(), tmp_v.end());

	// Extract
	std::vector<std::string> servers_content;
    for (std::size_t i = 0; i != indexes.size(); ++i)
    {
        int closing_i = get_closing_bracket_index(s_content, indexes[i]);
		std::stringstream curr_server_content;
		curr_server_content << s_content.substr(indexes[i] + 1, closing_i - indexes[i] - 1);
		Conf curr_web_conf = manage_config_file(curr_server_content);
		server.web_conf_vector.insert(server.web_conf_vector.end(), curr_web_conf);
	}

	server.Start(web_conf);
    return 0;
}
