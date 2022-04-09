#ifndef _LIST_H
#define _LIST_H


struct songnode
{
	int songid;
	char path[100];
	struct songnode *next;
};

typedef struct songnode Song;

void freesong(Song *);

Song *read_song();

int display_song();

int add_song();

int delete_song();

int exist_song(char *buf);      //判断一首歌是否重复添加

void download_song();

void songsheet();

#endif
