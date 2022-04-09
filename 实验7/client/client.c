//客户端代码
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>  /* netdb is necessary for struct hostent */

#define PORT 4321  /* server port */
#define MAXDATASIZE 10240
#define DOWNLOAD "./download"
char *download="./download/";
int main()
{
    int sockfd;    /* files descriptors */
    struct sockaddr_in server;

    if((sockfd=socket(AF_INET,SOCK_STREAM, 0))==-1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;//指代协议族，在socket编程中只能是AF_INET
    server.sin_port = htons(PORT);//存储端口号（使用网络字节顺序），在linux下，端口号的范围0~65535,
    server.sin_addr.s_addr = inet_addr("127.0.0.1");//存储IP地址，使用in_addr这个数据结构
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server))==-1)//connect用于建立与指定外部端口socket的连接
    {
        printf("connect() error\n");
        exit(1);
    };
    printf("连接服务器成功！\n");
    char buf[1024];
    char data[MAXDATASIZE];
    char filesname[10][1024];
    int filecount;
    int i, ndata, filesize;
    char temp[30];
    while(1)
    {
        memset(filesname, 0, sizeof(filesname));
        filecount = 0;
        printf("请输入您要查询的关键词:");
        scanf("%s", buf);
        write(sockfd, buf, strlen(buf));
        usleep(10);
        read(sockfd, &filecount, sizeof(filecount));
        long int filessize[filecount];
        memset(filessize, 0, sizeof(filessize));
        read(sockfd, filesname, sizeof(filesname));
        read(sockfd, filessize, sizeof(filessize));
        if(filecount > 0)
        {
            printf("查询出%d个文件!\n", filecount);
        }
        else
            printf("未查询出相应文件！\n",i);
        for (i = 0; i < filecount; i++)
        {
            memset(temp, 0, sizeof(temp));
            filesize = 0;
            strcpy(temp, DOWNLOAD);
            strcat(temp, filesname[i]);
            int fd = open(temp, O_RDWR | O_CREAT, 0666);
            if (fd == -1)
            {
                printf("can not open the file\n");
                printf("press any key to continue.\n");
                getchar();
                return;
            }
            ftruncate(fd, 0);
            while ((ndata = read(sockfd, data, sizeof(data))) > 0)
            {
                write(fd, data, ndata);
                filesize += ndata;
                if (filesize == filessize[i])
                    break;
            }
            printf("文件%s下载成功.", filesname[i]);
        }
        if(i!=0)
            printf("%d个文件下载成功！\n",i);
    }
    close(sockfd);
    return 0;
}

