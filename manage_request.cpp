#include "webserv.hpp"

void open_file(std::ifstream &myfile, std::string path)
{
	myfile.open(path.c_str(), std::ios::in);
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

void send_response(Client_Request &obj, int &new_socket)
{
	std::string response = response_str(obj);
	const char *new_str = response.c_str();
	std::cout << "Start sending repond for `" << RED << new_socket << NC << "`\n";
	int res = send(new_socket , new_str , strlen(new_str), 0);
	// todo: MANAGE SEND ERROR HERE
	std::cout << res << endl;
	std::cout << "End sending repond for `" << RED << new_socket << NC << "`\n";
}

/*
** selon err code, get return page path, and also init obj's file total nb and content
** :param (int) error_code: code to manage
** :param (Client_Request) obj: a pure obj without init
** :param (Conf) web_conf: configuration structure obj
** return: void
 */
void send_error_page(int error_code, Client_Request &obj, Conf &web_conf, int &new_socket)
{
	std::ifstream ss;
	std::map<int, std::string> error_code_message_map;
	std::string status_nb_message ;

	open_file(ss, web_conf.get_conf_err_page_map()[error_code]);
	set_length_and_content(ss, obj);
	error_code_message_map = init_status_code_message_map();
	status_nb_message = error_code_message_map[error_code] + "\r\n";
	obj.set_status_code(status_nb_message);
	send_response(obj, new_socket);
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
		open_file(myfile, web_conf.get_conf_err_page_map()[status_code_nb]);
	}
	return status_nb_message;
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
	if (buffer)
	{
		char *data = strstr(buffer, "/" );
		int i = 0;
		if (data)
		{
			while (data[i] && data[i] != ' ')
				i++;
			if (i != 1)
				file = get_file(data, i);
		}
	}
	return file;
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
**main part of programme, accept and recv info, and then extract info from buffer
**store them to structure Client_Request obj, send response to client
*/
void echange_with_client(int &server_fd, struct sockaddr_in &address, Conf &web_conf)
{
	std::cout << "\n+++++++ Waiting for new connection ++++++++" << std::endl << std::endl;
	Client_Request obj;
	int new_socket;
	int addrlen = sizeof(address);
	int max_nb = web_conf.get_max_size_request();
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	{
		//need to send as a file to browser
		perror("In accept");
		return ;
	}
	long nb_read = recv(new_socket, buffer, sizeof(buffer), 0);
	if (nb_read < 0)
	{
		send_error_page(204, obj, web_conf, new_socket);
		close(new_socket);
		return ;
	}
	std::cout << "[buffer]" << GREEN << buffer << NC << std::endl;
	extract_info_from_first_line_of_buffer(obj, buffer, web_conf);
	extract_info_from_rest_buffer(obj, buffer);

	obj.display_client_request();
	send_response(obj, new_socket);
	close(new_socket);
}
