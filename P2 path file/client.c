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
    char file[256]="\0",contents[1024]="\0";
    int size;
    int c;
    struct sockaddr_in server;

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

    printf("Give filepath:");
    scanf("%s",file);
    int res = send(c,&file,strlen(file),0);
    if(res!=strlen(file))
    {
        printf("Error sending to server\n");
        return 1;
    }

    res = recv(c,&size,sizeof(size),0);
    if(res!=sizeof(size))
    {
        printf("Error recieving from server\n");
        return 1;
    }

    res = recv(c,&contents,sizeof(contents),0);
    if(res!=strlen(contents))
    {
        printf("Error recieving from server\n");
        return 1;
    }

    size = ntohs(size);

    strcat(file,"-copy");

    FILE* f = fopen(file,"w");
    write(fileno(f),contents,strlen(contents));
    close(fileno(f));

    printf("Size of file is %d\n",size);

    close(c);
}