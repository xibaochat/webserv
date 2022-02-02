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

int manage_extension_err_status(Client_Request &obj)
{
	std::string f_extension_list[8]={"py", "html", "css", "png", "bmp", "js", "jpeg", "jpg"};
	std::string file = obj.get_client_ask_file();
	std::string extension = file.substr(file.find_last_of(".") + 1);
	obj.set_file_extension(extension);
	if (extension_is_not_exist(f_extension_list, extension, 8))
	{
		obj.set_status_code_nb(501);
		set_request_status_nb_message(501, obj);
		return (1);
	}
	return (0);
}

int file_not_exist(Client_Request &obj)
{
	std::string file = obj.get_client_ask_file();
	if (access(file.c_str(), F_OK) != 0)
	{
		obj.set_status_code_nb(404);
		set_request_status_nb_message(404, obj);
		return (1);
	}
	return (0);
}

int file_no_permission(Client_Request &obj)
{
	std::ifstream myfile;
	std::string file = obj.get_client_ask_file();
	myfile.open(file.c_str(), std::ios::in);
	if (!myfile.is_open())
	{
		obj.set_status_code_nb(503);
		set_request_status_nb_message(503, obj);
		return (1);
	}
	myfile.close();
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
		exit(0);
	}
}

/*
**check file is valid or not; by default, status_nb_message is "200 OK"in the constructor;
**if file has unaccepted extension->501, if file not exist, ->404; if exist but no open right, ->503; and from map to obtain status error message
*/
void manage_request_status(Client_Request &obj, Conf &web_conf)
{
	std::ifstream myfile;
	//error file
	if (manage_extension_err_status(obj) || file_not_exist(obj) || file_no_permission(obj))
	{
		int status_code_nb = obj.get_status_code_nb();
		open_file(myfile, web_conf.get_conf_err_page_map()[status_code_nb]);
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
