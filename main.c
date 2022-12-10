



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

}