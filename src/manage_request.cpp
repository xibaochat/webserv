#include "webserv.hpp"

void	delete_request(Client_Request &obj, Conf &curr_conf)
{
	int msg;
	std::string file = obj.get_client_ask_file();

	if (remove(file.c_str()) == 0)
		msg = 200;
	else
		msg = 403;
	set_error(obj, curr_conf, msg);
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
void extract_info_from_first_line(Client_Request &obj, std::string full_request)
{
	size_t ptr = full_request.find(" ");
	std::string method = full_request.substr(0, ptr);
	obj.set_client_method(method);
	std::string file = get_client_file(&full_request[0], obj);//the file client ask
	obj.origin_path = file;
	obj.set_client_file(file);
}
