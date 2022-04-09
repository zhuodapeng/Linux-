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
#include <sqlite3.h>

#define PORT 4321
#define BACKLOG 5
#define MAXDATASIZE 1024*1024
#define USER "user"
#define ADMIN "admin"
#define RES "./res/"

char status[10] = "idle";
sqlite3 *db = NULL;
int rc, row, col;
char *Errormsg;
char **Result;

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
            char cmd1[20]="find ./res -name *";
            char cmd3[10]="*";
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

void server_displaysong(int client_socket)
{
    int i = 0;
    int j = 0;
    rc = sqlite3_open("song.db", &db);
    if (rc)
    {
        fprintf(stderr, "can't open:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    else
    {
        printf("open song.db success\n");
    }
    //char *sql = "create table song(ID integer primary key, songname varchar(30),songpath varchar(50))";
    char *sql = "select * from song";
    sqlite3_exec(db, sql, 0, 0, &Errormsg);
    rc = sqlite3_get_table(db, sql, &Result, &row, &col, &Errormsg);
    char data[2048];
    memset(data, 0, sizeof(data));
    for (i = 0; i < row + 1; i++)
    {
        for (j = 0; j < col; j++)
        {
            if(j > 0)
                strcat(data, " ");
            strcat(data, Result[j + i * col]);
        }
        strcat(data, "\n");
    }
    printf("%s", data);
    int size = strlen(data);
    write(client_socket, &size, sizeof(size));
    write(client_socket, data, strlen(data));
    printf("管理员查询了歌单.\n");
    sqlite3_free(Errormsg);
    sqlite3_free_table(Result);
    sqlite3_close(db);
    db = NULL;
    printf("close song.db\n");
    return;
}

void server_uploadsong(int client_socket)
{
    char f;
    read(client_socket, &f, sizeof(f));
    if(f == 'n')
    {
        printf("管理员上传音乐格式不符.\n");
        printf("上传终止.\n");
        return;
    }
    char songname[50], data[1024], songpath[70];
    memset(songpath, 0, sizeof(songpath));
    strcat(songpath, RES);
    int ndata;
    int hasrev = 0;
    int songsize = 0;
    read(client_socket, songname, sizeof(songname));
    read(client_socket, &songsize, sizeof(songsize));
    strcat(songpath, songname);
    printf("%s\n", songpath);
    int fd = open(songpath, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        printf("歌曲路径不存在.\n");
        return;
    }
    ftruncate(fd, 0);
    while ((ndata = read(client_socket, data, sizeof(data))) > 0)
    {
        write(fd, data, ndata);
        hasrev += ndata;
        if(hasrev == songsize)
            break;
    }
    usleep(10);
    rc = sqlite3_open("song.db", &db);
    if (rc)
    {
        fprintf(stderr, "can't open:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    else
    {
        printf("open song.db success\n");
    }
    char sqld[100] = "insert into song values(NULL,'";
    strcat(sqld, songname);
    strcat(sqld, "','");
    strcat(sqld, songpath);
    strcat(sqld, "')");
    sqlite3_exec(db, sqld, 0, 0, &Errormsg);
    sqlite3_free(Errormsg);
    sqlite3_close(db);
    db = NULL;
    printf("close song.db\n");
    printf("管理员上传了歌曲 %s.\n",songname);
}

void server_deletesong(int client_socket)
{

    char songname[50], path[100];
    read(client_socket, songname, sizeof(songname));
    strcpy(path, RES);
    strcat(path, songname);
    int f = 0;
    if (access(path, F_OK) != 0) //判断文件是否存在，存在返回0
    {
        write(client_socket, &f, sizeof(f));
        printf("需要删除的歌曲不存在.\n");
        printf("删除终止.\n");
        return;
    }
    f = 1;
    write(client_socket, &f, sizeof(f));
    char cmd[100];
    strcpy(cmd, "rm -f ");
    strcat(cmd, path);
    system(cmd);

    rc = sqlite3_open("song.db", &db);
    if (rc)
    {
        fprintf(stderr, "can't open:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    else
    {
        printf("open song.db success\n");
    }
    char sqld[100] = "delete from song where songname='";
    strcat(sqld, songname);
    strcat(sqld, "'");
    sqlite3_exec(db, sqld, 0, 0, &Errormsg);
    sqlite3_free(Errormsg);
    sqlite3_close(db);
    db = NULL;
    printf("close song.db\n");
    printf("管理员删除了歌曲 %s\n", songname);
}

static void *server_manage(void *arg)
{
    pthread_detach(pthread_self());
    printf("管理员已登录.\n");
    int x, loop;
    loop = 1;
    int client_socket = *(int *)arg;
    while (loop)
    {
        x = 0;
        read(client_socket, &x, sizeof(x));
        switch (x)
        {
        case 1:
            server_displaysong(client_socket);
            break;
        case 2:
            server_uploadsong(client_socket);
            break;
        case 3:
            server_deletesong(client_socket);
            break;
        case 0:
            loop = 0;
            break;
        default:
            loop = 0;
            break;
        }
    }
    strcpy(status, "idle");
    close(client_socket);
    printf("管理员退出.\n");
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    int server_sockfd,client_socket;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t addrlen;
    char who[10];
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
    char adminname[20];
    char adminpasswd[20];
    while(1)
    {
        if((client_socket=accept(server_sockfd,(struct sockaddr *)&client, &addrlen))==-1)
        {
            perror("accept() error. \n");
            exit(1);
        }
        memset(who, 0, sizeof(who));
        read(client_socket, who, sizeof(who)); //判断访问者的身份
        usleep(10);

        if(strcmp(who,ADMIN) == 0 && strcmp(status,ADMIN) != 0)
        {
            strcpy(status,ADMIN);
            printf("管理员正在登录...\n");
            int logsucccess = 0;
            char sqld[100];
            rc = sqlite3_open("song.db", &db);
            if (rc)
            {
                fprintf(stderr, "can't open:%s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            else
            {
                printf("open song.db success\n");
            }
            while (1)
            {
                memset(adminname,0,sizeof(adminname));
                memset(adminpasswd,0,sizeof(adminpasswd));
                memset(sqld, 0, sizeof(sqld));
                read(client_socket, adminname, sizeof(adminname));
                printf("接收的登录账号: %s\n",adminname);
                read(client_socket, adminpasswd, sizeof(adminpasswd));
                printf("接收的登录密码: %s\n",adminpasswd);
                char islogin;
                strcpy(sqld, "select adminpasswd from admin where adminname='");
                strcat(sqld, adminname);
                strcat(sqld, "'");
                sqlite3_exec(db, sqld, 0, 0, &Errormsg);
                rc = sqlite3_get_table(db, sqld, &Result, &row, NULL, &Errormsg);
                if (row > 0 && strcmp(adminpasswd, Result[1]) == 0)
                {
                    islogin = 'y';
                    write(client_socket, &islogin, sizeof(islogin));
                    logsucccess = 1;
                    usleep(10);
                    break;
                }
                else
                {
                    islogin = 'n';
                    write(client_socket, &islogin, sizeof(islogin));
                    usleep(10);
                    char choice;
                    read(client_socket, &choice, sizeof(choice));
                    if(choice == 'y'||choice == 'Y')
                        continue;
                    else if(choice == 'n'||choice == 'N')
                    {
                        printf("管理员取消登录.\n");
                        break;
                    }
                }
            }
            if(logsucccess)
            {
                pthread_t thread_id;
                pthread_create(&thread_id, NULL, server_manage, &client_socket);
            }
            else
            {
                strcpy(status, "idle");
                close(client_socket);
            }
            sqlite3_free(Errormsg);
            sqlite3_free_table(Result);
            sqlite3_close(db);
            db = NULL;
            printf("close song.db\n");
            continue;
        }
        if(strcmp(status,ADMIN) == 0)
        {
            write(client_socket, ADMIN, sizeof(ADMIN));
            usleep(10);
        }
        else
        {
            write(client_socket, USER, sizeof(USER));
            usleep(10);
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, query, &client_socket);
        }
    }
    close(server_sockfd);
    return 0;
}

