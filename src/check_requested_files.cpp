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

int file_extension_is_managed(std::string type)
{
	std::string mylist[] = MANAGED_FILE_EXTENSIONS;
	if (type.length() == 0)
		return (0);
	if (!extension_is_not_exist(mylist, type, sizeof(mylist) / sizeof(std::string)))
		return (1);
	return (0);
}

int file_is_text_based(std::string type)
{
	std::string mylist[] = TEXT_BASED_EXTENSIONS;
	if (!extension_is_not_exist(mylist, type, sizeof(mylist) / sizeof(std::string)))
		return (1);
	return (0);
}

int file_is_app_based(std::string type)
{
	std::string mylist[] = APP_BASED_EXTENSIONS;
	if (!extension_is_not_exist(mylist, type, sizeof(mylist) / sizeof(std::string)))
		return (1);
	return (0);
}

int file_not_exist(Client_Request &obj)
{
	struct stat sb;
	std::string file = obj.get_client_ask_file();
	stat(file.c_str(), &sb);
	if (stat(file.c_str(), &sb) == -1)
		return 1;
    // file could not be open
	return (0);
}

int file_no_read_permission(route &r, Client_Request &obj)
{
	struct stat sb;
	std::string file = obj.get_client_ask_file();

	//check in case it doesn't exist
	if (stat(file.c_str(), &sb) == -1)
		return (1);

	if (!S_ISREG(sb.st_mode))//directory
	{
		//it is a dir, auto is off
		if (r.auto_index == false)
			return 1;
		obj.dir_list = true;
	}
	//check in case it doesn't exist
	if (stat(file.c_str(), &sb) == -1)
		return 1;

	unsigned int server_uid = getuid();
	unsigned int server_gid = getgid();

	if ((server_uid == sb.st_uid && (sb.st_mode & S_IRUSR)) ||
		(server_gid == sb.st_gid && (sb.st_mode & S_IRGRP)) ||
		(sb.st_mode & S_IROTH))
		return (0);
	return (1);
}

int file_no_write_permission(std::string filepath, std::string path)
{
	struct stat sb;
	stat(filepath.c_str(), &sb);

	if (stat(filepath.c_str(), &sb) == -1)
	{
		if (path.length() > 0)
			return file_no_write_permission(path, "");
		return (1);

	}

	unsigned int server_uid = getuid();
	unsigned int server_gid = getgid();

	if ((server_uid == sb.st_uid && (sb.st_mode & S_IWUSR)) ||
		(server_gid == sb.st_gid && (sb.st_mode & S_IWGRP)) ||
		(sb.st_mode & S_IWOTH))
		return (0);
	return (1);
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
	return 1;
}

//https://stackoverflow.com/questions/4908043/what-is-the-best-way-to-check-a-files-existence-and-file-permissions-in-linux-u/4908070
//https://stackoverflow.com/questions/8812959/how-to-read-linux-file-permission-programmatically-in-c-c
//https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
