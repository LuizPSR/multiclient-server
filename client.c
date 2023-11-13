#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// ***************************************************************
// *********************** Data Structures ***********************
// ***************************************************************

#define MAX_TOPICS_SUB 10
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

int myID; 
int is_connected = 0;
BlogOperation* message;

char my_topics[MAX_TOPICS_SUB][TOPIC_SIZE];

// ***************************************************************
// *********************** Implementations ***********************
// ***************************************************************

int createAndConnectSockToServerIPV4orIPV6(char* ip, int port){
    int sock;
    struct sockaddr_in servAddrV4;
    struct sockaddr_in6 servAddrV6;

    //IPV4
    if(inet_pton(AF_INET, ip, &servAddrV4.sin_addr) == 1){

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Failure to create socket");
            return -1;
        }
      
      memset(&servAddrV4, '0', sizeof(servAddrV4));
      servAddrV4.sin_family = AF_INET;
      servAddrV4.sin_port = htons(port);

      if (inet_pton(AF_INET, ip, &servAddrV4.sin_addr) <= 0) {
        perror("IPv4 address is not supported by server");
        return -1;
      }

      if (connect(sock, (struct sockaddr *)&servAddrV4, sizeof(servAddrV4)) < 0) {
        perror("Failure to connect to server");
        return -1;
      }

    //IPV6
    }else if(inet_pton(AF_INET6, ip, &servAddrV6.sin6_addr) == 1){

      if ((sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("Failure to create socket");
        return -1;
      }
      
      memset(&servAddrV6, '0', sizeof(servAddrV6));
      servAddrV6.sin6_family = AF_INET6;
      servAddrV6.sin6_port = htons(port);

      if (inet_pton(AF_INET6, ip, &servAddrV6.sin6_addr) <= 0) {
        perror("IPv4 address is not supported by server");
        return -1;
      }

      if (connect(sock, (struct sockaddr *)&servAddrV6, sizeof(servAddrV6)) < 0) {
        perror("Failure to connect to server");
        return -1;
      }
    }

    return sock;
}

void handleClientInput() {
  
}

void handleServerResponse(){

}

int main(int argc, char *argv[]){
    
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int sock;

    // Create and connect sock to server
    sock = createAndConnectSockToServerIPV4orIPV6(ip, port);

    // Stablish connection
    char buffer[sizeof(BlogOperation)];
    message = malloc(sizeof(BlogOperation));
    ssize_t bytes_received = recv(client_socket, &server_response, sizeof(server_response), 0);
    memcpy(&response, bufferResponse, sizeof(Payload));
    printf("%s\n", response.message);

    // Get own id from server confirmation
    myID = response.client_id;

    if(response.idMsg == ERROR){
        close(sock);
        return 0;
    }

    //Receive all users list
    recv(sock, bufferResponse, sizeof(Payload), 0);
    memcpy(&response, bufferResponse, sizeof(Payload));

    //Get the list of users and store in the database
    char *token = strtok(response.message, ", ");
    int count = 0;
    while (token != NULL) {
        clientsIdDb[count] = atoi(token);
        count++;

        token = strtok(NULL, ", ");
    }

    while(is_connected){
        
    }

    close(sock);

    return 0;
}
