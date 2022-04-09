//服务端代码
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 4321
#define BACKLOG 5
#define MAXDATASIZE 1024*1024

int file_size(char* filename)//获取文件名为filename的文件大小。
{

    struct stat statbuf;

    int ret;

    ret = stat(filename,&statbuf);//调用stat函数

    if(ret != 0) return -1;//获取失败。

    return statbuf.st_size;//返回文件大小。

}

void shellcmd(char* cmd,int *filecount,char filesname[][1024])
{
    char temp[256];
    FILE* fp = NULL;
    fp = popen(cmd, "r");
    int len;
    if(fp)
    {
        while(fgets(temp, sizeof(temp), fp) != NULL)
        {
            len=strlen(temp);
            strncpy(filesname[*filecount],temp,len-1);
            (*filecount)++;
            memset(temp,0,sizeof(temp));
        }
        pclose(fp);
    }
}
int sendback(int client_socket,int *filecount,char filesname[][1024])
{
    char tfilesname[10][1024];
    long int filessize[*filecount];
    char buff[MAXDATASIZE];
    int i;
    for(i=0; i<(*filecount); i++)
    {
        strncpy(tfilesname[i],filesname[i]+6,strlen(filesname[i])-6);
        filessize[i]=file_size(filesname[i]);
    }
    write(client_socket, filecount, sizeof(*filecount));
    write(client_socket, tfilesname, sizeof(tfilesname));
    write(client_socket, filessize, sizeof(filessize));
    usleep(20);
    int ndata=0;
    for(i=0; i<(*filecount); i++)
    {
        int fd = open(filesname[i], O_RDWR);

        lseek(fd,0,SEEK_SET);
        if(fd==-1)
        {
            printf("can not open the file\n");
            return -1;
        }
        while((ndata = read(fd, buff, sizeof(buff)))>0)
        {
            write(client_socket, buff, ndata);
        }
        printf("文件%s传输完毕.\n",filesname[i]);
	usleep(100);//等待客户端处理数据
    }
    return *filecount;
}
static void* query(void* arg)
{
    pthread_detach(pthread_self());
    printf("客户接入...\n");
    int filecount;
    char filesname[10][1024];
    int client_socket = *(int *)arg;
    char objstr[1024];
    while(1)
    {
        filecount=0;
        int len = read(client_socket, objstr, sizeof(objstr));
        if(len == -1)
        {
            perror("read error");
            pthread_exit(NULL);
        }
        else if(len == 0)
        {
            printf("客户断开\n");
            close(client_socket);
            break;
        }
        else
        {
            char cmd1[20]="grep -lr ";
            char cmd3[10]=" ./res";
            strcat(cmd1,objstr);
            strcat(cmd1,cmd3);
            shellcmd(cmd1,&filecount,filesname);
            if(filecount==0)
            {
                printf("客户未查询到相关文件！\n");
            }
            if(sendback(client_socket,&filecount,filesname))
            {
                printf("客户查询并下载了以下文件:\n");
                int i;
                for(i=0; i<filecount; i++)
                    printf("%s\n",filesname[i]);
            }
            printf("等待客户操作:\n");
        }
        memset(objstr,0,sizeof(objstr));
    }
    pthread_exit(0) ;
}


int main(int argc, char *argv[])
{
    int server_sockfd,client_socket;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t addrlen;
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() error. Failed to initiate a socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrlen = sizeof(server);

    if(bind(server_sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Bind() error.");
        exit(1);
    }

    if(listen(server_sockfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }
    printf("服务器启动成功！\n");
    while(1)
    {
        pthread_t thread_id;
        if((client_socket=accept(server_sockfd,(struct sockaddr *)&client, &addrlen))==-1)
        {
            perror("accept() error. \n");
            exit(1);
        }
        pthread_create(&thread_id, NULL, query, &client_socket);
    }
    close(server_sockfd);
    return 0;
}

