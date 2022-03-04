#include "webserv.hpp"

int has_new_line(std::string full_request)
{
	int i = 0;
	while (full_request[i])
	{
		if (full_request[i] == '\n')
			return i;
		i++;
	}
	return 0;
}

std::string get_key_from_line(string str, int *i)
{
	int lens = 0;

	while (str[*i] && str[*i] != ':')
	{
		(*i)++;
		lens++;
	}
	std::string key = str.substr(0, lens);
	return (key);
}

int get_r_n_index(std::string full_request)
{
	int i = 0;
	while (full_request[i] && full_request[i] != '\n')
	{
		if (i > 0 && full_request[i] == '\r' && full_request[i + 1] && full_request[i + 1] == '\n')
			return i;
		i++;
	}
	return 0;
}


/*sauf fst line of full_request, extract info from client, create a map to store them, then this map become attribute in obj
**:params(Client_Request) incomplete obj, (char*) user
*/
void extract_info_from_rest(Client_Request &obj, std::string full_request)
{
	int len = 0;
	int i;
	std::string headers_buffer(full_request);
	std::map<std::string,std::string> header;

	std::string line;
	headers_buffer += get_r_n_index(headers_buffer) + 1 + 1;//skip fst line which ask filename
	while ((len = get_r_n_index(headers_buffer)) > 0)
	{
		i = 0;
		std::string str = headers_buffer.substr(0, len);
		std::string key = get_key_from_line(str, &i);
		while (str[i] && (str[i] == ':' || str[i] == ' '))
			i++;
		std::string value = str.substr(i);
		header[key] = value;
		headers_buffer = headers_buffer.substr(len + 2);
	}

	if (header.count("Content-Length") && headers_buffer[0] &&
		headers_buffer[0] == '\r' && headers_buffer[1] && headers_buffer[1] == '\n')
		header["body"] = headers_buffer.substr(2, atoi(header["Content-Length"].c_str()));

	header["method"] = obj.get_client_method();
	header["file"] = obj.get_client_ask_file();
	ostringstream convert;
	convert << obj.get_status_code_nb();
	header["status_code_nb"] = convert.str();
	header["status_code_message"] = obj.get_status_code_message();
	header["extension"] = obj.get_file_extension();
	std::ostringstream ss;
	ss << obj.get_total_nb();
	header["total_nb"] = ss.str();
	header["body_response"] = obj.get_body_response();
	obj.set_client_request_map(header);

	int i_payload = full_request.find("\r\n\r\n") + 4;
	obj.payload = full_request.substr(i_payload);
}
