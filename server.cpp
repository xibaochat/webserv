#include "webserv.hpp"

void init_socket(int port, int &server_fd, struct sockaddr_in &address)
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
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

void open_file(std::ifstream &myfile, int code, Conf &web_conf)
{
	std::string page_path = web_conf.get_conf_err_page_map()[code];
	myfile.open(page_path.c_str(), std::ios::in);
}

/*
**check file is valid or not
**return (string)corresponding status maeeage
*/
std::string get_status_nb_message(std::ifstream &myfile, std::string &file, Conf &web_conf)
{
	std::string status_nb_message;
	std::map<int, std::string> error_code_message_map = init_status_code_message_map();
	myfile.open(file.c_str(), std::ios::in);
	if (myfile.is_open())
	{
		status_nb_message = "200 OK";
		std::cout << "\nOK\n";
	}
	else
	{
		int status_code_nb = 404;
		status_nb_message = error_code_message_map[status_code_nb];
		open_file(myfile, status_code_nb, web_conf);
	}
	return status_nb_message;
}

/*
** read user asked file, and store total length and content
*/
void set_length_and_content(std::ifstream &myfile, Client_Request &obj)
{
	std::string line;
	std::string content_page;
	while (getline(myfile, line))
	{
		line += "\n";
		content_page += line;
	}
	obj.set_total_nb(content_page.length());
	obj.set_total_line(content_page);
	myfile.close();
}

/*
** From fst line of buffer, extract info of method; client asked file; and status_code
** :param (Client_Request) obj: uninitialized obj
** :param (char *) buffer that from client, (Conf) configuration file passed as scd parameter
 */
//extract method; client asked file; and status_code of file(file valid?)
void extract_info_from_first_line_of_buffer(Client_Request &obj, char *buffer, Conf &web_conf)
{
	std::ifstream myfile;
	char *ptr = strstr(buffer, " ");//GET , POST ?
	std::string method(buffer, 0, ptr - buffer);
	obj.set_client_method(method);
	std::string file = get_client_file(buffer);//the file client ask
	obj.set_client_file(file);
	std::string status_nb_message = get_status_nb_message(myfile, file, web_conf);//file valid?
	obj.set_status_code(status_nb_message);
	set_length_and_content(myfile, obj);//even if not valid, we send 404.html
}

/*
**extract info from configuration file passed as scd argument
**create and init socket, bind and listen, then exchange with user
 */
int main(int ac, char **av)
{
	//###parse nginx conf
	Conf web_conf = manage_config_file(ac, av);
	int server_fd;

    struct sockaddr_in address;
	init_socket(web_conf.get_port(), server_fd, address);
	bind_and_listen(server_fd, address);
	while(1)
    {
		echange_with_client(server_fd, address, web_conf);
    }
    return 0;
}
