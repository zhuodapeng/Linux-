#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
typedef struct
{
    char no[10];
    char name[20];
    char sex[10];
    char age[10];
} Student;

Student stu[15];
int stulen=0;
static void *thread_input()
{
    pthread_mutex_lock(&mutex);
    FILE *fp;
    fp = fopen("students.txt", "r");
    int i=0;
    if(fp)
    {
        while(fscanf(fp,"%s %s %s %s", stu[i].no, stu[i].name, stu[i].sex, stu[i].age)!=EOF)
        {
           i++;
        }
        stulen=i;
        fclose(fp);
    }
    else
    {
        pthread_mutex_unlock(&mutex);
        printf("file not exist!!!\n");
        exit(0);
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}
static void *thread_order()
{
    pthread_mutex_lock(&mutex);
    while(stulen==0)
        pthread_cond_wait(&cond, &mutex);
    int i,j,min;
    for (i=0; i<stulen; i++)
    {
        min=i;
        for (j=i+1; j<stulen; j++)
        {
            if(strcmp(stu[min].name, stu[j].name) > 0)
                min=j;
        }
        Student tmp=stu[i];
        stu[i]=stu[min];
        stu[min]=tmp;
    }
    pthread_mutex_unlock(&mutex);
}
int main()
{
    pthread_t thid1,thid2;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&thid1, NULL, thread_input, NULL);
    pthread_create(&thid2, NULL, thread_order, NULL);

    pthread_join(thid1, NULL);
    pthread_join(thid2, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    int i=0;
    FILE *fp;
    fp=fopen("out.txt","w+");
    if(fp)
    {
        while( i<stulen )
        {
            fprintf(fp,"%s %s %s %s\n",stu[i].no,stu[i].name,stu[i].sex,stu[i].age);
            i++;
        }
        fclose(fp);
    }

    printf("ALL have done!\n");
}
