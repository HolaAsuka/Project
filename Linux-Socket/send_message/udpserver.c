#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8900
#define MAXSIZE 1024

int main(){
    
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    char send_buf[MAXSIZE];
    char recv_buf[MAXSIZE];
    socklen_t len;
    int recvnum;
    int sockfd;

    if(-1==(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
        perror("create error!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if(-1==bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        perror("bind error!\n");
        return -1;
    }

    while(1){
        memset(recv_buf, 0, MAXSIZE);
        memset(send_buf, 0, MAXSIZE);
        len = sizeof(client_addr);
        
        if(-1==(recvnum = recvfrom(sockfd, recv_buf, MAXSIZE, 0, (struct sockaddr*)&client_addr, &len))){
            perror("receive error!\n");
            return -1;
        }
        printf("client:[%dB]%s\n", strlen(recv_buf), recv_buf);
        
        sprintf(send_buf, "[%dB]%s\n", strlen(recv_buf), recv_buf);
        sendto(sockfd, send_buf, MAXSIZE, 0, (struct sockaddr*)&client_addr, len); 
    }

    close(sockfd);
    return 0;
}
