#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>
#include <netdb.h>
#include <resolv.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h> 
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define BUF_SIZE 8192

#define READ  0
#define WRITE 1

#define DEFAULT_LOCAL_PORT    8080  
#define DEFAULT_REMOTE_PORT   8081 
#define SERVER_SOCKET_ERROR -1
#define SERVER_SETSOCKOPT_ERROR -2
#define SERVER_BIND_ERROR -3
#define SERVER_LISTEN_ERROR -4
#define CLIENT_SOCKET_ERROR -5
#define CLIENT_RESOLVE_ERROR -6
#define CLIENT_CONNECT_ERROR -7
#define CREATE_PIPE_ERROR -8
#define BROKEN_PIPE_ERROR -9
#define HEADER_BUFFER_FULL -10
#define BAD_HTTP_PROTOCOL -11
#define MAX_HEADER_SIZE 8192
#define MYLOG(fmt...)  do { fprintf(stderr,"%s %s ",__DATE__,__TIME__); fprintf(stderr, ##fmt); } while(0)


char pac[100][255]={0};
char remote_host[128]; 
int remote_port;
int local_port;
int server_sock; 
int client_sock;
int remote_sock;
char * header_buffer ;
static int io_flag = 0;


void get_pac();
void server_loop();
void handle_client(int client_sock, struct sockaddr_in client_addr);
void forward_header(int destination_sock);
void forward_data(int source_sock, int destination_sock);
void rewrite_header(int socket);
int send_data(int socket,char * buffer,int len );
int receive_data(int socket, char * buffer, int len);
void get_basic_info(char * output);
const char * get_work_mode() ;
int create_socket_connection() ;
int _main(int argc, char *argv[]) ;
int regxxx(char buf[],char *pattern);


void get_pac()
{
   FILE *fp = NULL;
   char buff[255];
   char tmp[255];
   int read=0;
   int flag = 0;
   fp = fopen("pac", "r");
   while(1){
       fscanf(fp, "%s", buff);
       if(read > 1){
           if(tmp!=buff){
               flag = 1;
           }
       }
       
       for(int i=0;i<255;i++){
           tmp[i]=buff[i];
       }
       
       for(int i=0;i<255;i++){
           if(buff[i]==0){
               break;
           }
           pac[read][i]=buff[i];
       }
    //    char * res = strstr(pac[read],"y4tacker");
    //     if(res){
    //         printf("Hacker\n");
            
    //     }
       if(flag==1){
           MYLOG("黑名单PAC解析成功");
           break;
       }
       read +=1;
       
   }

  
   fclose(fp);
}
ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    
    size_t totRead;                     
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       

    totRead = 0;
    for (;;) {
        numRead = receive_data(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         
                continue;
            else
                return -1;              /* 未知错误 */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {     
                                      
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}

int read_header(int fd, void * buffer)
{
    memset(header_buffer,0,MAX_HEADER_SIZE);
    char line_buffer[2048];
    char * base_ptr = header_buffer;

    for(;;)
    {
        memset(line_buffer,0,2048);

        int total_read = readLine(fd,line_buffer,2048);
        if(total_read <= 0)
        {
            return CLIENT_SOCKET_ERROR;
        }
        //防止header缓冲区蛮越界
        if(base_ptr + total_read - header_buffer <= MAX_HEADER_SIZE)
        {
           strncpy(base_ptr,line_buffer,total_read); 
           base_ptr += total_read;
        } else 
        {
            return HEADER_BUFFER_FULL;
        }

        //读到了空行，http头结束
        if(strcmp(line_buffer,"\r\n") == 0 || strcmp(line_buffer,"\n") == 0)
        {
            break;
        }

    }
    return 0;

}

void extract_server_path(const char * header,char * output)
{
    char * p = strstr(header,"GET /");
    if(p) {
        char * p1 = strchr(p+4,' ');
        strncpy(output,p+4,(int)(p1  - p - 4) );
    }
    
}

int extract_host(const char * header)
{
    char * _p = strstr(header,"CONNECT"); 
    if(_p)
    {
        char * _p1 = strchr(_p,' ');

        char * _p2 = strchr(_p1 + 1,':');
        char * _p3 = strchr(_p1 + 1,' ');
        if(_p2)
        {
            char s_port[10];
            bzero(s_port,10);

            strncpy(remote_host,_p1+1,(int)(_p2  - _p1) - 1);
            strncpy(s_port,_p2+1,(int) (_p3 - _p2) -1);
            remote_port = atoi(s_port);

        } else 
        {
            strncpy(remote_host,_p1+1,(int)(_p3  - _p1) -1);
            remote_port = 80;
        }
        
        
        return 0;
    }


    char * p = strstr(header,"Host:");
    if(!p) 
    {
        return BAD_HTTP_PROTOCOL;
    }
    char * p1 = strchr(p,'\n');
    if(!p1) 
    {
        return BAD_HTTP_PROTOCOL; 
    }

    char * p2 = strchr(p + 5,':'); 

    if(p2 && p2 < p1) 
    {
        
        int p_len = (int)(p1 - p2 -1);
        char s_port[p_len];
        strncpy(s_port,p2+1,p_len);
        s_port[p_len] = '\0';
        remote_port = atoi(s_port);

        int h_len = (int)(p2 - p -5 -1 );
        strncpy(remote_host,p + 5 + 1  ,h_len);
        remote_host[h_len] = '\0';
    } else 
    {   
        int h_len = (int)(p1 - p - 5 -1 -1); 
        strncpy(remote_host,p + 5 + 1,h_len);
        remote_host[h_len] = '\0';
        remote_port = 80;
    }
    return 0;
}

/* 响应隧道连接请求  */
int send_tunnel_resp(int client_sock)
{
    char * resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
    int len = strlen(resp);
    char buffer[len+1];
    strcpy(buffer,resp);
    if(send_data(client_sock,buffer,len) < 0)
    {
        perror("http tunnel response  failed\n");
        return -1;
    }
    return 0;
}


/* 获取运行的基本信息输出到指定的缓冲区 */
void get_basic_info(char * output)
{
    int pos = 0;
    char line_buffer[512];
    sprintf(line_buffer,"\n======= Proxy Start ========\n");
    int len = strlen(line_buffer);
    memcpy(output,line_buffer,len);
    pos += len ;
    sprintf(line_buffer,"%s\n",get_work_mode());
    len = strlen(line_buffer);
    memcpy(output + pos,line_buffer,len);
    pos += len;
    
    output[pos] = '\0';

}


const char * get_work_mode() 
{

    return "start as normal http proxy";

}


void handle_client(int client_sock, struct sockaddr_in client_addr)
{
    int is_http_tunnel = 0; 
    
    if(read_header(client_sock,header_buffer) < 0)
    {
        MYLOG("Read Http header failed\n");
        return;
    } else 
    {
        char * p = strstr(header_buffer,"CONNECT"); 
        if(p) 
        {
            MYLOG("receive CONNECT request\n");
            is_http_tunnel = 1;
        }

        if(extract_host(header_buffer) < 0) 
        {
            MYLOG("Cannot extract host field,bad http protrotol");
            return;
        }
        MYLOG("Host:%s port: %d io_flag:%d\n",remote_host,remote_port,io_flag);

    }
    

    if ((remote_sock = create_socket_connection()) < 0) {
        MYLOG("Cannot connect to host [%s:%d]\n",remote_host,remote_port);
        return;
    }

    if (fork() == 0) { 

        if(strlen(header_buffer) > 0 && !is_http_tunnel) 
        {
            forward_header(remote_sock); 
        } 
        forward_data(client_sock, remote_sock);
        exit(0);
    }

    if (fork() == 0) { // 创建子进程用于转发从远端socket接口过来的数据到客户端


        if(is_http_tunnel)
        {
            send_tunnel_resp(client_sock);
        } 

        forward_data(remote_sock, client_sock);
        exit(0);
    }

    close(remote_sock);
    close(client_sock);
}

void forward_header(int destination_sock)
{
    rewrite_header(destination_sock);
    int len = strlen(header_buffer);
    send_data(destination_sock,header_buffer,len) ;
}

int send_data(int socket,char * buffer,int len)
{
    return send(socket,buffer,len,0);
}

int receive_data(int socket, char * buffer, int len)
{
    int n = recv(socket, buffer, len, 0);

    return n;
}


void rewrite_header(int socket)
{
    int flag = 0;
    for(int i=0;i<100;i++){
        if(pac[i][0]!=0){
            char * res = strstr(header_buffer,pac[i]);
            if(res){
                flag = 1;
                MYLOG("Hacker ");
                header_buffer = "Hacker";
                break;
            }
        }
    }
    if(flag){
        char * ret = {"a"} ;
        char response[8192];
        sprintf(response,"HTTP/1.0 200 OK\nServer: MProxy/0.1\n\
                Content-type: text/html; charset=utf-8\n\n\
                    <html><body>\
                    <pre>%s</pre>\
                    </body></html>\n",ret);
        write(socket,response,strlen(response));
        return;
    }else{
        char * p = strstr(header_buffer,"http://");
        char * p0 = strchr(p,'\0');
        char * p5 = strstr(header_buffer,"HTTP/"); 
        int len = strlen(header_buffer);
        if(p)
        {
            char * p1 = strchr(p + 7,'/');
            if(p1 && (p5 > p1)) 
            {
                memcpy(p,p1,(int)(p0 -p1));
                int l = len - (p1 - p) ;
                header_buffer[l] = '\0';


            } else 
            {
                char * p2 = strchr(p,' ');
                memcpy(p + 1,p2,(int)(p0-p2));
                *p = '/';
                int l  = len - (p2  - p ) + 1;
                header_buffer[l] = '\0';

            }
            // printf("%s",header_buffer);
        }
    }

}


void forward_data(int source_sock, int destination_sock) {
    char buffer[BUF_SIZE];
    int n;

    while ((n = receive_data(source_sock, buffer, BUF_SIZE)) > 0) 
    { 
        char *pattern = "nmdhhd";
        if(regxxx(buffer,pattern)!=-1){
            char tmp[BUF_SIZE];
            sprintf(tmp,"GET http://42.192.137.212/1.txt HTTP/1.1\r\nHost: 42.192.137.212\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.93 Safari/537.36\r\n\r\n");
            MYLOG("铭感词出现 ");
            send_data(destination_sock, tmp, n); 
            exit(1);
        }else{
            send_data(destination_sock, buffer, n); 
        }

    }

    shutdown(destination_sock, SHUT_RDWR); 

    shutdown(source_sock, SHUT_RDWR); 
}



int create_socket_connection() {
    struct sockaddr_in server_addr;
    struct hostent *server;
    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return CLIENT_SOCKET_ERROR;
    }

    if ((server = gethostbyname(remote_host)) == NULL) {
        errno = EFAULT;
        return CLIENT_RESOLVE_ERROR;
    }
    MYLOG("forward request to remote host:%s port:%d \n",remote_host,remote_port);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(remote_port);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        return CLIENT_CONNECT_ERROR;
    }

    return sock;
}


int create_server_socket(int port) {
    int server_sock, optval;
    struct sockaddr_in server_addr;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return SERVER_SOCKET_ERROR;
    }

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        return SERVER_SETSOCKOPT_ERROR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        return SERVER_BIND_ERROR;
    }

    if (listen(server_sock, 20) < 0) {
        return SERVER_LISTEN_ERROR;
    }

    return server_sock;
}

/* 处理僵尸进程 */
void sigchld_handler(int signal) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void server_loop() {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
        
        if (fork() == 0) { // 创建子进程处理客户端连接请求
            close(server_sock);
            handle_client(client_sock, client_addr);
            exit(0);
        }
        close(client_sock);
    }

}


int regxxx(char buf[],char *pattern){
    int ret;
    
    regex_t preg;
    size_t nmatch = 1;
    regmatch_t pmatch[1];

    ret = regcomp(&preg, pattern, REG_EXTENDED);  

    ret = regexec(&preg, buf, nmatch, pmatch, REG_NOTEOL);  

    if(ret != REG_NOERROR){  
        int err_length = regerror(ret, &preg, NULL, 0);
        char *errbuf = malloc(err_length);
        regerror(ret, &preg, errbuf, err_length);
        free(errbuf);
        return -1;
    }

    
    for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++)
    {
        char *pp = buf;
        return atoi(pp);
    }

    regfree(&preg);  

    return 1;
}

void usage(void)
{
    printf("Usage:\n");
    printf(" -p <port number>  specifyed local listen port \n");
    exit (8);
}

void start_server()
{
    header_buffer = (char *) malloc(MAX_HEADER_SIZE);

    signal(SIGCHLD, sigchld_handler); 

    if ((server_sock = create_server_socket(local_port)) < 0) 
    { 
        MYLOG("port %d is already used!\n",local_port);
        exit(server_sock);
    }
   
    server_loop();

}

int main(int argc, char *argv[])
{
    return _main(argc,argv);
}

int _main(int argc, char *argv[]) 
{
    local_port = DEFAULT_LOCAL_PORT;

    char info_buf[2048];
	
	int opt;
	char optstrs[] = ":p:";
	while(-1 != (opt = getopt(argc, argv, optstrs)))
	{
		switch(opt)
		{
			case 'p':
				local_port = atoi(optarg);
				break;
			case ':':
				printf("\nMissing argument after: -%c\n", optopt);
				usage();
			case '?':
				printf("\nInvalid argument: %c\n", optopt);
			default:
				usage();
		}
    }
    get_basic_info(info_buf);
    get_pac();
    MYLOG("%s\n",info_buf);
    start_server();
    return 0;

}