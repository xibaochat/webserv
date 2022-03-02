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
		ready_map.erase(request_fd);
		request_map.erase(request_fd);
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
void Server::manage_event(struct epoll_event *events, int &epoll_event_count)
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
		{
			ready_map.erase(sockfd);
			request_map.erase(sockfd);
			this->Close(sockfd);
		}
		// A request is now ready to receive a response
		else if (ev & EPOLLIN)
			this->ready_map[sockfd] = this->handle_client_event(sockfd);
		else if (ev & EPOLLOUT && this->ready_map[sockfd])/*send content to request*/
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
	std::cout << YELLOW << "Looking for request" << NC << std::endl;
	while (1)
	{
 		memset(events, 0, EPOLL_SIZE);
		int epoll_event_count = epoll_wait(this->epfd, events, EPOLL_SIZE, 10000000);
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
		this->manage_event(events, epoll_event_count);
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
	obj.clean_relative_path = full_path;
	full_path = r.path_root + file;
	if (full_path.length() - file.length() == 0)
		full_path = "." + full_path;
	obj.set_client_file(full_path);
}

std::string get_curr_server_name(Client_Request &obj)
{
	std::string curr_server_name;
	for (std::map<std::string, std::string>::iterator it=obj.client_request.begin();
		 it!=obj.client_request.end(); ++it)
		if (it->first == "Host")
			curr_server_name = it->second.substr(0, it->second.find(':'));
	return curr_server_name;
}


int get_curr_port(Client_Request &obj)
{
	std::stringstream iss_port;
	int port;

	for (std::map<std::string, std::string>::iterator it=obj.client_request.begin();
		 it!=obj.client_request.end(); ++it)
		if (it->first == "Host")
		{
			iss_port << it->second.substr(it->second.find(':') + 1, it->second.length());;
			iss_port >> port;
			return (port);
		}
	return (-1);
}

int port_is_matching_conf(int curr_port, Conf &curr_conf)
{
	return (curr_conf.port.count(curr_port));
}


/*
** Found the request's matching configuration based on the `Host` header.
** In case the `server_name` & `port` pair do not match any conf, we
** will use the default conf (first one parsed)
**
** :param (std::string) &curr_server_name: requested server name
** :param (int) curr_port: port used by request
** :param (std::vector<Conf>) &web_conf_vector: all parsed server configurations
** :param (Conf) &default_conf: server configuration to use if no matching Conf where found
** :return (Conf): matching Conf or default one
*/
Conf get_curr_conf(std::string &curr_server_name, int curr_port, std::vector<Conf> &web_conf_vector, Conf &default_conf)
{
	std::vector<Conf>::iterator it;
	for (it = web_conf_vector.begin() ;
		 it != web_conf_vector.end(); ++it)
	{
		if (curr_server_name == (*it).server_name)
		{
			if (port_is_matching_conf(curr_port, (*it)))
				return (*it);
			else
				return (default_conf);
		}
	}
	// If request's server_name is not in conf file
	if (it == web_conf_vector.end())
		return default_conf;
	return default_conf;
}

void  Server::extract_info_from_buffer(Client_Request &obj, char *buffer)
{
	extract_info_from_first_line(obj, buffer);
	extract_info_from_rest(obj, buffer);
}

void Server::extract_info_and_prepare_response(Conf &curr_conf, int &fd, Client_Request &obj)
{
	this->request_map[fd] = "";
	route r = get_matching_route(obj, curr_conf);
	reset_file_full_path(r, obj);
	manage_request_status(r, obj, curr_conf);
	this->request_map.erase(fd);
	this->request_map.insert(std::pair<int, std::string> (fd, response_str(obj)));
}

bool Server::chunkManagement(size_t end_of_header)
{
	return true;
}

void manage_default_file_if_needed(Client_Request &obj, Conf &curr_conf)
{
	route r = get_matching_route(obj, curr_conf);
	if (r.auto_index == false && (obj.clean_relative_path == "" || obj.clean_relative_path == "/"))
	{
		obj.file = curr_conf.default_file;
		obj.clean_relative_path = curr_conf.default_file;
	}
}


/*read from the buffer and store the request fd and reponse in the map
 */
bool Server::handle_client_event(int &request_fd)
{
	Client_Request obj;
	int max_nb = 65536;
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	long nb_read = recv(request_fd, buffer, sizeof(buffer), 0);

	Conf default_conf = this->web_conf_vector.at(0);

	std::cout << GREEN << buffer << NC << "\n";
	// Initialize in case it isn't yet
	if (request_map.find(request_fd) == request_map.end())
	{
		this->request_map.insert(std::pair<int, std::string> (request_fd, ""));
		this->ready_map.insert(std::pair<int, bool> (request_fd, false));
	}
	// Add what we just read from the buffer
	request_map[request_fd] += std::string(buffer, nb_read);

	if (nb_read <= 0)
	{
		set_error(obj, default_conf, 204);
		send_response(obj, request_fd);
		ready_map.erase(request_fd);
		request_map.erase(request_fd);
		this->Close(request_fd);
	}
	else
	{
		this->extract_info_from_buffer(obj, buffer);
		std::string curr_server_name = get_curr_server_name(obj);
		int curr_port = get_curr_port(obj);
		Conf curr_conf = get_curr_conf(curr_server_name, curr_port, this->web_conf_vector, default_conf);


		// -------- HTTP REDIRECTION -------
		route r = get_matching_route(obj, curr_conf);
		if (r.redirection.length() > 0)
		{
			std::string final_redir =  r.redirection + obj.origin_path;
			set_error(obj, curr_conf, 301);
			obj.custom_headers["Location"] = final_redir;
			send_response(obj, request_fd);
			ready_map.erase(request_fd);
			request_map.erase(request_fd);
			this->Close(request_fd);
			return (ready_map[request_fd]);
		}
		// ---------------------------------

		manage_default_file_if_needed(obj, curr_conf);

		size_t	end_of_header = request_map[request_fd].find("\r\n\r\n");
		if (end_of_header != std::string::npos)
		{
			if (request_map[request_fd].find("Content-Length: ") == std::string::npos)
			{
				if (request_map[request_fd].find("Transfer-Encoding: chunked") != std::string::npos)
					ready_map[request_fd] = this->chunkManagement(end_of_header);
				else
					ready_map[request_fd] = true;
			}
			else
			{
				std::string tmp = request_map[request_fd].substr(request_map[request_fd].find("Content-Length: ") + 16, 10);
				int len = (int)std::strtol(tmp.c_str(), NULL, 10);
				if (len > curr_conf.get_client_max_body_size() && curr_conf.get_client_max_body_size() != -1)
				{
					set_error(obj, curr_conf, 413);
					send_response(obj, request_fd);
					ready_map.erase(request_fd);
					request_map.erase(request_fd);
					this->Close(request_fd);
					return (ready_map[request_fd]);
				}
				if (request_map[request_fd].size() >= len + end_of_header + 4)
					ready_map[request_fd] = true;
				else
					ready_map[request_fd] = false;
			}
			if (ready_map[request_fd])
				this->extract_info_and_prepare_response(curr_conf, request_fd, obj);
		}
		else
			ready_map[request_fd] = true;
	}
	std::cout << "\t\t" << ready_map[request_fd] << std::endl;
	return (ready_map[request_fd]);
}
