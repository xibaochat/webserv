#include "webserv.hpp"
#include "server.hpp"

/*
**extract info from configuration file passed as scd argument
**create and init socket, bind and listen, then exchange with user
 */
int main(int ac, char **av)
{
	//###parse nginx conf
	Conf web_conf = manage_config_file(ac, av);

	Server server(web_conf);
	server.Start(web_conf);
    return 0;
}
