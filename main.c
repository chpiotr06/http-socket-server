#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#define Address struct sockaddr_in


typedef struct HTTP_Server {
    int socket;
    int port;
} HTTP_Server;

typedef struct singleRoute {
    char* key;
    char* val;
    struct singleRoute *prev, *next;
} singleRoute ;

void initServer(HTTP_Server * server, int port);

singleRoute * initRoute(char* key, char* val);
singleRoute * addRoute(singleRoute * baseRoute, char* key, char* val);
singleRoute * getRoute(singleRoute * baseRoute, char* key);


int main(){

}

void initServer(HTTP_Server * server, int port){
    server->port = port;
    
    int socketFD = sockaddr(AF_INET, SOCK_STREAM, 0);

    Address servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(socketFD, (Address *) &servAddr, sizeof servAddr);

    server->socket = socketFD;

    listen(socketFD, 5);
}