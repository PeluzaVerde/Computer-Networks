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
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

float float_rand(float minimum, float maximum)
{
  float scale = rand() / (float)RAND_MAX;       /* [0, 1.0] */
  return minimum + scale * (maximum - minimum); /* [min, max] */
}

int main(){
    srand((unsigned int)time(NULL));
    int c,ok=0;
    struct sockaddr_in server;
    char buffer[256];
    float nr;

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


    while(!ok){
        nr = float_rand(0.0,100.0);
        printf("Your guess is %f\n",nr);
        memset(&buffer,0,sizeof(buffer));
        sprintf(buffer,"%f",nr);
        int res = send(c,&buffer,strlen(buffer),0);
        if(res!=strlen(buffer)){
            printf("Error sending to server\n");
            return 1;
        }
        sleep(1);

        memset(&buffer,0,sizeof(buffer));
        res = recv(c,&buffer,sizeof(buffer),MSG_DONTWAIT);
        if(res==strlen(buffer)){
            printf("%s\n",buffer);
            break;
        }

    }
    close(c);
}