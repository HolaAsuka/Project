#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>

#define PORT 8900
#define MAXSIZE 4096

void print_usage(char* str){
	fprintf(stderr," %s usage:\n",str);
	fprintf(stderr,"%s IP_Addr",str);
}


int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[MAXSIZE];
	char recv_buf[MAXSIZE];
	int sendnum;
	int recvnum;
	int len;
	int input_len;

	if(2!=argc){
		print_usage(argv[0]);
		return -1;	

	}

	memset(&server,0,sizeof(struct sockaddr_in));
	memset(&client,0,sizeof(struct sockaddr_in));
	
	
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(PORT);	
	
 	len = sizeof(struct sockaddr);	
 	

	
	while(1){
		memset(send_buf,0,MAXSIZE);
		memset(recv_buf,0,MAXSIZE);
		
		if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0))){
			perror("create socket error\n");
			return -1;
		}	
		
		if (-1==(connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))){
			perror("connect error");
			close(sockfd);
			break;
		}
				
		printf("tcp>");
		fgets(send_buf,MAXSIZE,stdin);
		input_len=strlen(send_buf);
		send_buf[input_len-1]='\0';
		
		if (-1==send(sockfd,send_buf,sizeof(send_buf),0)){
			perror("send error!\n");
			continue;
		}
		
		if (0==strcmp(send_buf,"quit")){
			perror("quit\n");
			continue;
		}
		
		if (-1==recv(sockfd,recv_buf,sizeof(recv_buf),0)){
			perror("recv error!");
			continue;
		}

		fprintf(stdout,"%s\n",recv_buf);
		close(sockfd);

	}
	close(sockfd);
	return 0;
}




