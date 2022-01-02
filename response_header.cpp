#include "webserv.hpp"

//type of file that will send to user
std::string get_file_type(std::string file, Client_Request &obj)
{
	std::string type;
	if (obj.get_status_code() != "200 OK\r\n")
		return (type = "text/html");
	size_t pos = file.rfind (".") + 1;
	type = file.substr(pos);
	if (type == "html" || type == "css" || type == "png" || type == "bmp")
		type = "text/" + type;
	else if (type == "js")
		type = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		type = "image/jpeg";
	else
		type = "text/plain";
	return type;
}

//string of response to client
std::string response_str(Client_Request &obj)
{
	std::map<std::string, std::string> response_header;
	std::string res;
	std::ostringstream convert;

	convert << obj.get_total_nb();
	res = convert.str();// str of nb_read

	std::string response = "HTTP/1.1 ";
	response.append(obj.get_status_code());

	//connection
	response.append("Connection: keep-alive\r\n");

	//content-Type
	response.append("content-Type: ");//text/html\r\n");
	response.append(get_file_type(obj.get_client_ask_file(), obj));
	response.append("\r\n");

	//time
	response.append("Date: ");
	get_time(response);
	response.append("\r\n");

	//server
	response.append("Server: ");
	response.append("nginx/1.18.0 (Ubuntu)");
	response.append("\r\n");

	//Transfer-Encoding
	response.append("Transfer-Encoding: ");
	response.append("identity");
	response.append("\r\n");
//	response.append("Content-Type: text/plain\r\nContent-Length: 12\r\n\nHello world!");

	//content-encoding| no need to manage

	response.append("Content-Length: ");
	std::ostringstream ss;
	ss << obj.get_total_nb();
	response.append(ss.str());
	response.append("\n\n");
	response.append(obj.get_total_line());

	std::cout << RED << response << std::endl << NC;
	return response;
	//return std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\nHello world!");
}
