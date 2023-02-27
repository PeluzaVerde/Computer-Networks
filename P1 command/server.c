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

int c;

void tratare(){
    char buffer[256];
    int res = recv(c,&buffer,sizeof(buffer),0);
    if(res != strlen(buffer))
    {
        printf("Error recieving from client\n");
        return;
    }
    int fd[2];
    pipe(fd);
    dup2(fd[1],fileno(stdout));
    int code = system(buffer);
    memset(&buffer,0,sizeof(buffer));
    read(fd[0],buffer,sizeof(buffer));
    close(fd[1]);
    close(fd[0]);

/*
    for(int i = strlen(buffer)-1;i>=0;i--)
        if(buffer[i]=='\n')
        {
            buffer[i]='\0';
            break;
        }
*/
    code = htons(code);

    res = send(c,&code,sizeof(code),0);
    if(res != sizeof(code))
    {
        printf("Error sending to client\n");
        return;
    }

    res = send(c,&buffer,strlen(buffer),0);
    if(res != strlen(buffer))
    {
        printf("Error sending to client\n");
        return;
    }
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