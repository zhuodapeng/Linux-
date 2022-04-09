#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "list.h"
#include "menu.h"
#include "playcontrl.h"

extern Song pnow;	// pnow声明使用main.c中的pnow
extern int pthreadcls; //主线程退出标志
extern char musicbar[40];
extern int fd[2]; //无名管道

extern int inflag;		//主线程进入，暂停歌曲标志
extern int pauseflag; //暂停标志


int playsong() //点歌函数
{
	int x, flag = 0;
	char buf[100] = "loadfile ";
	Song *p, *head;
	head = read_song();
	p = head;
	strcpy(musicbar, "=");
	printf("请输入要播放歌曲的序号:\n");
	scanf("%d", &x);
	getchar();
	while (p != NULL)
	{
		if (p->songid == x)
		{
			pnow = *p;
			strcat(buf, pnow.path);
			strcat(buf, "\n");
			flag = 1;
			break;
		}
		p = p->next;
	}
	if (flag == 0)
	{
		printf("歌单内没有此序号的歌!\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	}
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	write(fp, buf, strlen(buf));
	close(fp);
	freesong(head);
	printf("正在加载歌曲...\n");
	sleep(1);
	if (pauseflag == 1)
		pauseflag = -pauseflag;
	return 0;
}

int pausesong() //暂停歌曲函数
{
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	char buf[5] = "p\n";
	write(fp, buf, strlen(buf));
	pauseflag = -pauseflag;
	close(fp);
	return 0;
}

int nextsong() //下一首歌
{
	Song *p = read_song();
	Song *head = p;
	while (p != NULL)
	{
		if (strcmp(p->path, pnow.path) == 0)
		{
			if (p->next == NULL)
			{
				pnow = *(head);
				break;
			}
			pnow = *(p->next);
			break;
		}
		p = p->next;
	}
	char buf[100] = {0};
	strcpy(buf, "loadfile ");
	strcat(buf, pnow.path);
	strcat(buf, "\n");
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	write(fp, buf, strlen(buf));
	if (pauseflag == 1)
	{
		sleep(1);
		pauseflag = -pauseflag;
	}
	close(fp);
	freesong(head);
	return 0;
}

int lastsong() //上一首歌
{
	int flag = 0;
	Song *p = read_song();
	Song *head = p;
	while (p->next != NULL)
	{
		if ((pnow.songid - 1) == 0)
		{
			break;
		}
		if (p->songid == (pnow.songid - 1))
		{
			pnow = *p;
			flag = 1;
			break;
		}
		p = p->next;
	}
	if (flag == 0)
	{
		getchar();
		printf("目前播放的已经是第一首歌\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	}
	char buf[100] = {0};
	strcat(buf, "loadfile ");
	strcat(buf, pnow.path);
	strcat(buf, "\n");

	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	write(fp, buf, strlen(buf));
	if (pauseflag == 1)
		pauseflag = -pauseflag;
	close(fp);
	freesong(head);
	return 0;
}

void exitsong() //退出mplayer,程序退出调用atexit函数
{
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	char buf[5] = "q\n";
	write(fp, buf, strlen(buf));
	close(fp);
}

int fastsong() //快进
{
	int t;
	printf("请输入想快进的秒数:\n");
	scanf("%d", &t);
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	char buf[10] = {0};
	sprintf(buf, "%s%d", "seek ", t);
	strcat(buf, "\n");
	write(fp, buf, strlen(buf));
	if (pauseflag == 1)
		pauseflag = -pauseflag;
	close(fp);
	return 0;
}

int slowsong() //快退
{
	int t;
	printf("请输入想快退的秒数:\n");
	scanf("%d", &t);
	t = -t;
	int fp = open("pipe", O_WRONLY | O_NONBLOCK);
	char buf[10] = {0};
	sprintf(buf, "%s%d", "seek ", t);
	strcat(buf, "\n");
	write(fp, buf, strlen(buf));
	if (pauseflag == 1)
		pauseflag = -pauseflag;
	close(fp);
	return 0;
}

void playcontrl() //播放控制
{
	int x, loop = 1;
	while (loop)
	{
		system("clear");
		printf("播放控制\n");
		x = contrlmenu();
		inflag = 1;
		switch (x)
		{
		case 1:
			playsong();
			break;
		case 2:
			pausesong();
			break;
		case 3:
			lastsong();
			break;
		case 4:
			nextsong();
			break;
		case 5:
			fastsong();
			break;
		case 6:
			slowsong();
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

