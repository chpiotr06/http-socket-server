#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define Address struct sockaddr_in
#define HASH_TABLE_SIZE 10

typedef struct HTTP_Server {
    int socket;
    int port;
} HTTP_Server;

struct MapNode {
  char *key;
  char *value;
  struct MapNode *next;
};

/* The hash table */
struct Map {
  struct MapNode *table[HASH_TABLE_SIZE];
};

void initServer(HTTP_Server * server, int port);

void map_init(struct Map *map);
unsigned int hash(char *key);
char * map_get(struct Map *map, char *key);
void map_set(struct Map *map, char *key, char *value);

char * renderFile(char * fileName);

int main(){
    HTTP_Server Server;
    struct Map map;
    int connfd;

    initServer(&Server, 4000);
    map_init(&map);
    map_set(&map, "/", "index.html");
    map_set(&map, "/authors", "authors.html");
    map_set(&map, "/authors", "authors.html");
    map_set(&map, "/contact", "contact.html");
    map_set(&map, "/about", "about.html");


    while(1){
        char clientMsg[4096] = "";
        char *method = "";
        char *urlRoute = "";
        char *clientHeader;
        char *headerToken;
        int parseCounter = 0;
        char template[100] = "";
        char *responseData;
        char responseHeader[4096] = "HTTP/1.1 200 OK\r\n\r\n";

        connfd = accept(Server.socket, NULL, NULL);
        read(connfd, clientMsg, 4095);

        printf("hello world");

        clientHeader = strtok(clientMsg, "\n");
        headerToken = strtok(clientHeader, " ");

        while(headerToken != NULL){
            switch(parseCounter){
                case 0:
                    method = headerToken;
                case 1:
                    urlRoute = headerToken;
            }
            headerToken = strtok(NULL, " ");
            parseCounter ++;
        }

        if(strstr(urlRoute, "/static/") != NULL){
            strcat(template, "static/index.css");
        }else{
            char *destination = map_get(&map, urlRoute);
            strcat(template, "templates/");

            if(destination == NULL){
                strcat(template, "404.html");
            }else{
                strcat(template, destination);
            }
        }

        responseData = renderFile(template);
        printf("\n%s", responseData);

        strcat(responseHeader, responseData);
        strcat(responseHeader, "\r\n\r\n");

        send(connfd, responseHeader, sizeof(responseHeader),0);
        close(connfd);
        free(responseData);
    }

    return 0;
}

void initServer(HTTP_Server * server, int port){
    server->port = port;
    
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFD < 0 ){
        printf("Socket error");
    }

    Address servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socketFD, (const Address *) &servAddr, sizeof servAddr) < 0){
        printf("Bind error");
    }

    server->socket = socketFD;

    if(listen(socketFD, 5) < 0){
        printf("Listen error");
    }
	printf("HTTP Server Initialized\nPort: %d\n", server->port);
}

/* Initialize a new map */
void map_init(struct Map *map) {
  for (int i = 0; i < HASH_TABLE_SIZE; i++) {
    map->table[i] = NULL;
  }
}

/* Hash a string to an integer in the range [0, HASH_TABLE_SIZE) */
unsigned int hash(char *key) {
  unsigned int hash = 5381;
  int c;
  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % HASH_TABLE_SIZE;
}

/* Look up the value for a given key in the map */
char *map_get(struct Map *map, char *key) {
  unsigned int index = hash(key);
  struct MapNode *node = map->table[index];
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

/* Set the value for a given key in the map */
void map_set(struct Map *map, char *key, char *value) {
  unsigned int index = hash(key);
  struct MapNode *node = map->table[index];
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      free(node->value);
      node->value = strdup(value);
      return;
    }
    node = node->next;
  }
  node = malloc(sizeof(struct MapNode));
  node->key = strdup(key);
  node->value = strdup(value);
  node->next = map->table[index];
  map->table[index] = node;
}

char * renderFile(char * fileName){
    FILE* fileDesc = fopen(fileName, "r");
    char character;
    char * file;
    int iterator = 0;
    long fileSize;

    if(fileDesc == NULL){
        printf("File is null");
        return NULL;
    }
    
    fseek(fileDesc, 0, SEEK_END);
    fileSize = ftell(fileDesc);
    fseek(fileDesc, 0, SEEK_SET);

    file = malloc(sizeof(char) * (fileSize+1));

    while((character = fgetc(fileDesc)) != EOF){
        file[iterator] = character;
        iterator += 1;
    }

    fclose(fileDesc);
    return file;
}