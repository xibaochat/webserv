#include "server.hpp"
#include "webserv.hpp"

/*Constructor of class Server*/
Server::Server(Conf &web_conf)
{
	this->port = web_conf.get_port();
	memset(this->serverAddr.sin_zero, '\0', sizeof this->serverAddr.sin_zero);
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;
	this->serverAddr.sin_port = htons(this->port);
	this->listener = 0;
	this->epfd = 0;
	this->error_page_map = web_conf.get_conf_err_page_map();
	this->web_conf = web_conf;
}

/* create sockfd fd of endpoint, bind and listen, add sockfd to interest list of epoll*/
void Server::Init()
{
	int opt = 1;
	this->listener = socket(AF_INET, SOCK_STREAM, 0);
	if (this->listener < 0)
		throw("[ERROR]Failed to create socket fd");
	/*even stop the program and restart it, the port is still available.*/
	if (setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(int)) < 0)
		throw("setsockopt(SO_REUSEADDR) failed");
	if (this->listener < 0)
		throw("[ERROR]Failed to create socket");
	if (bind(this->listener, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
		throw("[ERROR]Failed to bind");
	if (listen(this->listener, 5000) < 0) //maximum length to which the  queue  of pending  connections  for sockfd may grow
		throw("[ERROR]Listen error");
	/*the size argument is ignored, but must be greater than zero*/
	this->epfd = epoll_create(5000);
	if (this->epfd < 0)
		throw("[ERROR]epoll create error");
	/*add this->listener to interest list of epoll*/
	this->addfd(this->listener, 0);
}

void Server::Close(int &sockfd)
{
	epoll_ctl(this->epfd, EPOLL_CTL_DEL, sockfd, NULL);
	close(sockfd);
	this->request_map.erase(sockfd);
}

Server::~Server()
{
	close(this->listener);
	close(this->epfd);
}

void Server::send_content_to_request(int &request_fd)
{
	std::map<int, std::string>::iterator it;
	it = this->request_map.find(request_fd);
	if (it != request_map.end())
	{
		const char *new_str = (*it).second.c_str();
		if (send((*it).first, new_str, strlen(new_str), 0) < 0)
			std::cout << RED << ERR_SEND << NC << std::endl;
		this->Close(request_fd);
	}

}
/*loop for each event, manage the cas: new request(add request fd to epoll interest list);
  error or interrupt(close fd); read the request(read from buffer and store reponse in map);
  send reponse to request(send the reponse and close fd, erase fd from map)
 *:paramas (struct epoll_event *) events: event array that store all the info of each events
 *:params (int)epoll_event_count:total nb of events
 *:params (std::map<int, std::string>) request_map: a map store fd from read process and corresponding reponse
 */
void Server::manage_event(struct epoll_event *events, int &epoll_event_count, std::map<int, std::string> &request_map)
{
	for(int i = 0; i < epoll_event_count; i++)
	{
		int sockfd = events[i].data.fd;
		uint32_t ev = events[i].events;
		/* We receive a new request on the socket
		   We accept it and store new the FD of the request in the epoll interet list*/
		if (sockfd == this->listener)
		{
			try
			{
				this->acceptConnect();
			}
			catch(const char *s)
			{
				std::cerr << s << std::endl;
			}
		}
		/* sth went wrong in the epoll monitoring list*/
		else if (ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			this->Close(sockfd);
		// A request is now ready to receive a response
		else if (ev & EPOLLIN)
			this->handle_client_event(sockfd);
		else if (ev && EPOLLOUT)/*send content to request*/
			this->send_content_to_request(sockfd);
	}
}

void Server::Start(Conf &web_conf)
{
	struct epoll_event events[EPOLL_SIZE];
	this->Init();
	while (1)
	{
		std::cout << YELLOW << "Looking for request" << NC << std::endl;
		memset(events, 0, EPOLL_SIZE);
		int epoll_event_count = epoll_wait(this->epfd, events, EPOLL_SIZE, 1000);
		/*err manage*/
		if (epoll_event_count < 0)
		{
			std::cout << RED << "epoll_wait error occurs" << NC << std::endl;
			continue ;
		}
		if (epoll_event_count == 0)
		{
			std::cout << GREEN << "NO REQUEST\n" << NC;
			continue;
		}
		/* `epoll_event_count` will most likely always be equal to 1, since `epoll_wait`
		   will return immediatly after receiving an event */
		this->manage_event(events, epoll_event_count, this->request_map);
	}
}

void Server::addfd(int fd, bool enable_et)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(struct epoll_event));
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (enable_et)
		//to  avoid  continuously  switching between EPOLLIN and EPOLLOUT calling
		ev.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
			throw("[ERROR]Failed to in epoll_ctl");
	fcntl(fd, F_SETFL, O_NONBLOCK);
	std::cout << "fd added to epoll" << std::endl;
}

/*create a new fd and add to the interest list
 */
void Server::acceptConnect(int fd)
{
	struct sockaddr_in client_address;
	int addrlen = sizeof(struct sockaddr_in);
//	int request_fd = accept(this->listener, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
	int request_fd = accept(fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
	if (request_fd < 0)
		throw("[ERROR]accpet failure");
	this->addfd(request_fd, true);
}

int check_substring(std::string str1, std::string str2)
{
    int i;
    int c = 0; // counter for substring
    for( i=0; i < str1.length();i++)
    {
        if( c == str2.length() )
            return 1;
        if(str2[c] == str1[i])
            c++;
    }
	if (c == str1.length())
		return 1;
    return 0;
    //checking if the substring is present or not
}

void reset_file_full_path(Client_Request &obj, Conf &web_conf)
{
	std::map<std::string, std::string> loc_root = web_conf.get_root();
	std::string file = obj.get_client_ask_file();
    for (std::map<std::string, std::string>::iterator it=loc_root.begin(); it!=loc_root.end(); ++it)
    {
		cout << RED << file << " " << it->first << " " << check_substring(file, it->first) << "\n";
        if (it->first != "/" && check_substring(file, it->first))
        {
            file = it->second + file;
			obj.set_client_file(file);
			return ;
        }
    }
	file = loc_root["/"] + obj.get_client_ask_file();
	obj.set_client_file(file);
	return ;
}

/*read from the buffer and store the request fd and reponse in the map
 */
void Server::handle_client_event(int &request_fd)
{
	Client_Request obj;
	int max_nb = this->web_conf.get_max_size_request();
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	long nb_read = recv(request_fd, buffer, sizeof(buffer), 0);
	std::cout << GREEN << buffer << NC << "\n";
	if (nb_read <= 0)
	{
        send_error_page(204, obj, this->web_conf, request_fd);
		this->Close(request_fd);
	}
	else
	{
		extract_info_from_first_line_of_buffer(obj, buffer, this->web_conf);
		extract_info_from_rest_buffer(obj, buffer);
		reset_file_full_path(obj, this->web_conf);
		std::cout << RED << "[file]" << obj.get_client_ask_file() << NC << endl;
		manage_request_status(obj, this->web_conf);
		this->request_map.insert(std::pair<int, std::string> (request_fd, response_str(obj)));
	}
}
