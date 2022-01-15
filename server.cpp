#include "server.hpp"
#include "webserv.hpp"

Server::Server(Conf &web_conf)
{
	this->port = web_conf.get_port();
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;
	this->serverAddr.sin_port = htons(this->port);
	memset(this->serverAddr.sin_zero, '\0', sizeof this->serverAddr.sin_zero);
	this->listener = 0;
	this->epfd = 0;
	this->error_page_map = web_conf.get_conf_err_page_map();
}

void Server::Init()
{
	this->listener = socket(AF_INET, SOCK_STREAM, 0);

	if (this->listener < 0)
		throw("[ERROR]Failed to create socket");
	if (bind(this->listener, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
		throw("[ERROR]Failed to bind");
	if (listen(this->listener, 1000) < 0)
		throw("[ERROR]Listen error");
	/*the size argument is ignored, but must be greater than zero*/
	this->epfd = epoll_create(5000);
	if (this->epfd < 0)
		throw("[ERROR]epoll create error");
	this->addfd(this->epfd, this->listener, 1);
}

//https://www.researchgate.net/post/What_is_the_role_of_static_function_and_this_pointer_in_C


void Server::Close()
{
	close(this->listener);
	close(this->epfd);
}

void Server::Start(Conf &web_conf)
{
	struct epoll_event events[EPOLL_SIZE]; // I write 32 as example, will see after
	try
	{
		this->Init();
	}
	catch(const char* exception)
	{
		std::cerr << exception << std::endl;
	}
	while (1)
	{
		int epoll_event_count = epoll_wait(this->epfd, events, EPOLL_SIZE, -1);//-1 means will wait all the time
		if (epoll_event_count < 0)
		{
			throw("[ERROR]epoll failure");
			exit(0);
		}
		std::cout << "epoll_events_count = " << epoll_event_count << std::endl;
		for(int i = 0; i < epoll_event_count; i++)
		{
			int sockfd = events[i].data.fd;
			uint32_t ev = events[i].events;
			if (sockfd == this->listener)
				acceptConnect();
			else if (ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))/*err happened*/
				epoll_ctl(this->epfd, EPOLL_CTL_DEL, sockfd, NULL);
			else //if (ev & EPOLLIN)
				handle_client_event(*this, sockfd, web_conf);
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
	addfd(this->epfd, clientfd, true);
}

void handle_client_event(Server &server, int &clientfd, Conf &web_conf)
{
	Client_Request obj;
	int max_nb = web_conf.get_max_size_request();
	char buffer[max_nb];
	memset(buffer, 0, max_nb);
	long nb_read = recv(clientfd, buffer, sizeof(buffer), 0);
	if (nb_read < 0)
        send_error_page(204, obj, web_conf, clientfd);
	else
	{
		extract_info_from_first_line_of_buffer(obj, buffer, web_conf);
		extract_info_from_rest_buffer(obj, buffer);
		send_response(obj, clientfd);
	}
	close(clientfd);
	(void)server;
//	epoll_ctl(server.get_epfd(), EPOLL_CTL_DEL, clientfd, NULL);
}
