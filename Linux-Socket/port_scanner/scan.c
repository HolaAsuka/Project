#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void print_usage(char* str) 
{
	printf("the command %s usage is:\n",str);
	printf("%s Ip_Address [port] \n");
}
int scanport(int sockfd, struct sockaddr_in scanip, int port, char** arg)
{
	if(-1==(sockfd=socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("generate socket error\n");
		exit(1);
	}
	memset(&scanip, 0, sizeof(struct sockaddr));
	scanip.sin_family = AF_INET;
	scanip.sin_addr.s_addr = inet_addr(arg[1]);
	scanip.sin_port = htons(port);
	int ret = connect(sockfd, (struct sockaddr*)&scanip, sizeof(struct sockaddr));
	close(sockfd);
	return ret;
}
int main(int argc, char**argv)
{
	int sockfd;
	struct servent* sp;
	int start_port = atoi(argv[2]);
	int end_port = atoi(argv[3]);
	struct sockaddr_in scanip;
	if(4!=argc)
	{
		print_usage(argv[0]);
		exit(1);
	}
	for(start_port;start_port<=end_port;start_port++)
	{
		if(!scanport(sockfd,scanip,start_port,argv))
		{
			if(-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
			{
				perror("generate socket error\n");
				exit(1);
			}
			connect(sockfd, (struct sockaddr*)&scanip, sizeof(struct sockaddr));
			sp=getservbyport(htons(start_port),"tcp");
            if(NULL!=sp)
			{
				printf("%d %s\n",start_port,sp->s_name);
			}
			else
			{
				printf("%d unknow \n",start_port);
			}
			close(sockfd);
		}
	}
	exit(0);
}

