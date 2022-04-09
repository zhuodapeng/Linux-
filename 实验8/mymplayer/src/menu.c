#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "list.h"

int mainmenu()
{
	int x;
	printf("\t\t\t      Mplayer\n");
	printf("\t\t\t********************\n");
	printf("\t\t\t     1.歌单\n");
	printf("\t\t\t     2.播放音乐\n");
	printf("\t\t\t     3.服务器管理\n");
	printf("\t\t\t     0.退出\n");
	printf("\t\t\t********************\n");
	printf("please select:\n");
	scanf("%d", &x);
	while (x < 0 || x > 3)
	{
		printf("输入有误,请重新输入\n");
		scanf("%d", &x);
	}
	return x;
}

int songmenu()
{
	printf("\t\t\t       Mplayer\n");
	printf("\t\t\t**********************\n");
	printf("\t\t\t     1.音乐列表\n");
	printf("\t\t\t     2.添加歌曲\n");
	printf("\t\t\t     3.删除歌曲\n");
	printf("\t\t\t     4.下载音乐\n");
	printf("\t\t\t     0.返回上一级\n");
	printf("\t\t\t**********************\n");
	return 0;
}

int contrlmenu() //音乐菜单
{
	int x;
	printf("\t\t\t      Mplayer\n");
	printf("\t\t\t********************\n");
	printf("\t\t\t     1.点歌\n");
	printf("\t\t\t     2.暂停/播放\n");
	printf("\t\t\t     3.上一首\n");
	printf("\t\t\t     4.下一首\n");
	printf("\t\t\t     5.快进\n");
	printf("\t\t\t     6.快退\n");
	printf("\t\t\t     0.返回上一级\n");
	printf("\t\t\t********************\n");

	Song *p, *head;
	head = read_song();
	p = head;
	if (p == NULL)
	{
		getchar();
		printf("文件中还没有歌曲,请先添加歌曲!\n");
		printf("按任意键继续:\n");
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
	printf("please select:\n");
	scanf("%d", &x);
	getchar();
	while (x < 0 || x > 6)
	{
		printf("输入有误,请重新输入:\n");
		scanf("%d", &x);
	}
	return x;
}

int adminmenu()
{
	int x;
	printf("\t\t\t  ServerManagement\n");
	printf("\t\t\t********************\n");
	printf("\t\t\t     1.查询服务器歌单\n");
	printf("\t\t\t     2.上传音乐\n");
	printf("\t\t\t     3.删除音乐\n");
	printf("\t\t\t     0.退出登录\n");
	printf("\t\t\t********************\n");
	printf("please select:\n");
	scanf("%d", &x);
	while (x < 0 || x > 3)
	{
		printf("输入有误,请重新输入:\n");
		scanf("%d", &x);
	}
	return x;
}