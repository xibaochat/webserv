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
	send(new_socket , new_str , strlen(new_str), 0);
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
