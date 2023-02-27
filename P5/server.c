#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <sys/stat.h>

int c;

void tratare(){
    char buffer[1024];
    memset(&buffer,0,sizeof(buffer));
    int res = recv(c,&buffer,sizeof(buffer),0);
    if(res != strlen(buffer))
    {
        printf("Error recieving from client\n");
        exit(1);
    }

    int d;
    struct sockaddr_in domain;
    struct hostent *h;
    h=gethostbyname(buffer);

    d = socket(AF_INET, SOCK_STREAM, 0);
    if(d<0){
        printf("Error creating socket\n");
        exit(0);
    }

    memset(&domain,0,sizeof(domain));
    domain.sin_family = AF_INET;
    domain.sin_port = htons(80);
    inet_aton(inet_ntoa(*((struct in_addr *)h->h_addr)),&domain.sin_addr);
    if(connect(d,(struct sockaddr*)&domain,sizeof(domain))<0){
        printf("Error connecting\n");
        exit(0);
    }

    strcpy(buffer,"GET / HTTP/1.0\n\n");

    res = send(d,&buffer,strlen(buffer),0);
    if(res!=strlen(buffer)){
        printf("Error sending to domain\n");
        exit(0);
    }

    memset(&buffer,0,sizeof(buffer));
    res = recv(d,&buffer,sizeof(buffer),0);
    if(res!=strlen(buffer)){
        printf("Error recieving from domain\n");
        exit(0);
    }

    res = send(c,&buffer,strlen(buffer),0);
    if(res!=strlen(buffer)){
        printf("Error recieving from domain\n");
        exit(0);
    }

    while(isfdtype(d,S_IFSOCK)){}
    close(c);
    
    exit(0);
}

int main(){
    int s;
    struct sockaddr_in server,client;
    unsigned int l;

    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("Error creating socket\n");
        return 1;
    }

    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    server.sin_addr.s_addr = INADDR_ANY;
    if(bind(s ,(struct sockaddr*)&server, sizeof(server))<0){
        perror("Binding error:");
        return 1;
    }

    listen(s,5);

    while(1){
        memset(&client, 0, sizeof(client));
        l = sizeof(client);
        c = accept(s,(struct sockaddr*)&server, &l);

        if(fork()==0)
        {
            tratare();
        }
    }
}