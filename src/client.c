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
int send_request;

BlogOperation request;  
BlogOperation response;

// ***************************************************************
// *********************** Implementations ***********************
// ***************************************************************

void* handle_server(void* args) {
  int sock = *(int*) args;
  
  while (is_connected) {
    ssize_t bytes_received = recv(sock, &response, sizeof(response), 0);
    if (bytes_received <= 0) {
      printf("error receiving data\n");
      continue;
    }

    switch (response.operation_type) {
      case NEW_POST:
        printf("new post added in %s by %d\n", response.topic, response.client_id);
        puts(response.content);
        break;

      case TOP_LIST:
        puts(response.content);
        break;

      case NEW_CONN:
      case NEW_INSC:
      case END_CONN:
      case END_INSC:
        // undefined behabior as those ops do not receive response
        break;

      default:
        // error messages
        puts(response.content);
    }
  }
}

void handle_input() {
  char line[CONTENT_SIZE];
  char* token;
  char* delim = " \n";

  fgets(line, CONTENT_SIZE, stdin);
  token = strtok(line, delim);

  send_request = 1;
  if (strcmp(token, "publish") == 0)  {
    
    token = strtok(NULL, delim);
    if (strcmp(token, "in") == 0)
      token = strtok(NULL, delim);

    if (token == NULL) {
      printf("error: must especify the topic of the post\n");
      send_request = 0;
      return;
    }

    request.client_id = myID;
    request.operation_type = NEW_POST;
    request.server_response = 0;
    strcpy(request.topic, token);

    fgets(line, CONTENT_SIZE, stdin);
    strcpy(request.content, line);

    printf("Publishing post\n");

  } else if (strcmp(token, "list") == 0) {

    request.client_id = myID;
    request.operation_type = TOP_LIST;
    request.server_response = 0;
    strcpy(request.topic, "");
    strcpy(request.content, "");

    printf("Requesting topics list\n");

  } else if (strcmp(token, "subscribe") == 0) {

    token = strtok(NULL, delim);
    if (strcmp(token, "in") == 0 ||
        strcmp(token, "to") == 0)
      token = strtok(NULL, delim);

    if (token == NULL) {
      printf("error: must especify the topic to subscribe\n");
      send_request = 0;
      return;
    }

    request.client_id = myID;
    request.operation_type = NEW_INSC;
    request.server_response = 0;
    strcpy(request.topic, token);
    strcpy(request.content, "");

    printf("Subscribing\n");

  } else if (strcmp(token, "exit") == 0) {

    request.client_id = myID;
    request.operation_type = END_CONN;
    request.server_response = 0;
    strcpy(request.topic, "");
    strcpy(request.content, "");

    is_connected = 0;
    printf("Exiting\n");


  } else if (strcmp(token, "unsubscribe") == 0) {

    token = strtok(NULL, delim);
    if (strcmp(token, "in") == 0 ||
        strcmp(token, "to") == 0 ||
        strcmp(token, "from") == 0)
      token = strtok(NULL, delim);

    if (token == NULL) {
      printf("error: must especify the topic to unsubscribe\n");
      send_request = 0;
      return;
    }

    request.client_id = myID;
    request.operation_type = END_INSC;
    request.server_response = 0;
    strcpy(request.topic, token);
    strcpy(request.content, "");

    printf("Unsubscribing\n");


  } else {
    printf("error: invalid command\n");
    send_request = 0;
  }
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

// ***************************************************************
// **************************** Main *****************************
// ***************************************************************

int main(int argc, char** argv) {
    
  int sock;
  if (strchr(argv[1], '.') != NULL) {
    sock = create_and_connect_v4(argv[1], atoi(argv[2]));
    
  } else if (strchr(argv[1], ':') != NULL) {
    sock = create_and_connect_v6(argv[1], atoi(argv[2]));
    
  } else {
    perror("error: invalid IP address");
    return EXIT_FAILURE;
  }

  // starting exchange
  request.operation_type = NEW_CONN;
  request.server_response = 0;

  // send connection request
  send(sock, &request, sizeof(request), 0);
  ssize_t bytes_received = recv(sock, &response, sizeof(response), 0);
  if (bytes_received <= 0) {
    perror("error receiving data");
    return EXIT_FAILURE;
  }

  // check if response is according protocol
  if (response.operation_type != NEW_CONN || !response.server_response) {
    perror("error: improper communication");
    return EXIT_FAILURE;
  }

  // base configuration
  myID = response.client_id;
  request.client_id = myID;
  int is_connected = 1;

  // create server responses handler thread
  pthread_t thread;
  if (pthread_create(&thread, NULL, handle_server, (void *)&index) != 0) {
    perror("error creating client handling thread");
    return EXIT_FAILURE;
  }
  printf("Connected\n");
  while (is_connected) {
    handle_input();
    printf("Input handled\n");
    if (send_request) 
      send(sock, &request, sizeof(request), 0);
  }

  close(sock);
  return 0;
}
