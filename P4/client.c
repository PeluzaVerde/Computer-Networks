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

int main(){
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

    int len;
    float arr[500];

    printf("Give len for array:");
    scanf("%d",&len);
    int res = send(c,&len,sizeof(len),0);
    if(res!=sizeof(len)){
        printf("Error sending len to server\n");
        return 1;
    }

    for(int i=0;i<len;i++){
        printf("Give number:");
        scanf("%f",&arr[i]);
        res = send(c,&arr[i],sizeof(arr[i]),0);
        if(res!=sizeof(arr[i])){
            printf("Error sending elem to server\n");
            return 1;
        }
    }

    res = recv(c,&len,sizeof(len),0);
    if(res!=sizeof(len)){
        printf("Error recieving len from server\n");
        return 1;
    }

    memset(&arr,0,sizeof(arr));
    printf("Len of array is %d\n",len);
    printf("Array: ");
    for(int i=0;i<len;i++){
        res = recv(c,&arr[i],sizeof(arr[i]),0);
        if(res!=sizeof(arr[i])){
            printf("Error recieving elem to server\n");
            return 1;
        }
        printf("%f ",arr[i]);
    }
    printf("\n");
}