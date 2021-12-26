#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
int main()
{
  // Create a socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cout << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(9999);
  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
  {
    std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 10) < 0)
  {
	  std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
	  exit(EXIT_FAILURE);
  }

  auto addrlen = sizeof(sockaddr);
  int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
  if (connection < 0)
  {
    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  char buffer[100];
  auto bytesRead = read(connection, buffer, 100);//read from user
//  recv(sockfd, buffer, 100, 0);
  std::cout << "The message was: " << buffer;// print it

  std::string response = "Good talking to you\n";//send to user feedback
  send(connection, response.c_str(), response.size(), 0);

  close(connection);
  close(sockfd);

}
