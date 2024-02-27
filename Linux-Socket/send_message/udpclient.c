#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8900
#define MAXSIZE 1024

int main(){
    int sockfd;
    socklen_t len;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    char send_buf[MAXSIZE];
    char recv_buf[MAXSIZE];
 
    if(-1==(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
        perror("create error!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);


    while(1)
    {  
        len = sizeof(server_addr);
        
        memset(send_buf, 0, MAXSIZE);
        memset(recv_buf, 0, MAXSIZE);
        
        printf("client:");
        gets(send_buf);
        
        sendto(sockfd, send_buf, MAXSIZE, 0, (struct sockaddr*)&server_addr, len);
        
        recvfrom(sockfd, recv_buf, MAXSIZE, 0, (struct sockaddr*)&client_addr, &len);
        printf("server:%s\n",recv_buf);

    }

    close(sockfd);
    return 0;
}
