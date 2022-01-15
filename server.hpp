#ifndef SERVER_HPP
# define SERVER_HPP

#define EPOLL_SIZE 32

class Server;
#include "webserv.hpp"
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <list>
#include <stdint.h>

void handle_client_event(Server &server, int &clientfd, Conf &c);

class Server
{
private:
	struct sockaddr_in serverAddr;
	int listener;
	int epfd;
	int port;
	std::map<int, std::string> error_page_map;
public:
	static void addfd(int epfd, int fd, bool enable_et)
	{
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN;
		if (enable_et)
			ev.events = EPOLLIN | EPOLLET;// read edge-triggered let me think a while
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
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

	void acceptConnect();
};

/*ref : Level-triggered vs Edge-triggered
  https://stackoverflow.com/questions/1966863/level-vs-edge-trigger-network-event-mechanisms
 */
#endif
