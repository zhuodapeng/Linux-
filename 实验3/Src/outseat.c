#include"../Inc/outseat.h"

void RandomSort(Student *stu,int len)
{
    int t=50;
    int i,j;
    Student tempstu;
    srand((unsigned)time(NULL));
    while(t--)
    {
        i=rand()%len;
        j=rand()%len;
        tempstu=stu[i];
        stu[i]=stu[j];
        stu[j]=tempstu;
    }
}
void Exchange(Student *stu,int t,int allStuCount)
{
    int i,j;
    i=1;
    j=allStuCount-1;
    Student tempstu;
    srand((unsigned)time(NULL));
    while(t)
    {
        i=rand()%(allStuCount-t)+1;
        if(strcmp(stu[i-1].classes,stu[j].classes)!=0&&strcmp(stu[i+1].classes,stu[j].classes)!=0&&strcmp(stu[i].classes,stu[j-1].classes)!=0&&strcmp(stu[i].classes,stu[j+1].classes)!=0)
        {
            tempstu=stu[i];
            stu[i]=stu[j];
            stu[j]=tempstu;
            j--;
            t--;
        }
    }
}
