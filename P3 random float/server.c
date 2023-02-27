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
#include <math.h>
#include <time.h>

int c[5],pos=0;
float cnr[5],snr;
fd_set readSet;

float float_rand(float minimum, float maximum)
{
    float scale = rand() / (float)RAND_MAX;       /* [0, 1.0] */
    return minimum + scale * (maximum - minimum); /* [min, max] */
}

void end(int semnal){
    char buffer[256];
    
    if(pos>0){

        int max = 0;
        for(int i =1;i<pos;i++){
            if(fabs(snr-cnr[max])>fabs(snr-cnr[i]))
                max=i;
        }

        for(int i=0;i<pos;i++){
            memset(&buffer,0,sizeof(buffer));
            if(max==i)
                sprintf(buffer,"You have the best guess with an error of %f\n",fabs(snr-cnr[max]));
            else
                sprintf(buffer,"You lost\n");
            int res = send(c[i],&buffer,strlen(buffer),0);
            if(res!=strlen(buffer)){
                printf("Error sending data\n");
                return;
            }
            close(c[i]);
        }

    }
    exit(0);
}


int main(){
    srand((unsigned int)time(NULL));
    int s,maxfd;
    struct sockaddr_in server,client;
    unsigned int l;
    char buffer[256];

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

    snr=float_rand(0.0,100.0);
    printf("Server number is %f\n",snr);

    FD_ZERO(&readSet);
    FD_SET(s,&readSet);
    maxfd=s;

    signal(SIGALRM,end);
    alarm(10);

    float aux;

    while(1)
    {
        int res = select(maxfd+1,&readSet,NULL,NULL,NULL);
        if(res>=1)
        {
            for(int i=0;i<pos;i++)
            {
                if(FD_ISSET(c[i],&readSet)){
                    memset(&buffer,0,sizeof(buffer));
                    int rec = recv(c[i],&buffer,sizeof(buffer),0);
                    if(rec!=strlen(buffer))
                    {
                        printf("Error recieving from client\n");
                        return 1;
                    }
                    sscanf(buffer,"%f",&aux);
                    if(abs(snr-cnr[i])>abs(snr-aux))
                    {
                        cnr[i] = aux;
                        if(abs(snr-cnr[i])==0.0)
                            end(0);
                    }
                }
            }
            if(FD_ISSET(s,&readSet)){
                memset(&client, 0, sizeof(client));
                l = sizeof(client);
                c[pos] = accept(s,(struct sockaddr*)&server, &l);
                if(c[pos]>maxfd)
                    maxfd=c[pos];
                pos++;
                alarm(10);
            }

        }
        FD_ZERO(&readSet);
        for(int i=0;i<pos;i++){
            FD_SET(c[i],&readSet);
        }
        FD_SET(s,&readSet);
    }
}