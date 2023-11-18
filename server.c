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
#define ERR_MSGS -1

typedef struct {

  int client_id;
  int operation_type;
  int server_response;

  char topic[TOPIC_SIZE];
  char content[CONTENT_SIZE];

} BlogOperation;

int total_clients = 0;
int client_sock[MAX_CLIENTS];

int total_topics = 0;
char topics[MAX_TOPICS][TOPIC_SIZE];
int clients_in_topic[MAX_TOPICS][MAX_CLIENTS];

BlogOperation request[MAX_CLIENTS];
BlogOperation response[MAX_CLIENTS];

// ***************************************************************
// ******************** Server Implementation ********************
// ***************************************************************

void init_struct() {

  for (int i=0; i < MAX_CLIENTS; i++) {
    client_sock[i] = FREE_CLIENT; 
  }

  for (int i=0; i < MAX_TOPICS; i++) {
    topics[i] = "";
    for (int j=0; j < MAX_CLIENTS; j++) {
      clients_in_topic[i][j] = 0;
    } 
  }
}

int get_topic_index(char topic[TOPIC_SIZE]) {
  // check if topic already exist
  for (int i = 0; i < total_topics; i++) {
    if (stringcmp(topics[i], topic) == 0) {
      return i;
    }
  }

  return -1;
}

void create_topic(char topic[TOPIC_SIZE]) {
  // check if we can store one more topic
  if (total_topics >= MAX_TOPICS) {
    perror("maximum number of topics exceeded");
    exit(EXIT_FAILURE);
  }

  // open new topic
  strcpy(topic[total_topics], topic);
  total_topics++;
}

int generate_client_id(int cli_sock) {
  // check if client not already in
  for (int i=0; i<MAX_CLIENTS; i++) {
    if (client_id[i] == cli_sock)
      return i;
  }

  // check if there is space for a new client
  if (total_clients >= MAX_CLIENTS) {
    perror("maximum number of clients exceeded");
    exit(EXIT_FAILURE);
  }

  // replace a free slot
  for (int i=0; i<MAX_CLIENTS; i++) {
    if (client_id[i] == FREE_CLIENT) {
      total_clients++;
      return i;
    }
  }
}

void broadcast_post(int client) {
  // check if topic exist
  int index = get_topic_index(request[client].topic);
  if (index < 0) {
    response[client].operation_type = ERR_MSGS;
    strcpy(response[client].content, "error: invalid topic");
    return;
  }

  // send post to each subscriber
  for (int i=0; i < total_clients; i++) {
    // skip clients not subscribed
    if (clients_in_topic[index][i] == 0)
      continue;
    
    response[i].client_id = client;
    response[i].operation_type = NEW_POST;
    response[i].server_response = 1;

    strcpy(response[i].topic, request[client].topic);
    strcpy(response[i].content, request[client].content);

    send(client_id[i], &response[i], sizeof(response[i]), 0);
  }

  printf("new post added in %s by %d", request[client].topic, client);
}

void list_all_topics(int client) {
  // check if there are topics to list
  if (total_topics == 0) {
    strcpy(response[client].content, "no topics available");
    return;
  }
  
  // list topics
  strcpy(response[client].content, topics[0]);
  for (int i=1; i < total_topics; i++) {
    strcat(response[client].content, ";");
    strcat(response[client].content, topics[i]);
  }
  strcat(response[client].content, "\n");
}

void subscribe_to_topic(int client) {
  // find or generate topic
  int index = get_topic_index(request[client].topic);
  if (index < 0) {
    create_topic(request[client].topic);
    return;
  }

  // if client already subscribed
  if (topics[index][client]) {
    response[client].operation_type = ERR_MSGS;
    strcpy(response[client].content, "error: already subscribed");
    return;
  }

  // subscribe
  clients_in_topic[index][client] = 1;
  printf("client %d subscribed to %s", client, request[client].topic);
}

void free_client(int client) {
  for (int i=0; i<MAX_TOPICS; i++) {
    clients_in_topic[i][client] = 0;
  }
  client_id[client] = FREE_CLIENT;
  total_clients--;
  printf("client %d was disconnected", client);
}

void unsubscribe_to_topic(int client) {
  // check if topic exist
  int index = get_topic_index(request[client].topic);
  if (index < 0) {
    response[client].operation_type = ERR_MSGS;
    strcpy(response[client].content, "error: invalid topic");
    return;
  }

  // unsub
  topics[index][client] = 0;
  printf("client %d unsubscribed to %s", client, request[client].topic);
}

void* handle_client(void* arg) {
  int client = *(int*)args;
  int connected = 1;
  int answer_request;
  while (connected) {
    
    // wait for request
    ssize_t bytes_received = recv(client_id[client], &request[client], sizeof(request[client]), 0);
    if (bytes_received <= 0) {
      perror("error receiving data");
      break;
    }
    
    answer_request = 0;
    // handle request
    switch (request.operation_type) {
      case NEW_CONN:
        response[client].operation_type = ERR_MSGS;
        strcpy(response[client].content, "error: client already connected");
        break;

      case NEW_POST:
        broadcast_post(client);
        break;

      case TOP_LIST:
        list_all_topics(client);
        answer_request = 1;
        break;

      case NEW_INSC:
        subscribe_to_topic(client);
        break;

      case END_CONN:
        free_client(client);
        connected = 0;
        break;

      case END_INSC:
        unsubscribe_to_topic(client);
        break;
    }

    if (response[client].operation_type == ERR_MSGS) 
      answer_request = 1;
    
    // answer request, if necessary
    if (answer_request) 
      response[client].server_response = 1;
      send(client_id[client], &response[client], sizeof(response[client]), 0);
  }
}

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

void listen_clients_v4(int server){

  int client;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  
  while (1) {
    // cannot accept clients beyond the limit, but check if it is still full
    if (total_clients >= MAX_CLIENTS)
      continue;

    // accept and receive first message from client
    if ((client = accept(server, (struct sockaddr *) &client_addr, &client_len)) < 0) {
      perror("error: failure to accept client");
      exit(EXIT_FAILURE);
    }

    // handle connection
    int index = generate_client_id(client);
    client_id[index] = client;
    ssize_t bytes_received = recv(client_id[index], &request[index], sizeof(request[index]), 0);
    if (bytes_received <= 0) {
      perror("error receiving data");
      break;
    }

    if (request[index].operation_type != NEW_CONN) {
      perror("error: improper connection");
      break;
    }

    // create client handling thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, handle_client, (void *)&index) != 0) {
      perror("error creating client handling thread");
      exit(EXIT_FAILURE);
    }

    // send confirmation
    response[index].client_id = index;
    response[index].operation_type = NEW_CONN;
    response[index].server_response = 1;

    send(client_id[index], &response[index], sizeof(response[index]), 0);
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

void listen_clients_v6(int server){
  int client;
  struct sockaddr_in6 client_addr;
  socklen_t client_len = sizeof(client_addr);

  while (1) {
    // cannot accept clients beyond the limit, but check if it is still full
    if (total_clients >= MAX_CLIENTS)
      continue;

    // accept and receive first message from client
    if ((client = accept(server, (struct sockaddr *) &client_addr, &client_len)) < 0) {
      perror("error: failure to accept client");
      exit(EXIT_FAILURE);
    }

    // handle connection
    int index = generate_client_id(client);
    client_id[index] = client;
    ssize_t bytes_received = recv(client_id[index], &request[index], sizeof(request[index]), 0);
    if (bytes_received <= 0) {
      perror("error receiving data");
      break;
    }

    if (request[index].operation_type != NEW_CONN) {
      perror("error: improper connection");
      break;
    }

    // create client handling thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, handle_client, (void *)&index) != 0) {
      perror("error creating client handling thread");
      exit(EXIT_FAILURE);
    }

    // send confirmation
    response[index].client_id = index;
    response[index].operation_type = NEW_CONN;
    response[index].server_response = 1;

    send(client_id[index], &response[index], sizeof(response[index]), 0);
  }
}

// ***************************************************************
// **************************** Main *****************************
// ***************************************************************

int main(int argc, char** argv) {

  int server_socket;
  init_struct();
  if (strcmp(argv[1], "v4") == 0){
    server_socket = create_server_v4(atoi(argv[2]));
    listen_clients_v4(server_socket);
    
  } else if (strcmp(argv[1], "v6") == 0){
    server_socket = create_server_v6(atoi(argv[2]));
    listen_clients_v6(server_socket);
    
  } else {
    perror("error: invalid IP version");
    return EXIT_FAILURE;
  }
}