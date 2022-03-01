#include "webserv.hpp"

void send_response(Client_Request &obj, int &new_socket)
{
	std::string response = response_str(obj);
	const char *new_str = response.c_str();
	std::cout << "Start sending repond for `" << RED << new_socket << NC << "`\n";
	int res = send(new_socket , new_str , strlen(new_str), 0);
	if (res < 0)
		std::cout << RED << ERR_SEND << NC << std::endl;
	std::cout << res << endl;
	std::cout << "End sending repond for `" << RED << new_socket << NC << "`\n";
}

void	delete_request(Client_Request &obj)
{
	int msg;
	std::string file = obj.get_client_ask_file();

	if (remove(file.c_str()) == 0)
		msg = 204;
	else
		msg = 403;
	obj.set_status_code_nb(msg);
	set_request_status_nb_message(msg, obj);
}


std::string get_file(char *data, int i)
{
	std::string file(data, 0, i);
	return file;
}

/*
**  extract client asked file name from buffer
**  if no file, by default, file is "cute_cat.html"
**  return (string)file name
*/
std::string get_client_file(char *buffer, Client_Request &obj)
{
	std::string path;
	std::string file;
	std::string	query_string;

	if (buffer)
	{
		char *data = strstr(buffer, "/" );
		int i = 0;
		if (data)
		{
			while (data[i] && data[i] != ' ')
				i++;
			if (i != 1)//i == 1 means only /
			{
				path = string(data, i);
				std::string::size_type pos = path.find("?");
				if (pos != std::string::npos)
				{
					file = get_file(data, pos);
					query_string = string(data, pos + 1, i - pos);
					obj.set_query_string(query_string);
				}
				else
					file = get_file(data, i);
			}
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
void extract_info_from_first_line_of_buffer(Client_Request &obj, char *buffer)
{
	char *ptr = strstr(buffer, " ");//GET , POST ?
	std::string method(buffer, 0, ptr - buffer);
	obj.set_client_method(method);
	std::string file = get_client_file(buffer, obj);//the file client ask
	obj.origin_path = file;
	std::cout << BLUE << "[ORIGIN]" << file << NC << endl;
	obj.set_client_file(file);
}
