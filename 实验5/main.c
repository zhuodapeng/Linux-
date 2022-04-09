#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<malloc.h>
#include<unistd.h>


int main()
{
    char *cmd1 = "./fun";
	char *cmd2 = "fun";
    printf("\n---------欢迎使用学生信息管理系统-----------\n\n"
           "              1.查询全部学生信息\n"
           "              2.查询学生信息\n"
           "              3.添加学生信息\n"
           "              4.删除学生信息\n"
           "              5.退出系统\n\n");
    pid_t pid;
    char str[2];
    str[1]='\0';
    while(1)
    {
        printf("请输入你的操作:");
        scanf("%c",&str[0]);
        getchar();
        if(str[0] == '5')
            break;
        if(str[0] != '1' && str[0] != '2' && str[0] != '3'&& str[0] != '4')
        {
            printf("输入有误，请重新输入！！\n");
            continue;
        }
        pid = fork();
        if((pid == -1))
        {
            perror("fork error");
            exit(1);
        }
        else if(pid == 0)
        {
            if(execlp(cmd1,cmd2,str,NULL)<0)
            {
                perror("execl error");
                exit(1);
            }
            else
            {
                perror("execl success");
            }
        }
        else if(pid > 0)
        {
            wait(NULL);
        }
    }
	return 0;
}
