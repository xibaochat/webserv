#ifndef SERVER_HPP
# define SERVER_HPP

#define EPOLL_SIZE 1024

class Server;
#include "webserv.hpp"
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <list>
#include <stdint.h>

class Server
{
private:
	std::list<int> list;
	struct sockaddr_in serverAddr;
	int listener;
	int epfd;
	int port;
	std::map<int, std::string> error_page_map;
	Conf  web_conf;
	std::map<int, std::string> request_map;

public:
	void handle_client_event(int &clientfd, Conf &c);
	void addfd(int fd, bool enable_et)
	{
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));
		ev.data.fd = fd;
		ev.events = EPOLLIN | EPOLLET;
		if (enable_et)
			ev.events = EPOLLIN | EPOLLET;// read edge-triggered let me think a while
		if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
			throw("[ERROR]Failed to in epoll_ctl");
		fcntl(fd, F_SETFL, O_NONBLOCK);
		std::cout << "fd added to epoll" << std::endl;
	}
	Server(Conf &web_conf);
	~Server(){this->Close();}
	void Init();
	void Close();
	void Start(Conf &web_conf);
	Server(Server const &src){*this = src;}
	int get_listener(){return this->listener;}
	int get_epfd(){return this->epfd;}
	std::map<int, std::string> get_request_map(){return this->request_map;}
	void acceptConnect();
};

/*ref : Level-triggered vs Edge-triggered
  https://stackoverflow.com/questions/1966863/level-vs-edge-trigger-network-event-mechanisms
 */
#endif
