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
#include <sys/wait.h>
#include <stdlib.h>

int main(){
    int c;
    struct sockaddr_in server;
    char buffer[256];

    c = socket(AF_INET, SOCK_STREAM, 0);
    if(c<0){
        printf("Error creating socket\n");
        return 1;
    }

    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    inet_aton("127.0.0.1",&server.sin_addr);
    if(connect(c,(struct sockaddr*)&server,sizeof(server))<0){
        printf("Error connecting\n");
        return 1;
    }

    fgets(buffer,sizeof(buffer),stdin);
    int res = send(c,&buffer,strlen(buffer),0);
    if(res!=strlen(buffer))
    {
        printf("Error sending to server\n");
        return 1;
    }

    int code;
    res = recv(c,&code,sizeof(code),0);
    if(res!=sizeof(code))
    {
        printf("Error recieving from server\n");
    }

    code = ntohs(code);

    memset(&buffer,0,sizeof(buffer));
    res = recv(c,&buffer,sizeof(buffer),0);
    if(res!=strlen(buffer))
    {
        printf("Error recieving from server\n");
    }
    printf("%s",buffer);
    printf("Exit code is %d\n",code);
}