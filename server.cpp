#include "webserv.hpp"

void init_socket(int &server_fd, struct sockaddr_in &address)
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    memset(address.sin_zero, '\0', sizeof address.sin_zero);
}

void bind_and_listen(int &server_fd, struct sockaddr_in &address)
{
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

std::string get_client_file(char *buffer)
{
	char *data = strstr(buffer, "/" );
	int i = 0;
	while (data[i] && data[i] != ' ')
		i++;
	std::string file(data, 1, i - 1);
	if (file.length() == 0)
		file = "cute_cat.html";
	std::cout << RED << "FILE is :" << file << NC << "\n";
	return file;
}

std::string get_status_code_file(std::ifstream &myfile, std::string file)
{
	std::string status_code;

	myfile.open(file.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		status_code = "200 OK\r\n";
		std::cout << "\nOK\n";
	}
	else
	{
		status_code = "404 Not Found\r\n";
		myfile.open("404.html", std::ios::in);
		std::cout << "Wrong in file\n";
	}
	return status_code;
}

void read_the_file(std::ifstream &myfile, Client_Request &obj)
{
	std::string line;

	if (getline(myfile, line))
	{
		obj.set_total_nb(line.length());
		obj.set_total_line(line);
	}
	myfile.close();
}

void extract_info_from_buffer(Client_Request &obj, char *buffer)
{
	std::ifstream myfile;
	char *ptr = strstr(buffer, " ");
	std::string method(buffer, 0, ptr - buffer);
	obj.set_client_method(method);
	std::string file = get_client_file(buffer);//the file client ask
	obj.set_client_file(file);
	std::string status_code = get_status_code_file(myfile, file);//file valid?
	obj.set_status_code(status_code);
	read_the_file(myfile, obj);//even if not valid, we send 404.html
}

int main(int ac, char **av)
{
	//###parse nginx conf
	check_config_file(ac, av);
	// int server_fd;
	// int new_socket;
    // struct sockaddr_in address;
    // int addrlen = sizeof(address);

	// init_socket(server_fd, address);

	// bind_and_listen(server_fd, address);
    // while(1)
    // {
	// 	std::cout << "\n+++++++ Waiting for new connection ++++++++" << std::endl << std::endl;
    //     if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    //     {
    //         perror("In accept");
    //         exit(EXIT_FAILURE);
    //     }
	// 	Client_Request obj;
    //     char buffer[3000] = {0};
	// 	long nb_read = recv(new_socket, buffer, sizeof(buffer), 0);
	// 	if (nb_read < 0)
	// 	{
	// 		std::cout << "No byte are there to read" << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	else
	// 	{
	// 		std::cout << "[buffer]" << GREEN << buffer << NC << std::endl;
	// 		extract_info_from_buffer(obj, buffer);
	// 	}
	// 	std::map<std::string, std::string> client_req = extract_info_from_header(obj, buffer);
	// 	std::string response = response_str(obj);
	// 	const char *new_str = response.c_str() ;
	// 	send(new_socket , new_str , strlen(new_str), 0);
	// 	std::cout << "------------------Response message sent-------------------" << std::endl;
    //     close(new_socket);
    // }
    return 0;
}
