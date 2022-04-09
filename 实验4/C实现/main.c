#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<malloc.h>
typedef struct Student
{
    char no[100];
    char name[30];
    char sex[100];
    int age;
    struct Student *next;
} Student,*StuList;
void PrintStuList(StuList stulist)
{
    Student *p=stulist;
    while(p->next)
    {
        printf("%s %s %s %d\n",p->next->no,p->next->name,p->next->sex,p->next->age);
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
            printf("%s %s %s %d\n",p->next->no,p->next->name,p->next->sex,p->next->age);
            printf("查询成功！！！\n");
            break;
        }
        p=p->next;
    }
    if(p->next == NULL)
        printf("查无此人！！！\n");
}
void AddStudent(StuList stulist)
{
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
    scanf("%d", &(stu->age));
    FILE* fp = fopen("students.txt","a+");
    if(fp)
    {
        fseek(fp, 0, SEEK_END);
        fprintf(fp,"%s %s %s %d\n",stu->no,stu->name,stu->sex,stu->age);
        while(p->next)
            p=p->next;
        p->next=stu;
        fclose(fp);
        printf("添加成功！！！\n");
    }
}
void DeleteStudent(StuList stulist)
{
    FILE* fp = fopen ("students.txt", "w+");
    Student *p;
    if(fp)
    {
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
                printf("删除成功！！！\n");
                while(p)
                {
                    fprintf(fp,"%s %s %s %d\n",p->no,p->name,p->sex,p->age);
                    p=p->next;
                }
                break;
            }
            memset(name, 0, sizeof(name));
        }
        fclose(fp);
    }
}
int main()
{
    FILE* fp = fopen ("students.txt", "r");
    StuList stulist;
    Student *stu;
    Student *p;
    stulist = (Student*)malloc(sizeof(Student));
    stu = (Student*)malloc(sizeof(Student));
    stu->next=NULL;
    stulist->next=NULL;
    p = stulist;
    if(fp)
    {
        while(fscanf(fp,"%s %s %s %d", stu->no, stu->name, stu->sex, &(stu->age))!=EOF)
        {
            p->next=stu;
            p=p->next;
            stu = (Student*)malloc(sizeof(Student));
            stu->next=NULL;

        }
        fclose(fp);
    }
    printf("\n---------欢迎使用学生信息管理系统-----------\n\n"
           "              1.查询全部学生信息\n"
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
            AddStudent(stulist);
            break;
        case 4:
            DeleteStudent(stulist);
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
}

