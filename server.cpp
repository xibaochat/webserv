#include "server.hpp"
#include "webserv.hpp"

int get_total_port(std::vector<Conf> &v)
{
	int total_port = 0;
	for (std::vector<Conf>::iterator it = v.begin() ; it != v.end(); ++it)
		total_port += (*it).port.size();
	return total_port;
}


std::set<int> get_all_port_nb_in_set(std::vector<Conf> &v)
{
	std::set<int> port;
	for (std::vector<Conf>::iterator it = v.begin() ; it != v.end(); ++it)
	{
		std::set<int> p = (*it).port;
		for (std::set<int>::iterator it_set = p.begin() ; it_set != p.end(); ++it_set)
			port.insert(*it_set);
	}
	return port;
}

/*Constructor of class Server*/
Server::Server(std::vector<Conf> &web_conf_vector)
{
	//get a set of port from multi server
	this->web_conf_vector = web_conf_vector;
	this->port = get_all_port_nb_in_set(this->web_conf_vector);

	this->serverAddr = new struct sockaddr_in[this->port.size()];
	std::set<int>::iterator it=this->port.begin();
	int i = 0;
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

void Server::Start()
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

route get_matching_route(Client_Request &obj, Conf &web_conf)
{
	std::map<std::string, route> loc_root = web_conf.m_location;
	std::string file = obj.get_client_ask_file();
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

void reset_file_full_path(route &r, Client_Request &obj)
{
	std::string full_path("/");
	std::string file = obj.get_client_ask_file();
	std::vector<std::string> v = extract_words_in_vector(file);
	for (std::vector<std::string>::iterator it = v.begin() ; it != v.end(); ++it)
	{
		full_path += *it;
		if (it != v.end() - 1)
			full_path += "/";
	}
	// if (v.size() > 1 && file[file.size() - 1] == '/')//client ask a dir
	// 	full_path += '/';
	obj.clean_relative_path = full_path;
	full_path = r.path_root + file;
	obj.set_client_file(full_path);
}

/*read from the buffer and store the request fd and reponse in the map
 */
void Server::handle_client_event(int &request_fd)
{
	Conf curr_conf;
	Client_Request obj;
	int max_nb = 65536;
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	long nb_read = recv(request_fd, buffer, sizeof(buffer), 0);
	std::cout << GREEN << buffer << NC << "\n";

	// //Conf curr_conf = this->web_conf;

	if (nb_read <= 0)
	{
		set_error(obj, curr_conf, 204);
		send_response(obj, request_fd);
		this->Close(request_fd);
	}
	else
	{
		extract_info_from_first_line_of_buffer(obj, buffer);
		extract_info_from_rest_buffer(obj, buffer);

		std::string curr_server_name;
		for (std::map<std::string, std::string>::iterator it=obj.client_request.begin();
			 it!=obj.client_request.end(); ++it)
			if (it->first == "Host")
				curr_server_name = it->second.substr(0, it->second.find(':'));

		for (std::vector<Conf>::iterator it = this->web_conf_vector.begin() ;
			 it != this->web_conf_vector.end(); ++it)
			if (curr_server_name == (*it).server_name)
				curr_conf = (*it);
		// If request's server_name is not in conf file
		if (it == this->web_conf_vector.end())
		{
			set_error(obj, curr_conf, 204);
			send_response(obj, request_fd);
			this->Close(request_fd);
		}

		route r = get_matching_route(obj, curr_conf);
		reset_file_full_path(r, obj);
		std::cout << RED << "[file]" << obj.get_client_ask_file() << NC << endl;
		manage_request_status(r, obj, curr_conf);
		this->request_map.insert(std::pair<int, std::string> (request_fd, response_str(obj)));
	}
}
