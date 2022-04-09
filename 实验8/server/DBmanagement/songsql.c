#include <stdio.h>
#include <sqlite3.h>

int main()
{
    sqlite3* db= NULL;
    int rc;
    char *Errormsg;
    int row, col;
    int i = 0;
    int j = 0;
    char **Result;
    rc = sqlite3_open("song.db",&db);
    if(rc)
    {
        fprintf(stderr, "can't open:%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    else
    {
        printf("open success\n");
    }

    /* 歌单表格 */
    //char *sql = "create table song(ID integer primary key, songname varchar(20),songpath varchar(20))";
    //sqlite3_exec(db, sql, 0, 0, &Errormsg);

    /* 插入数据 */
    char *sql = "insert into song values(NULL,'句号.mp3','./res/句号.mp3')";
    sqlite3_exec(db, sql, 0, 0, &Errormsg);
    sql = "insert into song values(NULL,'Panda_Desiigner.mp3','./res/Panda_Desiigner.mp3')";
    sqlite3_exec(db, sql, 0, 0, &Errormsg);

    /* 删除操作 */
    // char *sql = "delete from song where ID = 2 or ID = 3 or ID = 4 or ID = 1";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);

    /* 管理员表格 */
    // char *sql = "create table admin(ID integer primary key, adminname varchar(20),adminpasswd varchar(20))";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);

    /* 插入数据 */
    // sql = "insert into admin values(NULL,'zyp','zyp')";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);
    // sql = "insert into admin values(NULL,'zhuoyunpeng','zhuoyunpeng')";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);
    // char* sql = "insert into song values(NULL,'Panda_Desiigner.mp3','./res/Panda_Desiigner.mp3')";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);
    // sql = "insert into song values(NULL,'句号.mp3','./res/句号.mp3')";
    // sqlite3_exec(db, sql, 0, 0, &Errormsg);
    sql = "select * from song";
    rc = sqlite3_get_table(db, sql,&Result,&row,&col,&Errormsg);
    printf("rc: %d\n", rc);
    printf("row=%d column=%d\n", row, col);
    for (i = 0; i < row + 1;i++)
    {
        for (j = 0; j < col;j++)
        {
            printf("%s\t", Result[j + i * col]);
        }
        printf("\n");
    }
    sqlite3_free(Errormsg);
    sqlite3_free_table(Result);
    sqlite3_close(db);
    return 0;
}
