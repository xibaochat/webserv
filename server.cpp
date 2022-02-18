#include "server.hpp"
#include "webserv.hpp"

/*Constructor of class Server*/
Server::Server(Conf &web_conf)
{
	this->port = web_conf.get_port();
	this->serverAddr = new struct sockaddr_in[this->port.size()];

	int i = 0;
	std::set<int>::iterator it=this->port.begin();
	while(it!=this->port.end())
	{
		memset(this->serverAddr[i].sin_zero, '\0', sizeof this->serverAddr[i].sin_zero);
		this->serverAddr[i].sin_family = AF_INET;
		this->serverAddr[i].sin_addr.s_addr = INADDR_ANY;
		this->serverAddr[i].sin_port = htons(*it);
		it++;
		i++;
	}
	this->listener = new int[this->port.size()];
	this->epfd = 0;
	this->error_page_map = web_conf.get_conf_err_page_map();
	this->web_conf = web_conf;
}

/* create sockfd fd of endpoint, bind and listen, add sockfd to interest list of epoll*/
void Server::Init()
{
	int opt = 1;
	int size = this->port.size();
	int i = 0;

	/*the size argument is ignored, but must be greater than zero*/
	this->epfd = epoll_create(5000);
	if (this->epfd < 0)
		throw("[ERROR]epoll create error");
	while (i < size)
	{
		this->listener[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (this->listener[i] < 0)
			throw("[ERROR]Failed to create socket fd");
	/*even stop the program and restart it, the port is still available.*/
		if (setsockopt(this->listener[i], SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(int)) < 0)
			throw("setsockopt(SO_REUSEADDR) failed");
		if (bind(this->listener[i], (struct sockaddr *)&this->serverAddr[i], sizeof(this->serverAddr[i])) < 0)
			throw("[ERROR]Failed to bind");
		if (listen(this->listener[i], 5000) < 0) //maximum length to which the  queue  of pending  connections  for sockfd may grow
			throw("[ERROR]Listen error");
		/*add this->listener to interest list of epoll*/
		this->addfd(this->listener[i], 0);
		i++;
	}
}

void Server::Close(int &sockfd)
{
	epoll_ctl(this->epfd, EPOLL_CTL_DEL, sockfd, NULL);
	close(sockfd);
	this->request_map.erase(sockfd);
}

Server::~Server()
{
	int i = this->port.size();
	while (--i >= 0)
		close(this->listener[i]);
	close(this->epfd);
	delete [] this->serverAddr;
	delete [] this->listener;
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

int Server::fd_is_in_listener(int fd)
{
	int i = this->port.size();
	while (--i >= 0)
	{
		if (fd == this->listener[i])
			return 1;
	}
	return 0;
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
		//if (sockfd == this->listener)
		if (fd_is_in_listener(sockfd))
		{
			try
			{
				this->acceptConnect(sockfd);
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
	try
	{
		this->Init();
	}
	catch(const char *s)
	{
		delete [] this->serverAddr;
		delete [] this->listener;
		std::cerr << s << std::endl;
		exit(EXIT_FAILURE);
	}
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
void Server::acceptConnect(int &fd)
{
	struct sockaddr_in client_address;
	int addrlen = sizeof(struct sockaddr_in);
	int request_fd = accept(fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
	if (request_fd < 0)
		throw("[ERROR]accpet failure");
	this->addfd(request_fd, true);
}

int check_substring(std::string str1, std::string str2)
{
    int i = 0;
    int c = 0; // counter for substring
	while (i < str1.length())
    {
		if (str1[i] == '/')
		{
			i++;
			continue;
		}
		if (str2[c] == '/')
		{
			c++;
			continue;
		}
        if( c == str2.length() )
            return 1;
        if(str2[c] == str1[i])
            c++;
		else
			return 0;
		i++;
    }
	if (c == str1.length())
		return 1;
    return 0;
    //checking if the substring is present or not
}

route get_most_match_route(std::string file, std::map<std::string, route> loc_root)
{
	int loc_len = 0;
	std::string key;
	for (std::map<std::string, route>::iterator it=loc_root.begin(); it!=loc_root.end(); ++it)
    {
		if (it->first != "/" && check_substring(file, it->first))
        {
			if (it->first.length() > loc_len)
			{
				loc_len = it->first.length();
				key = it->first;
			}
        }
    }
	if(!key.size())
		key = "/";
	return loc_root[key];
}

route find_match_route(Client_Request &obj, Conf &web_conf)
{
	std::map<std::string, route> loc_root = web_conf.m_location;
	std::string file = obj.get_client_ask_file();
	route r = get_most_match_route(file, loc_root);
	return r;
}

void reset_file_full_path(route &r, Client_Request &obj)
{
	std::string file = obj.get_client_ask_file();
	std::string full_path = r.path_root + file;
	obj.set_client_file(full_path);
}

/*read from the buffer and store the request fd and reponse in the map
 */
void Server::handle_client_event(int &request_fd)
{
	Client_Request obj;
	int max_nb = 3000000;
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
		route r = find_match_route(obj, this->web_conf);
		reset_file_full_path(r, obj);
		std::cout << RED << "[file]" << obj.get_client_ask_file() << NC << endl;
		manage_request_status(r, obj, this->web_conf);
		this->request_map.insert(std::pair<int, std::string> (request_fd, response_str(obj)));
	}
}
