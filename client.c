#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

// ***************************************************************
// *********************** Data Structures ***********************
// ***************************************************************

#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048

#define NEW_CONN 1
#define NEW_POST 2
#define TOP_LIST 3
#define NEW_INSC 4
#define END_CONN 5
#define END_INSC 6
// #define ERR_MSGS 7

typedef struct {
  int client_id;
  int operation_type;
  int server_response;

  char topic[TOPIC_SIZE];
  char content[CONTENT_SIZE];

} BlogOperation;

int myID; 
int is_connected = 0;

BlogOperation* message;  // to server 
BlogOperation* response; // from server 

// ***************************************************************
// *********************** Implementations ***********************
// ***************************************************************

void* handle_responses() {
  
}

void handle_input() {

}

// ***************************************************************
// **************************** IPv4 *****************************
// ***************************************************************

int create_and_connect_v4(char* ip, int port) {
  int client;
  struct sockaddr_in server_addr;

  inet_pton(AF_INET, ip, &server_addr.sin_addr);

  if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("error: failure to create socket");
    exit(EXIT_FAILURE);
  }
  
  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    perror("error: invalid address");
    exit(EXIT_FAILURE);
  }

  if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("error: failure to connect to server");
    exit(EXIT_FAILURE);
  }

  return client;
}

// ***************************************************************
// **************************** IPv6 *****************************
// ***************************************************************

int create_and_connect_v6(char* ip, int port) {
  int client;
  struct sockaddr_in6 server_addr;

  inet_pton(AF_INET6, ip, &server_addr.sin6_addr);

  if ((client = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("error: failure to create socket");
    exit(EXIT_FAILURE);
  }
  
  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(port);

  if (inet_pton(AF_INET6, ip, &server_addr.sin6_addr) <= 0) {
    perror("error: invalid address");
    exit(EXIT_FAILURE);
  }

  if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("error: failure to connect to server");
    exit(EXIT_FAILURE);
  }

  return client;
}

int main(int argc, char** argv) {
    
  int client_socket;
  if (strchr(argv[1], '.') != NULL) {
    client_socket = create_and_connect_v4(argv[1], atoi(argv[2]));
    
  } else if (strchr(argv[1], ':') != NULL) {
    client_socket = create_and_connect_v6(argv[1], atoi(argv[2]));
    
  } else {
    perror("error: invalid IP address");
    exit(EXIT_FAILURE);
  }

  int connected = 1;

  close(sock);
  return 0;
}
