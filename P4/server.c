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
#include <sys/socketvar.h>

int c[5],pos=0;
int len[5];
float arr[5][100];

void merge(float arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(float arr[], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;
 
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
 
        merge(arr, l, m, r);
    }
}
 
 void mergeArrays(float arr1[], float arr2[], int n1,
                            int n2, float arr3[])
{
    int i = 0, j = 0, k = 0;
      // traverse the arr1 and insert its element in arr3
      while(i < n1){
      arr3[k++] = arr1[i++];
    }
       
      // now traverse arr2 and insert in arr3
      while(j < n2){
      arr3[k++] = arr2[j++];
    }
       
      // sort the whole array arr3
      mergeSort(arr3, 0,n1+n2-1);
}

void copyArr(float* dArr, float* sArr,int len){
    for(int i = 0;i<len;i++)
            {
                dArr[i]=sArr[i];
            }
}

void end(){
    int blen;
    float barr[500],aux[500];
    for(int i=0;i<pos;i++)
        mergeSort(arr[i],0,len[i]-1);

    if(pos>2){
        mergeArrays(arr[0],arr[1],len[0],len[1],barr);
        blen = len[0]+len[1];
        for(int i=2;i<pos;i++)
        {
            copyArr(aux,barr,blen);
            mergeArrays(arr[i],aux,len[i],blen,barr);
            blen = blen + len[i];

        }
        for(int i=0;i<pos;i++){
            int res = send(c[i],&blen,sizeof(blen),0);
            if(res!=sizeof(blen))
            {
                printf("Error sending len to client\n");
                exit(1);
            }
            for(int j=0;j<blen;j++)
            {
                res = send(c[i],&barr[j],sizeof(barr[j]),0);
                if(res!=sizeof(barr[j])){
                    printf("Error sending elem to client\n");
                    exit(1);
                }
            }
            close(c[i]);
        }
    }
    exit(0);

}


int main(){
    int s,maxfd;
    struct sockaddr_in server,client;
    unsigned int l;
    fd_set readSet;

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


    FD_ZERO(&readSet);
    FD_SET(s,&readSet);
    maxfd=s;

    while(1)
    {
        int res = select(maxfd+1,&readSet,NULL,NULL,NULL);
        if(res>=1)
        {
            for(int i=0;i<pos;i++)
            {
                if(FD_ISSET(c[i],&readSet)){

                    int res = recv(c[i],&len[i],sizeof(len[i]),0);
                    if(res!=sizeof(len[i]))
                    {
                        printf("Error recieving len from client\n");
                        return 1;
                    }
                    if(len[i] == 0)
                        end();
                    for(int j=0;j<len[i];j++){
                        res = recv(c[i],&arr[i][j],sizeof(arr[i][j]),0);
                        if(res!=sizeof(arr[i][j]))
                        {
                            printf("Error recieving elem from client\n");
                            return 1;
                        }
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
            }

        }
        FD_ZERO(&readSet);
        for(int i=0;i<pos;i++){
            FD_SET(c[i],&readSet);
        }
        FD_SET(s,&readSet);
    }
}