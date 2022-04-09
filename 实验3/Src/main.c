#include "../Inc/outseat.h"

int main()
{
    /* 初始化 */
    FILE* fp = fopen ("../IOfile/students.txt", "r");
    int i,j,k,n,m,flag;;
    char temp[1024];
    fscanf(fp, "%[^\n]",temp);
    Student stu[100][100];
    Student stu1[1024];
    Student tempstu;
    Room room[100];
    int allStuCount=0;
    int maxStuCountClasses=0;
    int oneClassesStuCount=0;
    int room_Num=0;
    int classesNum=0;
    int classesStuNum[100];
    memset(classesStuNum, 0, 100);

    /* 输入 */
    fscanf(fp, "%s %s %s %s",stu[classesNum][oneClassesStuCount].no,stu[classesNum][oneClassesStuCount].name,stu[classesNum][oneClassesStuCount].classes,stu[classesNum][oneClassesStuCount].sdept);
    allStuCount++;
    oneClassesStuCount++;
    classesStuNum[classesNum]++;
    while(fscanf(fp, "%s %s %s %s",tempstu.no,tempstu.name,tempstu.classes,tempstu.sdept) != EOF)
    {
        if(strcmp(tempstu.classes,stu[classesNum][0].classes)!=0)
        {
            classesNum++;
            oneClassesStuCount=0;
            stu[classesNum][oneClassesStuCount] = tempstu;
        }
        else
        {
            stu[classesNum][oneClassesStuCount] = tempstu;
        }
        oneClassesStuCount++;
        classesStuNum[classesNum]++;
        allStuCount++;
    }
    fp = fopen ("../IOfile/rooms.txt", "r");
    while(fscanf(fp, "%s %d",room[room_Num].name,&room[room_Num].count) != EOF)
    {
        room_Num++;
    }

    /* 对每个班级进行内部打乱 */
    for(i=0; i<=classesNum; i++)
    {
        RandomSort(stu[i],classesStuNum[i]);
        if(classesStuNum[i]>maxStuCountClasses)
            maxStuCountClasses=classesStuNum[i];
    }

    /* 将打乱完的班级进行第一次排序，并把最后剩下的一个班级人数加到数组最后 */
    k=0;
    while(flag < classesNum)
    {
        n=rand()%(classesNum+1);
        if(n==m || classesStuNum[n]==0)
        {
            continue;
        }
        stu1[k]=stu[n][classesStuNum[n]-1];
        classesStuNum[n]--;
        if(classesStuNum[n]==0)
            flag++;
        m=n;
        k++;
    }
    for(i=0; i<=classesNum; i++)
    {
        for(j=0;j<classesStuNum[i];j++)
            stu1[k++]=stu[i][j];
    }

    /* 进行第二次排序，主要作用是处理最后同班级的学生 */
    Exchange(stu1,maxStuCountClasses,allStuCount);

    /* 输出 */
    fp = fopen("../IOfile/out.txt","w+");
    for(i=0,j=0; i<room_Num; i++)
    {
        fprintf(fp,"---------------------------------教室:%s\n",room[i].name);
        for(k=0; k<room[i].count && j<allStuCount; k++,j++)
            fprintf(fp, "座号: %d %s %s %s %s\n",k+1,stu1[j].no,stu1[j].name,stu1[j].classes,stu1[j].sdept);
        if(j==allStuCount)
            break;
    }

    fclose(fp);
    return 0;
}
