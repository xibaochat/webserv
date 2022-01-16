#include "webserv.hpp"
#include "server.hpp"

void init_socket(int port, int &server_fd, struct sockaddr_in &address)
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);
}

void bind_and_listen(int &server_fd, struct sockaddr_in &address)
{
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

std::string get_file(char *data, int i)
{
	std::string file(data, 1, i - 1);
	return file;
}

/*
**  extract client asked file name from buffer
**  if no file, by default, file is "cute_cat.html"
**  return (string)file name
*/
std::string get_client_file(char *buffer)
{
	std::string file("cute_cat.html");
	char *data = strstr(buffer, "/" );
	int i = 0;
	while (data[i] && data[i] != ' ')
		i++;
	if (i != 1)
		file = get_file(data, i);
	return file;
}

/*
**extract info from configuration file passed as scd argument
**create and init socket, bind and listen, then exchange with user
 */
int main(int ac, char **av)
{
	//###parse nginx conf
	Conf web_conf = manage_config_file(ac, av);
/*	int server_fd;

    struct sockaddr_in address;

	init_socket(web_conf.get_port(), server_fd, address);
	bind_and_listen(server_fd, address);
	while(1)
    {
		echange_with_client(server_fd, address, web_conf);
		}*/
	Server server(web_conf);
	try
	{
		server.Start(web_conf);
	}
	catch(const char *s)
	{
		std::cerr << s << std::endl;
	}
    return 0;
}
