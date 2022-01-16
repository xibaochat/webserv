#include "server.hpp"
#include "webserv.hpp"

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

void Server::Init()
{
	int opt = 1;
	this->listener = socket(AF_INET, SOCK_STREAM, 0);
	//even stop the program and restart it, the port is still available.
	if (setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(int)) < 0)
		throw("setsockopt(SO_REUSEADDR) failed");
	if (this->listener < 0)
		throw("[ERROR]Failed to create socket");
	if (bind(this->listener, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
		throw("[ERROR]Failed to bind");
	if (listen(this->listener, 5000) < 0) //maximum length to which the  queue  of pending  connections  for sockfd may grow
		throw("[ERROR]Listen error");
	/*the size argument is ignored, but must be greater than zero*/
	this->epfd = epoll_create(5000); //size用来告诉内核这个监听的数目一共有多大
	if (this->epfd < 0)
		throw("[ERROR]epoll create error");
	this->addfd(this->listener, 1);
}

//https://www.researchgate.net/post/What_is_the_role_of_static_function_and_this_pointer_in_C

void Server::Close()
{
	close(this->listener);
	close(this->epfd);
}

void Server::Start(Conf &web_conf)
{
	struct epoll_event events[EPOLL_SIZE];
	try
	{
		this->Init();
	}
	catch(const char* exception)
	{
		std::cerr << exception << std::endl;
		exit(0);
	}
	while (1)
	{
		std::cout << YELLOW << "Looking for request\n" << NC;
		int epoll_event_count = epoll_wait(this->epfd, events, EPOLL_SIZE, 100);
		if (epoll_event_count < 0)
		{
			std::cerr << "[ERROR]epoll failure" << std::endl;
			exit(0);
		}
		if (epoll_event_count == 0)
		{
			std::cout << GREEN << "NO REQUEST\n" << NC;
			continue;
		}
		else
			// `epoll_event_count` will most likely always be equal to 1, since `epoll_wait`
			// will return immediatly after receiving an event
			std::cout << RED << "nb request " << epoll_event_count << "\n" << NC;

		for(int i = 0; i < epoll_event_count; i++)
		{
			int sockfd = events[i].data.fd;
			uint32_t ev = events[i].events;

            // We receive a new request on the socket
			// We accept it and store new the FD of the request in the epoll list
			if (sockfd == this->listener)
				acceptConnect();
			// Something went wrong with an already accepted request stored in the epoll list
			else if (ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))/*err happened*/
				epoll_ctl(this->epfd, EPOLL_CTL_DEL, sockfd, NULL);
			// A request stored in the epoll list is now ready to receive a response
			else if (ev & EPOLLIN)
			{
				std::cout << sockfd << "Starting to repond: `" << MAGENTA << sockfd << NC << "`\n" << endl;
				handle_client_event(sockfd, web_conf);
			}
			else if (ev && EPOLLOUT)
			{
				struct epoll_event evv;
				std::cout << RED << sockfd << "WRITING\n";
				std::cout << this->get_request_map()[0] << "\n";
//				send_response(obj, clientfd);
				const char *new_str = this->request_map[0].c_str();
				std::cout << " new str len " << strlen(new_str) << "\n";
				send(sockfd, new_str , strlen(new_str), 0);
				close(sockfd);
				epoll_ctl(this->epfd, EPOLL_CTL_DEL, sockfd, NULL);
//				std::map<int, std::string>::iterator it = this->request_map.find(sockfd);
//				this->request_map.erase(it);
				exit(0);
			}
			std::cout << YELLOW << "INSIDE BELOW\n" << NC;
		}
	}
	this->Close();
}

void Server::acceptConnect()
{
	struct sockaddr_in client_address;
	int addrlen = sizeof(struct sockaddr_in);
	int clientfd = accept(this->listener, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
	if (clientfd < 0)
	{
		std::cerr << "[ERROR]accpet failure" << std::endl;
		return ;
	}
	try
	{
		std::cout << "Adding new fd in list: `" << BLUE << clientfd << NC << "`\n" << endl;
		this->addfd(clientfd, true);
	}
	catch(const char* e)
	{
		std::cerr << e << std::endl;
	}
}

void Server::handle_client_event(int &clientfd, Conf &web_conf)
{
	struct epoll_event ev;
	Client_Request obj;
	memset(&ev, 0, sizeof(struct epoll_event));
	int max_nb = web_conf.get_max_size_request();
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	long nb_read = recv(clientfd, buffer, sizeof(buffer), 0);
	std::cout << "[buffer]" << GREEN << buffer << NC << std::endl;
	std::cout << "STARTING HERE `" << clientfd << "`: \n" << buffer << "\n";
	if (nb_read < 0)
        send_error_page(204, obj, web_conf, clientfd);
	else
	{
		extract_info_from_first_line_of_buffer(obj, buffer, web_conf);
		extract_info_from_rest_buffer(obj, buffer);
		this->request_map[0] = response_str(obj);
		// 	send_response(obj, clientfd);
		// 	close(clientfd);
		// 	epoll_ctl(server.get_epfd(), EPOLL_CTL_DEL, clientfd, NULL);
		 ev.data.fd=clientfd;
		 //设置用于注测的写操作事件
		 ev.events=EPOLLOUT | EPOLLET;
		 epoll_ctl(this->epfd,EPOLL_CTL_MOD, clientfd, &ev);
		 //修改sockfd上要处理的事件为EPOLLOUT
	}
}
