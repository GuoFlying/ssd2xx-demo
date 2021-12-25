/*
 * @Author: Flying
 * @Date: 2021-12-25 07:01:34
 * @LastEditors: Flying
 * @LastEditTime: 2021-12-25 07:58:41
 * @Description: 新建文件
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// sqlite3_exec回调函数，执行sql语句后，有获取到内容时会调用该回调函数（如SELECT语句）
int callback(void *data, int ncols, char **values, char **headers)
{
    int i;
    fprintf(stderr, "%s : ", (char *)data);
    for (i = 0; i < ncols; i++)
    {
        fprintf(stderr, "%s=%s  ", headers[i], values[i]);
    }
    fprintf(stderr, "\n");
    return 0;
}

int main(int argc, char *argv[])
{
    //打开数据库或创建数据库
    sqlite3 *db;
    char *err;
    int rc = sqlite3_open("./test.db", &db);
    if (rc)
    {
        fprintf(stderr, "不能打开数据库，错误如下 : %s\n", sqlite3_errmsg(db));
        return -1;
    }

    //执行sql 语句
    struct sqlbuff_t
    {
        char *sql;
        int (*callback)(void *, int, char **, char **);
        void *data;
    } sqlbuff[] = {
        {"CREATE TABLE test_table (id integer PRIMARY KEY ,passwd text, other text);", NULL, NULL},
        {"INSERT INTO test_table (id,passwd,other)  VALUES(1001,'abc123','个性签名123');", NULL, NULL},
        {"INSERT INTO test_table VALUES(1002,'abc456','个性签名456'), (1003,'abc789','个性签名789');", NULL, NULL},
        {"SELECT * FROM test_table;", callback, (void *)"第一次查询回调 "},
        {"INSERT INTO test_table VALUES(1004,'smc-xm','smc other');", NULL, NULL},
        {"SELECT * FROM test_table;", callback, (void *)"增加数据后查询 "},
        {"UPDATE test_table SET passwd = 'cccccc' WHERE id = 1004;", NULL, NULL},
        {"SELECT * FROM test_table;", callback, (void *)"修改数据后查询 "},
        {"DELETE FROM test_table WHERE id = 1004;", NULL, NULL},
        {"SELECT * FROM test_table;", callback, (void *)"删除数据后查询 "}};

    for (int i = 0; i < sizeof(sqlbuff) / sizeof(sqlbuff[0]); i++)
    {
        rc = sqlite3_exec(db, sqlbuff[i].sql, sqlbuff[i].callback, sqlbuff[i].data, &err);
        if (rc != SQLITE_OK)
        {
            if (err)
            {
                fprintf(stderr, "SQL error: %s\n", err);
                sqlite3_free(err);
            }
        }
    }
    sqlite3_close(db);
    return 0;
}