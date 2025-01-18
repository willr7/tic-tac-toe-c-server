#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "6060"

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int get_listener_socket(void) {
  int listener;
  int yes = 1;
  int rv;

  struct addrinfo hints, *ai, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }

    break;
  }

  freeaddrinfo(ai);

  if (p == NULL) {
    return -1;
  }

  if (listen(listener, 10) == -1) {
    return -1;
  }

  return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count,
                 int *fd_size) {
  if (*fd_count == *fd_size) {
    *fd_size *= 2;

    *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
  }

  (*pfds)[*fd_count].fd = newfd;
  (*pfds)[*fd_count].events = POLLIN;
  // (*pfds)[*fd_count].events = POLLIN;

  (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
  pfds[i] = pfds[*fd_count - 1];

  (*fd_count)--;
}

int main(void) {
  // Create listener socket and start listening
  //
  // When the listener socket receives a connection from a new socket,
  // Accept the connection from that socket and add it to the sockets that you
  // are polling
  //
  // Keep a queue of the sockets that join, and pop the sockets from that queue
  // when you have enough players to start a game (2)
  //
  // Keep a queue of all of the buddy queues
  //
  // When you receive a message from one of the other sockets (that isn't the
  // listener), Then you want to send that message to that socket's buddy
  int listener;

  int newfd;
  struct sockaddr_storage remoteaddr;
  socklen_t addrlen;

  char buf[256];

  char remoteIP[INET6_ADDRSTRLEN];

  char turn = 'x';
  char locs[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

  int fd_count = 0;
  int fd_size = 5;
  struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

  int socket_queue[2] = {0, 0};
  int socket_pairs[20];
  memset(&socket_pairs, 0, sizeof(socket_pairs));

  listener = get_listener_socket();

  if (listener == -1) {
    fprintf(stderr, "error getting listening socket\n");
    exit(1);
  }

  pfds[0].fd = listener;
  pfds[0].events = POLLIN;

  fd_count = 1;

  for (;;) {
    // poll for new events
    // POLLIN sends a
    int poll_count = poll(pfds, fd_count, -1);

    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }

    for (int i = 0; i < fd_count; i++) {
      if (pfds[i].revents & POLLIN) {
        if (pfds[i].fd == listener) {
          addrlen = sizeof(remoteaddr);
          newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

          if (newfd == -1) {
            perror("accept");
          } else {
            add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

            printf("pollserver: new connection from %s on socket %d\n",
                   inet_ntop(remoteaddr.ss_family,
                             get_in_addr((struct sockaddr *)&remoteaddr),
                             remoteIP, INET6_ADDRSTRLEN),
                   newfd);
          }

          if (send(newfd, &turn, sizeof(turn), 0) == -1) {
            perror("failed to send player turn message");
          }
          turn = turn ^ ('x' ^ 'o');

          if (socket_queue[0] == 0) {
            // add to queue
            //  TODO: add functionality to add more than one concurrent game
            socket_queue[0] = newfd;
          } else {
            // Start Game
            socket_queue[1] = newfd;

            socket_pairs[socket_queue[0]] = socket_queue[1];
            socket_pairs[socket_queue[1]] = socket_queue[0];

            // FIX: Change this to IPv6 / fix the IP 0.0.0.0 thing
            // (if it's even an issue, test on pi)
            struct sockaddr_in player_1_addr;
            struct sockaddr_in player_2_addr;

            socklen_t player_1_addr_len = sizeof(player_1_addr);
            socklen_t player_2_addr_len = sizeof(player_2_addr);

            if (getpeername(socket_queue[0], (struct sockaddr *)&player_1_addr,
                            &player_1_addr_len)) {
              perror("getpeername player 1 failed");
            }

            if (getpeername(socket_queue[1], (struct sockaddr *)&player_2_addr,
                            &player_2_addr_len)) {
              perror("getpeername player 2 failed");
            }

            char player_1_message[50];
            char player_2_message[50];

            snprintf(player_1_message, sizeof(player_1_message),
                     "Playing with player at %s\n",
                     inet_ntop(AF_INET, &player_1_addr.sin_addr, remoteIP,
                               INET_ADDRSTRLEN));

            snprintf(player_2_message, sizeof(player_2_message),
                     "Playing with player at %s\n",
                     inet_ntop(AF_INET, &player_2_addr.sin_addr, remoteIP,
                               INET_ADDRSTRLEN));

            if (send(socket_queue[0], player_1_message,
                     sizeof(player_1_message), 0) == -1) {
              perror("send player joined message error");
            }

            if (send(socket_queue[1], player_2_message,
                     sizeof(player_2_message), 0) == -1) {
              perror("send player joined message error");
            }

            socket_queue[0] = 0;
            socket_queue[1] = 0;
            // END Start Game
          }
        } else {
          int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0);
          int sender_fd = pfds[i].fd;

          if (nbytes <= 0) {
            if (nbytes == 0) {
              printf("pollserver: socket %d hung up\n", sender_fd);
            } else {
              perror("recv");
            }

            int socket_buddy = socket_pairs[pfds[i].fd];

            close(pfds[i].fd);

            socket_pairs[socket_buddy] = 0;
            socket_pairs[pfds[i].fd] = 0;

            del_from_pfds(pfds, i, &fd_count);
          } else {
            int dest_fd = socket_pairs[pfds[i].fd];
            if (dest_fd != listener && dest_fd != sender_fd) {
              if (send(dest_fd, buf, nbytes, 0) == -1) {
                perror("send");
              }
            }
          }
        }
      }
    }
  }
}
