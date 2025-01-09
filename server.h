#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>
void bind_socket(int *sockfd, struct addrinfo *servinfo);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int start_server(int *sockfd, char incoming_address[INET6_ADDRSTRLEN],
                 char PORT[], int BACKLOG);

#endif // SERVER_H_
