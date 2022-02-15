#include "webserv.hpp"

void store_header_key_value_in_map(std::string &header_line, std::map<std::string, std::string> &header_map)
{
	size_t i = header_line.find(":");
	std::string header = header_line.substr(0, i);
	header_map[header] = header_line.erase(0, i + 1);
}

std::map<std::string, std::string> extract_header_from_str(std::string &str)
{
	std::map<std::string, std::string> header_map;
    std::string delimiter = "\r\n";

    size_t pos = 0;
    std::string header_line;
    while ((pos = str.find(delimiter)) != std::string::npos)
	{
		if (!pos)
		{
			str.erase(0, pos + delimiter.length());
			continue ;
		}
		header_line = str.substr(0, pos);
		//extract header and value then store them in a map
		store_header_key_value_in_map(header_line, header_map);
        str.erase(0, pos + delimiter.length());
    }
	return header_map;
}

int open_file(std::ifstream &myfile, std::string path)
{
    myfile.open(path.c_str(), std::ios::in);
	if (!myfile.is_open())
		return 1;
	return 0;
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

int extension_is_not_exist(std::string *mylist, std::string extension, int size)
{
    std::string *begin = mylist;
    std::string *end = mylist + size;
    if (std::find(begin, end, extension) != end)
        return (0);
    return (1);
}

void set_request_status_nb_message(int status_nb, Client_Request &obj)
{
	std::map<int, std::string> status_code_message_map = init_status_code_message_map();
	std::string status_nb_message;
	status_nb_message = status_code_message_map[status_nb];
	obj.set_status_code_message(status_nb_message);
}

//if it is not python, ex:php && file exist, status code is 501
int manage_cgi_based_file(Client_Request &obj)
{
	std::string f_extension_list[1]={"php"};
	//"html", "css", "png", "bmp", "js", "jpeg", "jpg"};
	std::string file = obj.get_client_ask_file();
	std::string extension = file.substr(file.find_last_of(".") + 1);
	obj.set_file_extension(extension);
	if (!extension_is_not_exist(f_extension_list, extension, 1))
	{
		std::ifstream fs;
		fs.open(file.c_str());
		if (fs.is_open())
		{
			obj.set_status_code_nb(501);
			set_request_status_nb_message(501, obj);
			fs.close();
		}
		return (1);
	}
	return (0);
}

int file_not_exist(Client_Request &obj)
{
	std::string file = obj.get_client_ask_file();
	fstream fileStream;
	fileStream.open(file);
	if (fileStream.fail())
	{
		obj.set_status_code_nb(404);
		return 1;
	}
    // file could not be open
	fileStream.close();
	return (0);
}

int file_no_permission(Client_Request &obj)
{
	std::string file = obj.get_client_ask_file();
	if (access(file.c_str(), W_OK) != 0)
	{
		fstream fileStream;
		fileStream.open(file);
		if (fileStream.fail())
		{
			obj.set_status_code_nb(404);
			set_request_status_nb_message(404, obj);
			fileStream.close();
		}
		else
		{
			obj.set_status_code_nb(403);
			set_request_status_nb_message(403, obj);
		}
		return (1);
	}
	return (0);
}

int file_is_text_based(std::string type)
{
	std::string mylist[]={"html", "css", "png", "bmp", "js", "jpeg", "jpg", "json"};
	if (!extension_is_not_exist(mylist, type, 7))
		return (1);
	return (0);
}

void manage_executable_file(Client_Request &obj)
{
	int status;
	int link[2];
	char *arr[3];
	std::map<std::string, std::string> f_header_map;
	arr[0] = strdup("/usr/bin/python3");
	arr[1] = strdup(obj.get_client_ask_file().c_str());
	arr[2] = NULL;
	char *env[] = {NULL};
	pipe(link);
	char foo[4096] = {0};
	pid_t pid = fork();
	if (pid == 0)
	{
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		execve(arr[0], arr, env);
	}
	else
	{
		waitpid(pid, &status, 0);
		int nb_read = read(link[0], foo, sizeof(foo));
		std::string ret(foo, nb_read);
		close(link[0]);
		std::cout << ret << "\n";
		free(arr[0]);
		free(arr[1]);
		//tract header and its value from output of the file
		f_header_map = extract_header_from_str(ret);
		obj.set_cgi_output_map(f_header_map);
		//after extract, ret is only content without header
		obj.set_total_nb(ret.length());
		obj.set_total_line(ret);
	}
}

int method_is_not_allow(route &r, Client_Request &obj)
{
	std::set<string>::iterator it;
	std::string method = obj.get_client_method();
	for (it = r.allow_methods.begin(); it != r.allow_methods.end(); ++it)
	{
		if (*it == method)
			return 0;
	}
	obj.set_status_code_nb(405);
	set_request_status_nb_message(405, obj);
	return 1;
}

int check_f_permi_existence(Client_Request &obj)
{
	 struct stat sb;
	 std::string file = obj.get_client_ask_file();

	 stat(file.c_str(), &sb);
	 if (!S_ISREG(sb.st_mode))//directory
	 {
		 obj.set_status_code_nb(404);
		 set_request_status_nb_message(404, obj);
		 return 1;
	 }
	 struct passwd *pw = getpwuid(sb.st_uid);
	 struct group  *gr = getgrgid(sb.st_gid);
	//no permission
	 if (!(pw && (sb.st_mode & S_IRUSR))
		 || !(gr && (sb.st_mode & S_IRGRP))
		 || !(sb.st_mode & S_IROTH))
	 {
		 obj.set_status_code_nb(403);
		 set_request_status_nb_message(403, obj);
		 return 1;
	 }
	 if (stat(file.c_str(), &sb) == -1)
	 {
		 cout << GREEN << "enter" << NC << "\n";
		 obj.set_status_code_nb(404);
		 set_request_status_nb_message(404, obj);
		 return 1;
	 }
	 return 0;
}
//https://stackoverflow.com/questions/4908043/what-is-the-best-way-to-check-a-files-existence-and-file-permissions-in-linux-u/4908070
//https://stackoverflow.com/questions/8812959/how-to-read-linux-file-permission-programmatically-in-c-c
//https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file

/*
**check file is valid or not; by default, status_nb_message is "200 OK"in the constructor;
**if file has unaccepted extension->501, if file not exist, ->404; if exist but no open right, ->503; and from map to obtain status error message
*/
void manage_request_status(route &r, Client_Request &obj, Conf &web_conf)
{
	std::ifstream myfile;
	int ret = 0;

	/*error file, if error html in Conf cannot be open and read, we send a static error
	 message */
	if (method_is_not_allow(r, obj) || check_f_permi_existence(obj) || manage_cgi_based_file(obj))
	{
		int status_code_nb = obj.get_status_code_nb();
		ret = open_file(myfile, web_conf.get_conf_err_page_map()[status_code_nb]);
		if (ret)
		{
			std::string str("Error 404 : Not Found");
			obj.set_status_code_nb(404);
			set_request_status_nb_message(404, obj);
			obj.set_total_nb(str.length());
			obj.set_total_line(str);
			myfile.close();
		}
		else
			set_length_and_content(myfile, obj);
	}
	//readable file
	else if (file_is_text_based(obj.get_file_extension()))
	{
		open_file(myfile, obj.get_client_ask_file());
		set_length_and_content(myfile, obj);
	}
	else if (obj.get_file_extension() == "py")
	{
		manage_executable_file(obj);
	}
}
