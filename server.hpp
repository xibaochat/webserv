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
public:
	struct sockaddr_in *serverAddr;
	int *listener;
	int epfd;
	std::string	_request;
	std::set<int> port;
	std::map<int, std::string> error_page_map;
	Conf  web_conf;
	std::vector<Conf> web_conf_vector;
	std::map<int, std::string> request_map;

public:
	int fd_is_in_listener(int fd);
	void handle_client_event(int &clientfd);
	void addfd(int fd, bool enable_et);
	Server(Conf &web_conf);
	~Server();
	void Init();
	void Close(int &fd);
	void Start(Conf &web_conf);
	Server(Server const &src){*this = src;}
	int* get_listener(){return this->listener;}
	int get_epfd(){return this->epfd;}
	std::map<int, std::string> get_request_map(){return this->request_map;}
	void acceptConnect(int &fd);
	void manage_event(struct epoll_event *events, int &epoll_event_count, std::map<int, std::string> &request_map);
	void send_content_to_request(int &fd);
};

/*ref : Level-triggered vs Edge-triggered
  https://stackoverflow.com/questions/1966863/level-vs-edge-trigger-network-event-mechanisms
 */
#endif
