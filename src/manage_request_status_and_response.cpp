#include "webserv.hpp"
#include <sys/stat.h>
#include <fcntl.h>
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
    obj.set_body_response(content_page);
    myfile.close();
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
	std::string extension;
	std::string f_extension_list[1]={"php"};
	//"html", "css", "png", "bmp", "js", "jpeg", "jpg"};
	std::string file = obj.origin_path;

	int i_last_dot = file.find_last_of(".");
	if (i_last_dot != string::npos && i_last_dot != -1)
		extension = file.substr(i_last_dot + 1);
	else
		extension = "";

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

// return CGI required env variable
char	**get_cgi_env(Client_Request &obj, route &r)
{
	std::vector<string> env;
	char	**_env;
	std::map<std::string, std::string> request;

	request = obj.get_client_request_map();

	std::string	script_filename = (std::string("SCRIPT_FILENAME=") + obj.get_client_ask_file());
	std::string	request_method = (std::string("REQUEST_METHOD=") + obj.get_client_method());
	std::string	upload_dir = (std::string("UPLOAD_DIR=") + UPLOAD_DIR);
	std::string	acceptable_upload = (std::string("ACCEPTABLE_UPLOAD=") + UPLOAD_DEFAUT);

	// env variable for GET and POST
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("REDIRECT_STATUS=200");
	env.push_back(script_filename.c_str());
	env.push_back(request_method.c_str());
	if (!r.path_upload_root.empty())
		upload_dir = (std::string("UPLOAD_DIR=") + r.path_upload_root);
	env.push_back(upload_dir.c_str());
	if (!r.acceptable_upload.empty())
		acceptable_upload = (std::string("ACCEPTABLE_UPLOAD=") + r.acceptable_upload);
	env.push_back(acceptable_upload.c_str());

	if (obj.get_client_method() == "GET") // env variable just for GET
	{
		std::string	query_string = (std::string("QUERY_STRING=") + obj.get_query_string());
		env.push_back(query_string.c_str());
	}
	else if (obj.get_client_method() == "POST") // env variable just for POST
	{
		std::string content_length = (std::string("CONTENT_LENGTH=") + request["Content-Length"]);
		std::string	content_type = (std::string("CONTENT_TYPE=") + request["Content-Type"]);
		env.push_back(content_type.c_str());
		env.push_back(content_length.c_str());
	}

	// convert verctor<string> to char**
	_env = static_cast<char**>(malloc(sizeof(char *) * (env.size() + 1)));
	for (size_t i = 0; i < env.size(); i++)
	{
		_env[i] = strdup(env[i].c_str());
	}
	_env[env.size()] = NULL;
	return (_env);
}

// write payload in stdin
void	write_payload_to_cgi(cl_response &fd_rep)
{
	std::ofstream tmpFile;
	std::string	filename = "/tmp/test";
	// create tmpfile and write data in file
	tmpFile.open(filename.c_str(), std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
	tmpFile.write(fd_rep.unparsed_payloads.c_str(), fd_rep.content_length);
	tmpFile.close();
	// dup fd to STDIN
	int fd = open(filename.c_str(), O_RDONLY);
	if (dup2(fd, STDIN_FILENO) == -1)
		std::cerr << "tmp file dup2 error" << std::endl;
	remove(filename.c_str());
}

// execute cgi file with right args and env
void	execute_cgi_file(int cgi_out[2], char *arr[3], char **env)
{
	close(cgi_out[0]);
	// put cgi print response in cgi_out
	if (dup2(cgi_out[1], STDOUT_FILENO) == -1)
		std::cerr << "cgi_out dup2 error" << std::endl;
	// exec cgi
	if (execve(arr[0], arr, env) == -1)
		std::cerr << "execve error" << std::endl;
}

// get cgi print from cgi_out
int	get_cgi_response(Client_Request &obj, int &cgi_out)
{
	char foo[40960] = {0};
	int nb_read = read(cgi_out, foo, sizeof(foo));
	std::map<std::string, std::string> f_header_map;

	if (nb_read <= 0)
	{
		std::cerr << "read error" << std::endl;
		return (1);
	}
	std::string ret(foo, nb_read);
	close(cgi_out);
	//tract header and its value from output of the file
	f_header_map = extract_header_from_str(ret);
	obj.set_cgi_output_map(f_header_map);
	//after extract, ret is only content without header
	obj.set_total_nb(ret.length());
	obj.set_body_response(ret);
	return (0);
}

int	manage_executable_file(Client_Request &obj, route &r, cl_response &fd_rep)
{
	int status;
	int cgi_out[2];
	char *arr[3];

	if (fd_rep.boundary.length() > 0)
		obj = fd_rep.obj;

	arr[0] = strdup("/usr/bin/python3");
	std::string asked_file = obj.get_client_ask_file();
	arr[1] = strdup(asked_file.c_str());
	arr[2] = NULL;

	if (pipe(cgi_out) == -1)
		std::cerr << "cgi_out error" << std::endl;
	if (obj.get_client_method() == "POST")
		write_payload_to_cgi(fd_rep);

	char **env = get_cgi_env(obj, r);
	pid_t pid = fork();
	if (pid < 0)
		std::cerr << "Fork error!" << std::endl;
	else if (pid == 0)
		execute_cgi_file(cgi_out, arr, env);
	else
	{
		waitpid(pid, &status, 0);
		close(cgi_out[1]);
		for (int i = 0; env[i]; i++)
		{
			free(env[i]);
			env[i] = NULL;
		}
		free(env);
		free(arr[0]);
		free(arr[1]);
		if (!(WIFEXITED(status) && WEXITSTATUS(status)))
		{
			if (get_cgi_response(obj, cgi_out[0]))
				return (1);
		}
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return (1);
	}
	return (0);
}

void    set_error(Client_Request &obj, Conf &web_conf, int status_code_nb)
{
    std::ifstream myfile;
	std::map<int, std::string> error_map = web_conf.get_conf_err_page_map();
	int ret = 0;

	if (status_code_nb == 204)
	{
		std::string emp;
		obj.set_status_code_nb(status_code_nb);
		obj.set_total_nb(0);
		set_request_status_nb_message(status_code_nb, obj);
		obj.set_body_response(emp);
		return ;
	}
	ret = open_file(myfile, error_map[status_code_nb]);
	if (ret)
	{
		obj.set_status_code_nb(status_code_nb);
		set_request_status_nb_message(status_code_nb, obj);
		std::string str = string("Error ") +  obj.get_status_code_message();
		obj.set_total_nb(str.length());
		obj.set_body_response(str);
		myfile.close();
	}
	else
	{
		set_length_and_content(myfile, obj);
		obj.set_status_code_nb(status_code_nb);
		set_request_status_nb_message(status_code_nb, obj);
	}
}

void manage_static_upload(route &r, Client_Request &obj, Conf &curr_conf, cl_response &fd_rep)
{
	if (r.acceptable_upload != "on")
		set_error(obj, curr_conf, 501);
	else
	{
		std::string filepath = r.path_upload_root + fd_rep.filename;
		if (file_no_write_permission(filepath, r.path_upload_root))
			set_error(obj, curr_conf, 403);
		else
		{
			std::ofstream f(filepath.c_str());
			f << fd_rep.payloads;
			f.close();
			set_error(obj, curr_conf, 200);
		}
	}

}

void manage_all_whitelisted_extensions(Client_Request &obj, Conf &web_conf)
{
	std::ifstream myfile;
	std::string extension(obj.get_file_extension());
	std::string header_value;

	open_file(myfile, obj.get_client_ask_file());
	set_length_and_content(myfile, obj);
	if (obj.body_response.length() == 0)
		set_error(obj, web_conf, 204);

	if (file_is_text_based(extension))
	{
		if (file_is_app_based(extension))
			header_value = "application/" + extension;
		else if (extension == "js")
			header_value = "text/javascript";
		else
			header_value = "text/" + extension;
	}
	else
		header_value = "application/octet-stream";
	obj.custom_headers["Content-Type"] = header_value;
}


/*
**check file is valid or not; by default, status_nb_message is "200 OK"in the constructor;
**if file has unaccepted extension->501, if file not exist, ->404; if exist but no open right, ->503; and from map to obtain status error message
*/
void manage_request_status(route &r, Client_Request &obj, Conf &web_conf, cl_response &fd_rep)
{
	std::map<int, std::string> error_map = web_conf.get_conf_err_page_map();

	/*error file, if error html in Conf cannot be open and read, we send a static error
	 message */
	if (method_is_not_allow(r, obj) && !fd_rep.boundary.size())
		set_error(obj, web_conf, 405);
	else if (file_not_exist(obj) && !fd_rep.boundary.size()
		&& obj.method != "DELETE")
	{
		if (obj.dir_list == false)
			set_error(obj, web_conf, 404);
		else
			set_error(obj, web_conf, 403);
	}
	else if (file_no_read_permission(r, obj) && !fd_rep.boundary.size())
	{
		set_error(obj, web_conf, 403);
	}
	else if (manage_cgi_based_file(obj) && !fd_rep.boundary.size())
		set_error(obj, web_conf, 501);
	else if (obj.dir_list == true  && !fd_rep.boundary.size() && r.auto_index == true) //dir listing
	{
		obj.f_extension = "html";
		//origin path is the short path affiche in the html file, full_path is place to open dir
		try
		{
			std::string auto_index_output = get_file_output(obj);
			obj.set_total_nb(auto_index_output.length());
			obj.set_body_response(auto_index_output);
		}
		catch (const char* msg)
		{
			std::cerr << msg << std::endl;
			set_error(obj, web_conf, 404);
		}
	}
	else if (obj.get_client_method() == "DELETE" && !fd_rep.boundary.size())
		delete_request(obj, web_conf);
	else if (fd_rep.file_extension != "py" && fd_rep.boundary.size())
		manage_static_upload(r, obj, web_conf, fd_rep);
	//readable file
	else if (file_extension_is_managed(obj.get_file_extension()))
		manage_all_whitelisted_extensions(obj, web_conf);
	else if (obj.get_file_extension() == "py" || fd_rep.file_extension == "py")
	{
		if (manage_executable_file(obj, r, fd_rep))
			set_error(obj, web_conf, 500);
	}
	else
		set_error(obj, web_conf, 501);
}
