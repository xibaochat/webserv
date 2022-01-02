#include "webserv.hpp"

std::map<int, std::string> init_status_code_message_map()
{
    std::map<int, std::string> map;

	map[200] = "200 OK";
    map[400] = "400 Bad Request";
    map[403] = "403 Forbidden";
    map[404] = "404 Not Found";
    map[405] = "405 Method Not Allowed";
    map[410] = "410 Gone";
    map[413] = "413 Request Entity Too Large";
    map[500] = "500 Internal Server Error";
    return map;
}
