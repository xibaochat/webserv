#include "webserv.hpp"

/*selon err code, get return page path, and also init obj's file total nb and content
**:param(int) error_code, (Client_Request) a pure obj without initm, (Conf) cofiguration strtureobj
 */
void send_error_page(int error_code, Client_Request &obj, Conf &web_conf)
{
	std::ifstream ss;
	std::map<int, std::string> error_code_message_map;
	std::string status_nb_message ;

	open_file(ss, error_code, web_conf);
	read_the_file(ss, obj);
	error_code_message_map = init_status_code_message_map();
	status_nb_message = error_code_message_map[error_code] + "\r\n";
	obj.set_status_code(status_nb_message);
	response_str(obj);
}

void send_response(Client_Request &obj, int &new_socket)
{
	std::string response = response_str(obj);
	const char *new_str = response.c_str();
	send(new_socket , new_str , strlen(new_str), 0);
}

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
		send_error_page(204, obj, web_conf);
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
