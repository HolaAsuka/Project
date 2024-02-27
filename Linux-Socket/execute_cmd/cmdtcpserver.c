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


int execute(char* cmd,char* buf){
	FILE *fp;
	int count;
	
	count = 0;

	if (NULL==(fp = popen(cmd,"r"))){
		perror("create pipe error\n");
		return -1;
	}

	while(((buf[count] = fgetc(fp))!=EOF)&&count<4095)
		count++;
		
	buf[count]='\0';

	pclose(fp);
	return count;
}


int main(){
	int sockfd;
	int fd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[MAXSIZE];
	char recv_buf[MAXSIZE];
	char cmd[MAXSIZE];
	int sendnum;
	int recvnum;
	int len;
	
	if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0))){
		perror("create socket error");
		return -1;
	}	

	memset(&server,0,sizeof(struct sockaddr_in));
	memset(&client,0,sizeof(struct sockaddr_in));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	if (-1==bind(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr))){
		perror("bind error\n");
		close(sockfd);
		return -1;
	}
	
	if (-1==listen(sockfd,5)){
		perror("listen error\n");
		return -1;
    }

	while(1){		
		memset(recv_buf,0,MAXSIZE);
		memset(send_buf,0,MAXSIZE);
		
		if(0>(fd=accept(sockfd,(struct sockaddr*)&client,&len))){
			perror("create connect socket error\n");
			continue;
		}

		if(-1==(recvnum = recv(fd,recv_buf,sizeof(recv_buf),0))){
			perror("recv error\n");
			continue;
		}	

		recv_buf[recvnum]='\0';

		if (0==strcmp(recv_buf,"quit")){
			perror("quit\n");
			break;

		}
		printf("receive: %s\n",recv_buf);

		strcpy(cmd,"/bin/");
		strcat(cmd,recv_buf);
		execute(cmd,send_buf);

		if ('\0'==*send_buf){
			memset(cmd,0,sizeof(cmd));
			strcpy(cmd,"/sbin/");
			strcat(cmd,recv_buf);
			execute(cmd,send_buf);
			
			if ('\0'==*send_buf){	
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/usr/bin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
			}
			
			if ('\0'==*send_buf){	
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/usr/sbin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
			}
		}
		if ('\0'==*send_buf)
			sprintf(send_buf,"cmd error\n");

		printf("reply:%s\n",send_buf);

		
		if (-1==send(fd,send_buf,sizeof(send_buf),0)){
			perror("send error\n");
			break;
		}
		close(fd);		
	}


	close(sockfd);
	return 0; 
}





