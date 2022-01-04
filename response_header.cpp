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


/*according to info (Client_Request &) obj,
**create string as response to client
**return (std::string) response
 */
std::string response_str(Client_Request &obj)
{
	std::map<std::string, std::string> response_header;
	std::string res("HTTP/1.1 ");
	res += obj.get_status_code() + "\r\n";
	response_header["Connection: "] = "keep-alive";
	response_header["content-Type: "] = get_file_type(obj.get_client_ask_file(), obj);
	response_header["Date: "] = get_time();
	response_header["Server: "] = "nginx/1.18.0 (Ubuntu)";
	response_header["Transfer-Encoding: "] = "identity";
	std::ostringstream ss;
	ss << obj.get_total_nb();
	response_header["Content-Length: "] = ss.str();
	for (std::map<std::string, std::string>::iterator it=response_header.begin(); it!=response_header.end(); ++it)
	{
		res.append(it->first);
		res.append(it->second);
		res.append("\r\n");
	}
	res.append("Content-Length: ");
	res.append(ss.str());
	res.append("\n\n");
	res.append(obj.get_total_line());
	return res;

//	return std::string("HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\nContent-Type: text/plain\r\nHello world!");
}
