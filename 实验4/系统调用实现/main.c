#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
typedef struct Student
{
    char no[100];
    char name[30];
    char sex[100];
    char age[10];
    struct Student *next;
} Student,*StuList;
void PrintStuList(StuList stulist)
{
    Student *p=stulist;
    while(p->next)
    {
        printf("%s %s %s %s\n",p->next->no,p->next->name,p->next->sex,p->next->age);
        p=p->next;
    }
}
void FindStudent(StuList stulist)
{
    printf("请输入您要查询的学生姓名:");
    char name[30];
    scanf("%s",name);
    Student* p=stulist;
    while(p->next)
    {
        if(strcmp(p->next->name,name)==0)
        {
            printf("%s %s %s %s\n",p->next->no,p->next->name,p->next->sex,p->next->age);
            printf("查询成功！！！\n");
            break;
        }
        p=p->next;
    }
    if(p->next == NULL)
        printf("查无此人！！！\n");
}
void AddStudent(StuList stulist,int buff_size)
{
    int fd = open("students.txt", O_RDWR);
    if(fd==-1){
      printf("can not open the file\n");
      return;
    }
    ftruncate(fd,0);
    char buf[buff_size];
    buf[buff_size]='\0';
    int i,j;
    Student *stu = (Student*)malloc(sizeof(Student));
    stu->next=NULL;
    Student *p = stulist;
    printf("请输入学生信息:\n");
    printf("学号:");
    scanf("%s", stu->no);
    printf("姓名:");
    scanf("%s", stu->name);
    printf("性别:");
    scanf("%s", stu->sex);
    printf("年龄:");
    scanf("%s", stu->age);
    while(p->next)
        p=p->next;
    p->next=stu;
    i=0;
    p=stulist->next;
    while(p)
    {
        for(j=0;j<strlen(p->no);j++)
        {
            buf[i]=p->no[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->name);j++)
        {
            buf[i]=p->name[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->sex);j++)
        {
            buf[i]=p->sex[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->age);j++)
        {
            buf[i]=p->age[j];
            i++;
        }
        buf[i++]='\n';
        p=p->next;
    }
    buf[i]='\0';
    //printf("%s\n",buf);
    write(fd,buf,i);
    close(fd);
    printf("添加成功！！！\n");
}
void DeleteStudent(StuList stulist,int buff_size)
{
    char buf[buff_size];
    buf[buff_size-1]='\0';
    int fd = open("students.txt", O_RDWR);
    fd = open("students.txt", O_RDWR);
    if(fd==-1){
      printf("can not open the file\n");
      return;
    }
    ftruncate(fd,0);
    int i,j;
    Student *p;
    printf("输入你要删除的学生姓名:");
    char name[30];
    int flag=0;
    while(1)
    {
        scanf("%s",name);
        p=stulist;
        while(p->next)
        {
            if(strcmp(name,p->next->name)==0)
            {
                flag=1;
                if(p->next->next)
                {
                    Student *ptr=p->next;
                    p->next=p->next->next;
                    free(ptr);
                    break;
                }
                else if(!p->next->next)
                {
                    Student *ptr=p->next;
                    free(ptr);
                    p->next=NULL;
                    break;
                }
            }
            p=p->next;
        }
        if(flag==0)
        {
            printf("您输入的名字不存在！请重新输入:");
            continue;
        }
        else if(flag==1)
        {
            p=stulist->next;
            break;
        }
        memset(name, 0, sizeof(name));
    }
    i=0;
    while(p)
    {
        for(j=0;j<strlen(p->no);j++)
        {
            buf[i]=p->no[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->name);j++)
        {
            buf[i]=p->name[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->sex);j++)
        {
            buf[i]=p->sex[j];
            i++;
        }
        buf[i++]=' ';
        for(j=0;j<strlen(p->age);j++)
        {
            buf[i]=p->age[j];
            i++;
        }
        buf[i++]='\n';
        p=p->next;
    }
    //printf("%s\n",buf);
    write(fd,buf,i);
    close(fd);
    printf("删除成功！！！\n");
}
int main()
{
	int fd = open("students.txt", O_RDWR);
	lseek(fd,0,SEEK_SET);
	if(fd==-1){
      printf("can not open the file\n");
      return 0;
    }
    int buff_size = 1024;
	char buf[buff_size];
    StuList stulist;
    Student *stu;
    Student *p;
    stulist = (Student*)malloc(sizeof(Student));
    stulist->next=NULL;
    p = stulist;
	int nread = read(fd, buf, buff_size);
	close(fd);
	int i,j;
	i=0;
	while(i<nread)
    {
        stu = (Student*)malloc(sizeof(Student));
        stu->next=NULL;
        j=0;
        while(buf[i]!=' ')
        {
            stu->no[j]=buf[i];
            j++;
            i++;
        }
        stu->no[j]='\0';
        j=0;
        i++;
        while(buf[i]!=' ')
        {
            stu->name[j]=buf[i];
            j++;
            i++;
        }
        stu->name[j]='\0';
        i++;
        j=0;
        while(buf[i]!=' ')
        {
            stu->sex[j]=buf[i];
            j++;
            i++;
        }
        stu->sex[j]='\0';
        i++;
        j=0;
        while(buf[i]!=' '&&buf[i]!='\n')
        {
            stu->age[j]=buf[i];
            j++;
            i++;
        }
        stu->age[j]='\0';
        i++;
        p->next=stu;
        p=p->next;
        stu=NULL;
    }
    printf("\n---------欢迎使用学生信息管理系统-----------\n\n"
       "              1.显示全部学生信息\n"
       "              2.查询学生信息\n"
       "              3.添加学生信息\n"
       "              4.删除学生信息\n"
       "              5.退出系统\n\n");
    while(1)
    {
        int opt;
        printf("请输入你的操作:");
        scanf("%d",&opt);
        switch(opt)
        {
        case 1:
            PrintStuList(stulist);
            break;
        case 2:
            FindStudent(stulist);
            break;
        case 3:
            AddStudent(stulist,buff_size);
            break;
        case 4:
            DeleteStudent(stulist,buff_size);
            break;
        case 5:
            break;
        default:
            printf("输入有误，请重新输入！！\n");
            break;
        }
        if(opt == 5)
            break;
    }

    p = stulist;
    while(p)
    {
        Student* ptr=p;
        p=p->next;
        free(ptr);
    }
	return 0;
}

