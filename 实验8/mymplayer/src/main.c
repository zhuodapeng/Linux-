#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include<semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "menu.h"
#include "list.h"
#include "playcontrl.h"
#include "admin.h"

#define PORT 4321
#define IPADDR "127.0.0.1"

Song pnow;//记录当前歌曲
int fd[2];
int sockfd;
struct sockaddr_in server;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int userflag = 0;//用来区分是管理员登录还是普通用户登录
int pthreadcls = 0; //主线程退出标志
int issend = 0;		//发送数据标志
char musicbar[40] = "=";
char musicname[20] = "musicname";
int bar = 0; //进度条长度
char musiclength[20] = "0";
int inflag = 0; //主线程进入，暂停歌曲标志
int pauseflag = -1; //暂停标志

void *getcmd_thread(void *arg);
void *sendfd_thread(void *arg);
void *readfd_thread(void *arg);
void deal_data(char *buf);
int server_connect();

int main()
{
	pthread_t tid1,tid2, tid3;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	if(access("pipe",F_OK) != 0)
	{
		mkfifo("pipe",0666);
	}

	if(pipe(fd) < 0)
	{
		perror("pipe error\n");
		return 0;
	}
	fcntl(fd[0], F_SETFL, O_NONBLOCK); //无名管道设为非阻塞

	pid_t pid = vfork();
	if(pid < 0)
	{
		perror("fork fail\n");
		return 1;
	}
	else if(pid == 0)
	{
		close(fd[0]);
		dup2(fd[1],1);
		execl("/usr/bin/mplayer", "mplayer", "-quiet", "-slave", "-idle", "-nolirc", "-input", "file=pipe", NULL, NULL);
	}
	else
	{
		atexit(exitsong);
		close(fd[1]);
		sleep(1);
		pthread_create(&tid1,NULL,getcmd_thread,NULL);
		pthread_create(&tid2,NULL,sendfd_thread,NULL);
		pthread_create(&tid3,NULL,readfd_thread,NULL);

		int a = pthread_join(tid1,NULL);
		if(a == 0)
		{
			printf("欢迎下次使用.\n");
		}
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);

	}
	return 0;
}

void *getcmd_thread(void *arg) //线程1：给有名管道发送命令（实现暂停等功能）
{
	int x, loop = 1;
	while (loop)
	{
		system("clear");
		x = mainmenu();
		getchar();
		switch (x)
		{
		case 1:
			songsheet(); //歌单
			break;
		case 2:
			playcontrl();
			break;
		case 3:
			admin_login();
			break;
		case 0:
			pthreadcls = 1;
			loop = 0;
			break;
		default:
			pthreadcls = 1;
			loop = 0;
			break;
		}
	}
	return NULL;
}

// get_file_name\nget_time_length\n
void *sendfd_thread(void *arg) //线程2：给有名管道发送命令，让mplayer返回信息
{
	pthread_detach(pthread_self());
	int fd = 0;
	char str[50] = "get_time_pos\nget_percent_pos\n";
	char str1[50] = "get_file_name\nget_time_length\n";
	while (inflag == 0)
	{
		sleep(1);
	}
	while (1)
	{
		pthread_mutex_lock(&mutex);
		while (pauseflag == 1 || pthreadcls == 1)
		{
			if (pthreadcls == 1)
			{
				pthread_mutex_unlock(&mutex);
				issend = 1;
				return NULL;
			}
			sleep(1);
		}
		fd = open("pipe", O_WRONLY | O_NONBLOCK);
		if (fd < 0)
		{
			perror("open fifo error\n");
			printf("按任意键继续...\n");
			getchar();
			return 0;
		}
		write(fd, str1, strlen(str1));
		write(fd, str, strlen(str));
		close(fd);
		issend = 1;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
		sleep(1);
	}
	pthread_exit(0);
	return NULL;
}

void *readfd_thread(void *arg) //读取mplayer发回的信息，保存到buf
{
	pthread_detach(pthread_self());
	while (inflag == 0)
	{
		sleep(1);
	}
	int ret;
	char buf[1024] = {0};
	while (1)
	{
		pthread_mutex_lock(&mutex);
		while (issend == 0 && pthreadcls != 1)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		while (pauseflag == 1 || pthreadcls == 1)
		{
			if (pthreadcls == 1)
			{
				issend = 0;
				pthread_mutex_unlock(&mutex);
				return NULL;
			}
			sleep(1);
		}

		ret = read(fd[0], buf, sizeof(buf));

		if (ret > 0)
		{
			buf[1024] = '\0';
			deal_data(buf);
			memset(buf, 0, strlen(buf));
		}
		issend = 0;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(0);
	return NULL;
}
// ANS_LENGTH=199.00 ANS_TIME_POSITION=22.0 ANS_FILENAME='AllFallsDown.mp3' ANS_PERCENT_POSITION=1
void deal_data(char *buf) //解析buf中的信息，拿到要用的
{
	char msg_buf[110] = {0};
	char *p = NULL;
	char *ps = NULL;
	strncpy(msg_buf, buf, 100);
	int b;
	p = strtok(msg_buf, "\n");
	while (p != NULL)
	{
		ps = NULL;
		if ((ps = strstr(p, "ANS_PERCENT_POSITION=")) != NULL)
		{
			ps += strlen("ANS_PERCENT_POSITION=");
			if (atoi(ps) != bar && atoi(ps) % 3 == 0)
			{
				if (atoi(ps) - bar > 3) //快进操作处理进度条
				{
					strcpy(musicbar, "");
					for (b = 0; b < atoi(ps) / 3; b++)
						strcat(musicbar, "=");
				}
				else if (bar - atoi(ps) > 3) //快退操作处理进度条
				{
					strcpy(musicbar, "");
					for (b = 0; b < atoi(ps) / 3; b++)
						strcat(musicbar, "=");
				}
				else
				{
					strcat(musicbar, "=");
				}
				bar = atoi(ps);
			}
			if (strcmp(ps, "98") == 0)
			{
				printf("\n歌曲即将结束,自动播放下一首歌...\n");
				strcpy(musicbar, "=");
				sleep(2);
				nextsong();
			}
		}
		else if ((ps = strstr(p, "ANS_TIME_POSITION=")) != NULL)
		{
			ps += strlen("ANS_TIME_POSITION=");
			printf("\33[2K\r");
			printf("%s  [%-35s] %s/%s", musicname, musicbar, ps, musiclength);
			fflush(stdout);
		}
		else if ((ps = strstr(p, "ANS_FILENAME=")) != NULL)
		{
			ps += strlen("ANS_FILENAME=");
			strcpy(musicname, ps);
		}
		else if ((ps = strstr(p, "ANS_LENGTH=")) != NULL)
		{
			ps += strlen("ANS_LENGTH=");
			strcpy(musiclength, ps);
		}
		p = strtok(NULL, "\n");
	}
	memset(msg_buf, 0, sizeof(msg_buf));
}

int server_connect()
{
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() error\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr(IPADDR);
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		getchar();
		printf("connect() error\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	};
	printf("连接服务器成功!\n");
	return 1;
}
