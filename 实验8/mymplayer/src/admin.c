#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "admin.h"
#include "list.h"
#include "menu.h"

extern int sockfd;
extern struct sockaddr_in server;
extern int server_connect();

int file_size(char *filename) //获取文件名为filename的文件大小。
{
    struct stat statbuf;
    int ret;
    ret = stat(filename, &statbuf); //调用stat函数
    if (ret != 0)
        return -1;          //获取失败。
    return statbuf.st_size; //返回文件大小。
}

void server_displaysong()
{
    char data[256];
    int size, tsize, ndata;
    tsize = 0;
    read(sockfd, &size, sizeof(size));
    while ( (ndata = read(sockfd, data, sizeof(data))) > 0)
    {
        printf("%s", data);
        tsize += ndata;
        if(tsize >= size)
            break;
    }
    getchar();
    printf("歌单查询成功!\n");
    printf("按任意键继续...\n");
    getchar();
}

void server_uploadsong()
{
    char songname[50],path[200],buff[1024];
    char f = 'y';
    printf("请输入要上传的歌曲名称:\n");
    scanf("%s", songname);
    printf("请输入歌曲绝对路径:\n");
    scanf("%s", path);
    getchar();
    if (access(path, F_OK) != 0) //判断文件是否存在，存在返回0
    {
        f = 'n';
        write(sockfd, &f, sizeof(f));
        printf("输入文件不存在,上传失败!\n");
        printf("按任意键继续...\n");
        getchar();
        return;
    }
    else if (strcmp(path + strlen(path) - 3, "mp3") != 0) //判断是否为.mp3
    {
        f = 'n';
        write(sockfd, &f, sizeof(f));
        printf("输入文件不是MP3文件!\n");
        printf("按任意键继续...\n");
        getchar();
        return;
    }
    write(sockfd, &f, sizeof(f));
    int songsize;
    songsize = file_size(path);
    write(sockfd, songname, sizeof(songname));
    write(sockfd, &songsize, sizeof(songsize));
    usleep(10);
    int fd = open(path, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    if (fd == -1)
    {
        printf("文件打开失败!\n");
        printf("按任意键继续...\n");
        getchar();
        return;
    }
    int ndata;
    while ((ndata = read(fd, buff, sizeof(buff))) > 0)
    {
        write(sockfd, buff, ndata);
    }
    printf("歌曲 %s 上传成功!\n", songname);
    printf("按任意键继续...\n");
    getchar();
}

void server_deletesong()
{
    char songname[50];
    printf("请输入要删除的歌曲名称:\n");
    scanf("%s", songname);
    write(sockfd, songname, sizeof(songname));
    usleep(10);
    int f;
    read(sockfd, &f, sizeof(f));
    if(f == 0)
    {
        printf("歌曲%s不存在!\n",songname);
    }
    else if(f == 1)
    {
        printf("歌曲%s删除成功!\n",songname);
    }
    getchar();
    printf("按任意键继续...\n");
    getchar();
    return;
}

void server_manage()
{
    int x, loop = 1;
    while (loop)
    {
        system("clear");
        printf("服务器管理\n");
        x = adminmenu();
        write(sockfd, &x, sizeof(x));
        switch (x)
        {
        case 1:
            server_displaysong();
            break;
        case 2:
            server_uploadsong();
            break;
        case 3:
            server_deletesong();
            break;
        case 0:
            loop = 0;
            break;
        default:
            loop = 0;
            break;
        }
    }
}

void admin_login()
{
    if(server_connect() == 0)
        return;
    char who[10] = "admin";
    write(sockfd, who, sizeof(who));
    usleep(10);
    char adminname[20];
    char adminpasswd[20];
    int logflag = 0;
    while (logflag == 0)
    {
        memset(adminname, 0, sizeof(adminname));
        memset(adminpasswd, 0, sizeof(adminpasswd));
        printf("请输入管理员账号:\n");
        scanf("%s", adminname);
        getchar();
        write(sockfd, adminname, sizeof(adminname));
        usleep(10);
        printf("请输入管理员密码:\n");
        scanf("%s", adminpasswd);
        getchar();
        write(sockfd, adminpasswd, sizeof(adminpasswd));
        usleep(10);
        char islogin;
        read(sockfd, &islogin, sizeof(islogin));
        if (islogin == 'y')
        {
            logflag = 1;
        }
        else if(islogin == 'n')
        {
            printf("账号或密码错误,是否重试(y/n):\n");
            char choice;
            scanf("%c", &choice);
            if (choice == 'y' || choice == 'Y')
            {
                write(sockfd, &choice, sizeof(choice));
                continue;
            }
            else if (choice == 'n' || choice == 'N')
            {
                write(sockfd, &choice, sizeof(choice));
                return;
            }
        }
    }
    server_manage();
}