#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>
void bind_socket(int *sockfd, struct addrinfo *servinfo);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);

#endif // SERVER_H_
