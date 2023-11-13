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

BlogOperation* message;

// ***************************************************************
// *********************** Implementations ***********************
// ***************************************************************

void init_struct() {}

int get_topic_index(char topic[TOPIC_SIZE]) {
  for (int i = 0; i < total_topics; i++) {
    if (stringcmp(topics[i], topic) == 0) {
      return i;
    }
  }

  return -1;
}

void* handle_client() {}

void listen_for_clients() {}

