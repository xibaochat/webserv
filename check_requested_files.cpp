#include "webserv.hpp"

int open_file(std::ifstream &myfile, std::string path)
{
    myfile.open(path.c_str(), std::ios::in);
	if (!myfile.is_open())
		return 1;
	return 0;
}

int extension_is_not_exist(std::string *mylist, std::string extension, int size)
{
    std::string *begin = mylist;
    std::string *end = mylist + size;
    if (std::find(begin, end, extension) != end)
        return (0);
    return (1);
}

int file_is_text_based(std::string type)
{
	std::string mylist[]={"html", "css", "png", "bmp", "js", "jpeg", "jpg", "json"};
	if (!extension_is_not_exist(mylist, type, 7))
		return (1);
	return (0);
}

int file_not_exist(Client_Request &obj)
{
	std::string file = obj.get_client_ask_file();
	fstream fileStream;
	fileStream.open(file.c_str());
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
		fileStream.open(file.c_str());
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

//in fact, if file not exist, return 0
int no_permission(struct stat &sb, Client_Request &obj)
{
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
	 return 0;
}

int check_f_permi_existence(route &r, Client_Request &obj)
{
	 struct stat sb;
	 std::string file = obj.get_client_ask_file();
	 stat(file.c_str(), &sb);

	 if (!S_ISREG(sb.st_mode))//directory
	 {
		 if (r.auto_index == false)
		 {
			 obj.set_status_code_nb(403);
			 set_request_status_nb_message(403, obj);
			 return 1;
		 }
		 obj.dir_list = true;
	 }
	 //cannot open the file
	 if (stat(file.c_str(), &sb) == -1)
	 {
		 obj.set_status_code_nb(404);
		 set_request_status_nb_message(404, obj);
		 return 1;
	 }
	 return (no_permission(sb, obj));
}
//https://stackoverflow.com/questions/4908043/what-is-the-best-way-to-check-a-files-existence-and-file-permissions-in-linux-u/4908070
//https://stackoverflow.com/questions/8812959/how-to-read-linux-file-permission-programmatically-in-c-c
//https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
