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
	std::vector<Conf> web_conf_vector = get_all_server_conf(ac, av);
	Server server(web_conf_vector);
	server.Start();
    return 0;
}
