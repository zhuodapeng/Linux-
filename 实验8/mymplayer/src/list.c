#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "list.h"
#include "menu.h"

#define SONG_FILE "song.txt"
#define MAXDATASIZE 1024 * 1024
#define DOWNLOAD "../download/music/"
extern int sockfd;
extern struct sockaddr_in server;
extern int server_connect();
int id = 0;
char rltpath[100] = "../download/music/";

void freesong(Song *head)
{
	Song *p;
	p = head;
	while (head->next)
	{
		head = head->next;
		free(p);
		p = head;
	}
	if(head)
		free(head);
}

int write_song(Song *p)
{
	FILE *fp;
	fp = fopen("song.txt", "a");
	if (fp == NULL)
	{
		perror("open fail\n");
		return 1;
	}
	fprintf(fp, "%-4d", p->songid);
	fprintf(fp, "%-100s\n", p->path);
	fclose(fp);
	return 0;
}

Song *read_song() //从存歌的文件中读取歌的信息，形成链表，返回链头
{
	Song *head;
	Song *p, *pold, *pnew;
	FILE *fp;
	fp = fopen(SONG_FILE, "r");
	if (fp == NULL)
	{
		return NULL;
	}
	head = (Song *)malloc(sizeof(Song));
	memset(head, 0, sizeof(Song));
	head->next = NULL;
	p = head;
	while (!feof(fp))
	{
		fscanf(fp, "%d", &p->songid);
		fscanf(fp, "%s", p->path);
		pnew = (Song *)malloc(sizeof(Song));
		memset(pnew, 0, sizeof(Song));
		pnew->next = NULL;
		pold = p;
		pold->next = pnew;
		p = pnew;
	}
	pold->next = NULL;
	free(p);
	fclose(fp);
	return head;
}

int exist_song(char *buf) //判断一首歌是否重复添加
{
	Song *p,*head;
	head = read_song();
	p = head;
	while (p != NULL)
	{
		if (strcmp(p->path, buf) == 0)
		{
			freesong(head);
			return 1;
		}
		p = p->next;
	}
	freesong(head);
	return 0;
}

int display_song()
{
	Song *p,*head;
	head = read_song();
	p = head;
	if (p == NULL)
	{
		getchar();
		printf("文件中还没有歌曲!\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	}
	printf("**********************\n");
	while (p->next != NULL)
	{
		printf("%04d,%s\n", p->songid, p->path);
		p = p->next;
	}
	printf("**********************\n");
	freesong(head);
	printf("按任意键继续...\n");
	getchar();
	return 0;
}

int add_song() //添加歌曲
{
	Song *p;
	char buf[100];

	p = (Song *)malloc(sizeof(Song));
	if (p == NULL)
	{
		perror("malloc fail\n");
		return 1;
	}
	memset(p, 0, sizeof(Song));
	Song *head = read_song();
	if (head == NULL)
	{
		id = 0;
	}
	else
	{
		while (head->next != NULL)
		{
			id = head->songid;
			head = head->next;
		}
	}

	id++;
	p->songid = id;
	printf("请输入歌曲绝对路径:\n");
	scanf("%s", buf);

	if (access(buf, F_OK) != 0) //判断文件是否存在，存在返回0
	{
		getchar();
		printf("输入文件不存在,添加失败!\n");
		getchar();
		return 0;
	}
	else if (exist_song(buf) == 1) //判断一首歌是否重复添加
	{
		getchar();
		printf("此歌曲已存在!\n");
		getchar();
		return 0;
	}
	else if (strcmp(buf + strlen(buf) - 3, "mp3") != 0) //判断是否为.mp3
	{
		getchar();
		printf("输入文件不是MP3文件!\n");
		getchar();
		return 0;
	}

	strcpy(p->path, buf);
	p->next = NULL;

	write_song(p);
	free(p);
	freesong(head);
	getchar();
	printf("歌曲添加成功!\n");
	printf("按任意键继续...\n");
	getchar();
	return 0;
}

int delete_song()
{
	int t; //确定删除哪首歌，输入其id
	int flag = 0;
	FILE *fp;
	Song *head;
	Song *p, *pold;
	head = read_song();
	p = head;
	pold = p;
	char cmd[100] = "rm -f ";
	printf("请输入您要删除的歌曲编号:\n");
	scanf("%d", &t);

	if (head == NULL)
	{
		getchar();
		printf("歌曲文件夹为空,请先添加音乐!\n");
		return 0;
	}
	if (head->songid == t)
	{
		head = head->next;
		free(p);
		flag = 1;
	}
	else
	{
		while (p != NULL)
		{
			if (p->songid == t)
			{
				pold->next = p->next;
				strcat(cmd,p->path);
				free(p);
				flag = 1;
				break;
			}
			else
			{
				pold = p;
				p = p->next;
			}
		}
	}
	if (flag == 0)
	{
		getchar();
		printf("歌曲编号不存在!\n");
		printf("按任意键继续...\n");
		getchar();
		return 0;
	}

	fp = fopen(SONG_FILE, "w");
	if (fp == NULL)
	{
		perror("open file fail\n");
		return 1;
	}
	id = 0;
	while (head->next != NULL) //重新写入一定要加->next,不然会多出一行0
	{
		id++;
		head->songid = id;
		fprintf(fp, "%-4d", head->songid);
		fprintf(fp, "%-100s\n", head->path);
		head = head->next;
	}
	system(cmd);
	fclose(fp);
	freesong(head);
	getchar();
	printf("歌曲删除成功!\n");
	printf("按任意键继续...\n");
	getchar();
	return 0;
}

void download_song()
{
	if(server_connect() == 0)
		return;
	char who[10] = "user";
	write(sockfd, who, strlen(who));
	usleep(10);
	char status[10];
	read(sockfd, status, sizeof(status));
	if(strcmp(status,"user") != 0)
	{
		close(sockfd);
		getchar();
		printf("服务器正在更新数据,请稍后连接.\n");
		printf("按任意键继续...\n");
		getchar();
		return;
	}
	char buf[1024];
	char data[MAXDATASIZE];
	printf("请输入您要下载的歌曲:");
	scanf("%s", buf);
	getchar();
	write(sockfd, buf, strlen(buf));
	char filesname[10][1024];
	int filecount;
	read(sockfd, &filecount, sizeof(filecount));
	printf("查询出%d个文件!\n", filecount);
	long int filessize[filecount];
	read(sockfd, filesname, sizeof(filesname));
	read(sockfd, filessize, sizeof(filessize));
	int i, ndata, filesize;
	char temp[30];
	for (i = 0; i < filecount; i++)
	{
		memset(temp, 0, sizeof(temp));
		filesize = 0;
		strcpy(temp, DOWNLOAD);
		strcat(temp, filesname[i]);
		int fd = open(temp, O_RDWR | O_CREAT, 0666);
		if (fd == -1)
		{
			getchar();
			printf("文件打开失败!\n");
			printf("按任意键继续...\n");
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
		printf("歌曲%s下载成功!\n", filesname[i]);
	}
	if (i != 0)
	{
		printf("%d个歌曲下载成功!\n", i);
		Song *head = read_song(); //将歌曲信息写入文件
		if (head == NULL)
		{
			id = 0;
		}
		else
		{
			while (head->next != NULL)
			{
				id = head->songid;
				head = head->next;
			}
		}
		id++;
		Song *p = (Song*)malloc(sizeof(Song));
		int j;
		char trltpath[100];
		for (j = 0; j < i; j++)
		{
			strcpy(trltpath, rltpath);
			p->songid = id++;
			strcat(trltpath, filesname[j]);
			strcpy(p->path, trltpath);
			write_song(p);
			memset(trltpath,0,sizeof(trltpath));
			memset(p, 0, sizeof(Song));
		}
		free(p);
		freesong(head);
	}
	else
	{
		printf("歌曲下载失败!\n");
	}
	close(sockfd);
	printf("按任意键继续...\n");
	getchar();
	return;
}

void songsheet() // ge dan
{
	int x, loop = 1;
	while (loop)
	{
		system("clear");
		printf("歌单\n");
		songmenu();
		printf("please select:");
		scanf("%d", &x);
		while (x < 0 || x > 5)
		{
			printf("输入有误,请重新输入:\n");
			scanf("%d", &x);
		}
		switch (x)
		{
		case 1:
			display_song();
			getchar();
			break;
		case 2:
			add_song();
			break;
		case 3:
			delete_song();
			break;
		case 4:
			download_song();
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