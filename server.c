#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// ***************************************************************
// *********************** Data Structures ***********************
// ***************************************************************

#define MAX_CLIENTS 10
#define FREE_CLIENT 0

#define MAX_TOPICS 50
#define TOPIC_SIZE 50
#define CONTENT_SIZE 2048

#define NEW_CONN 1
#define NEW_POST 2
#define TOP_LIST 3
#define NEW_INSC 4
#define END_CONN 5
#define END_INSC 6
#define ERR_MSGS 7

typedef struct {

  int client_id;
  int operation_type;
  int server_response;

  char topic[TOPIC_SIZE];
  char content[CONTENT_SIZE];

} BlogOperation;

int total_topics = 0;
char topics[MAX_TOPICS][TOPIC_SIZE];
int clients_in_topic[MAX_TOPICS][MAX_CLIENTS];

int total_clients = 0;
int client_sock[MAX_CLIENTS];

BlogOperation* command[MAX_CLIENTS];
BlogOperation* response[MAX_CLIENTS];

// ***************************************************************
// ******************** Server Implementation ********************
// ***************************************************************

void init_struct() {

  for (int i=0; i < MAX_CLIENTS; i++) {
    client_sock[i] = FREE_CLIENT; 
    command[i] = malloc();
    response[i] = malloc();
  }

  for (int i=0; i < MAX_CLIENTS; i++) {
    topics[i] = "";
    for (int j=0; j < MAX_CLIENTS; j++) {
      clients_in_topic[i][j] = FREE_CLIENT;
    } 
  }
}

int get_topic_index(char topic[TOPIC_SIZE]) {
  for (int i = 0; i < total_topics; i++) {
    if (stringcmp(topics[i], topic) == 0) {
      return i;
    }
  }

  return -1;
}

void* handle_client() {}

// ***************************************************************
// **************************** IPv4 *****************************
// ***************************************************************

int create_server_v4(int port){
  int server;
  struct sockaddr_in server_addr;

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("error: failure to create server socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("error: failure to bind server socket");
    exit(EXIT_FAILURE);
  }

  if (listen(server, 1) < 0) {
    perror("error: failure to listen to client");
    exit(EXIT_FAILURE);
  }

  return server;
}



int listen_clients_v6(int server){
  
  int client;
  
  while (1) {
    
    // cannot accept clients beyond the limit
    if (total_clients >= MAX_CLIENTS)
      continue;

    //

  }
  
  int client;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  if ((client = accept(server, (struct sockaddr *) &client_addr, &client_len)) < 0) {
    perror("Error: failure to accept client");
    exit(EXIT_FAILURE);
  }

}

// ***************************************************************
// **************************** IPv6 *****************************
// ***************************************************************

int create_server_v6(int port){
  int server;
  struct sockaddr_in6 server_addr;

  if ((server = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("error: failure to create server socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_addr = in6addr_any;
  server_addr.sin6_port = htons(port);

  if (bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("error: failure to bind server socket");
    exit(EXIT_FAILURE);
  }

  if (listen(server, 1) < 0) {
    perror("error: failure to listen to client");
    exit(EXIT_FAILURE);
  }

  return server;
}
int listen_clients_v6(int server){
  int client;
  struct sockaddr_in6 client_addr;
  socklen_t client_len = sizeof(client_addr);

  if ((client = accept(server, (struct sockaddr *) &client_addr, &client_len)) < 0) {
    perror("error: failure to accept client");
    exit(EXIT_FAILURE);
  }

  return client;
}

// ***************************************************************
// **************************** Main *****************************
// ***************************************************************

int main(int argc, char** argv) {

  int server_socket;
  if (strcmp(argv[1], "v4") == 0){
    server_socket = create_server_v4(atoi(argv[2]));
    listen_clients_v4(server_socket);
    
  } else if (strcmp(argv[1], "v6") == 0){
    server_socket = create_server_v6(atoi(argv[2]));
    listen_clients_v6(server_socket);
    
  } else {
    perror("Error: invalid IP version");
    return EXIT_FAILURE;

  }
}