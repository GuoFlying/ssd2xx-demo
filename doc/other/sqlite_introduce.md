<!--
 * @Author: Flying
 * @Date: 2021-12-16 10:47:48
 * @LastEditors: Flying
 * @LastEditTime: 2021-12-26 00:46:14
 * @Description: 新建文件
-->

# SQLite 基本概念及使用概述

- 基于 SSD平台加以介绍
  
## 1、什么是SQL

- SQL 指结构化查询语言
- SQL 使我们有能力访问数据库
- SQL 是一门 ANSI 的标准计算机语言，用来访问和操作数据库系统。SQL 语句用于取回和更新数据库中的数据。SQL 可与数据库程序协同工作，比如 MS Access、DB2、Informix、MS SQL Server、Oracle、Sybase 以及其他数据库系统。

- 不幸地是，存在着很多不同版本的 SQL 语言，但是为了与 ANSI 标准相兼容，它们必须以相似的方式共同地来支持一些主要的关键词（比如 SELECT、UPDATE、DELETE、INSERT、WHERE 等等）。
  
## 2、什么是数据库

- 数据库是存放数据的仓库。
- 数据库是一个按数据结构来存储和管理数据的计算机软件系统。数据库的概念实际包括两层意思。
  - （1）数据库是一个实体，它是能够合理保管数据的“仓库”，用户在该“仓库”中存放要管理的事务数据，“数据”和“库”两个概念结合成为数据库。
  - （2）数据库是数据管理的新方法和技术，它能更合适的组织数据、更方便的维护数据、更严密的控制数据和更有效的利用数据。
  
## 3、什么是SQLite

- SQLite是一个c语言库，它实现了一个小型、快速、自包含、高可靠性、功能齐全的SQL数据库引擎。 SQLite是世界上使用最多的数据库引擎。 SQLite内置在所有移动电话和大多数电脑中，并捆绑在人们每天使用的无数其他应用程序中。
- SQLite 是一个软件库，实现了自给自足的、无服务器的、零配置的、事务性的 SQL 数据库引擎。SQLite 是在世界上最广泛部署的 SQL 数据库引擎。SQLite 源代码不受版权限制。
- SQLite文件格式是稳定的、跨平台的、向后兼容的，开发人员承诺将一直保持这种方式到2050年。 SQLite数据库文件通常用作在系统之间传输丰富内容的容器，并用作数据的长期归档格式。 有超过1万亿(1e12) SQLite数据库在积极使用。  

## 4、为什么要用SQLite

- linux arm 上资源有限，需要保存数据时常用几种方式
  
### 4.1、思考

- 需要保存 3个 帐号、密码以及个性签名。linux arm上如何存储(存储时，需要考虑如何合理的进行增、删、改、查)
  
  ```shell
  1001  abc123 个性签名123
  1002  abc456 个性签名456
  1003  abc789 个性签名789
  ```

### 4.2、通过文本保存（fopen...）

#### 4.2.1、ini 格式存储

  ```ini
  [用户1]
  id = 1001; 
  passwd = abc123;
  other = 个性签名123;
  [用户2]
  id = 1002; 
  passwd = abc456;
  other = 个性签名456;
  [用户3]
  id = 1003; 
  passwd = abc789;
  other = 个性签名789;
  ```

- 按一定规格存储，可方便扩展

#### 4.2.2、csv 格式存储

  ```c
  id,passwd,other
  1001,"abc123","个性签名123"
  1002,"abc456","个性签名456"
  1003,"abc789","个性签名789"
  ```
  
- 按一定规格存储，可方便扩展
  
#### 4.2.3、json 格式存储

  ```json
  {
    "用户1":{
          "id":1001,
          "passwd":"abc123",
          "other":"个性签名123"
      },
    "用户2":{
          "id":1002,
          "passwd":"abc456",
          "other":"个性签名789"
      },
    "用户3":{
          "id":1003,
          "passwd":"abc789",
          "other":"个性签名789"
      },
  }
  ```
  
- 按一定规格存储，可方便扩展

#### 4.2.4、直接保存数据

- 直接将原数据按字符串保存
  
  ```shell
  1001  abc123 个性签名123
  1002  abc456 个性签名456
  1003  abc789 个性签名789
  ```

- 读取时候，根据原始数据规则进行读取。
- 写数据时，都需要去计算需要修改的光标位置。还需将修改的背后位置往后移动，然后重新写数据。
  
- 若需要增加一列“注册时间”时，需要重新修改数据读取规则。
  
#### 4.2.5、总结

如上列出的文本直接存储数据有如下问题

- 文本直接存储的方式，增删改时会很麻烦。
- ini、csv方式操作时，若从中插入或删除数据时，需要将插入之后的数据读出，插入或修改好后再重新写入文件
- json、直接原数据保存，在增删改时，需要读出全部数据，修改完成后，再重新写入数据
- 大文件操作时，速度较慢。若在操作过程中设备突然断电，会导致数据全部丢失。
- **文本保存的方式，适用于极少需要进行修改的数据存储：如配置文件等**

## 5、SQLite 优势

- 轻量：资源占用低（完全配置时小于400KiB，省略可选功能配置时小于250KiB）
- 纯C编写，移植方便
- 发布于公开领域(Public Domain);

## 6、SQLite3 下载、编译lib

### 6.1、ubuntu上安装 sqlite 命令

- 安装sqlite3 命令 `sudo apt-get install sqlite3`
- 安装sqlite3 头文件 `sudo apt-get install libsqlite3-dev`

### 6.2、下载

- sqlite官网直接下载 https://www.sqlite.org/
- 在ubuntu下可通过wget直接下载`wget https://www.sqlite.org/snapshot/sqlite-snapshot-202107191400.tar.gz`

### 6.2解压、配置编译

```shell
# 
tar -zxvf sqlite-snapshot-202107191400.tar.gz
cd sqlite-snapshot-202107191400/
mkdir build
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build
make -j8
make install 
```

## 7、SQLite3 命令行操作

- 环境:ubuntu
- 若输入非点（`.`）命令，结束为分号（`;`）
- **shell命令**
- 输入`sqlite3`命令后可进入sqlite3提示符

sqlite3提示符命令 | 说明 | 例举
---------|---------- | ----------
.exit | 退出sqlite3 交互 | \
.quit | 退出sqlite3 交互 | \
.help | 显示点命令帮助 | \
.databases | 列出数据库的名称及其所依附的文件 | \
.show  | 查看 SQLite 命令提示符的默认设置 | \
CREATE  |创建表| CREATE TABLE test_table (id integer ,passwd text, other text);
.schema | 查看所有表结构 | \
INSERT  | 插入数据 |INSERT INTO test_table (id,passwd,other)  VALUES(1001,"abc123","个性签名123");
SELECT | 查询数据 | SELECT * FROM test_table;
UPDATE | 修改数据 | UPDATE test_table SET passwd = "abc1234" WHERE id = 1001;
DELETE | 删除数据 | DELETE FROM test_table WHERE id = 1001;

- 操作事例

```c
sqlite3 test.db
CREATE TABLE test_table (id integer ,passwd text, other text);
INSERT INTO test_table (id,passwd,other)  VALUES(1001,"abc123","个性签名123");
INSERT INTO test_table VALUES(1002,"abc456","个性签名456"), (1003,"abc789","个性签名789");
SELECT * FROM test_table;
INSERT INTO test_table VALUES(1004,"smc-xm","smc other");
SELECT * FROM test_table;
UPDATE test_table SET passwd = "cccccc" WHERE id = 1004;
SELECT * FROM test_table;
DELETE FROM test_table WHERE id = 1004;
SELECT * FROM test_table;
```

![操作](https://gitee.com/mFlying/flying_picgo_img/raw/master/sqlite_test.png)

## 8、SQLite 常用语句

- 数据库操作中的基础操作：增、删、改、查。

### 8.1 CREATE TABLE 语句

- 创建表

```sqlite3
CREATE TABLE table_name(
   column1 datatype,
   column2 datatype,
   column3 datatype,
   .....
   columnN datatype,
   PRIMARY KEY( one or more columns )
);
```

### 8.2 INSERT INTO 语句

- 插入数据

```sqlite3
INSERT INTO table_name( column1, column2....columnN)
VALUES ( value1, value2....valueN);
```

### 8.3 DELETE 语句

- 删除数据

```sqlite3
DELETE FROM table_name
WHERE  {CONDITION};
```

### 8.4 UPDATE 语句

- 修改/更新数据

```sqlite3
UPDATE table_name
SET column1 = value1, column2 = value2....columnN=valueN
[ WHERE  CONDITION ];
```

### 8.5 SELECT 语句

- 修改/更新数据

```sqlite3
SELECT column1, column2....columnN
FROM   table_name;
```

## 9、SQLite 语法

- 有个重要的点值得注意，SQLite 是不区分大小写的，但也有一些命令是大小写敏感的，比如 GLOB 和 glob 在 SQLite 的语句中有不同的含义。

- 所有的 SQLite 语句可以以任何关键字开始，如 SELECT、INSERT、UPDATE、DELETE、ALTER、DROP 等，所有的语句以分号 ; 结束。

- 支持 算术运算符（+ - * / %）
- 支持 比较运算符（== != > < ……）
- 支持 逻辑运算符（AND OR NOT IN ……）
- 支持 位运算符 （& | ~ << >>）

- 数据类型

存储类 | 描述
---------|----------
 NULL | 值是一个 NULL 值。
 INTEGER | 值是一个带符号的整数，根据值的大小存储在 1、2、3、4、6 或 8 字节中。
 REAL | 值是一个浮点值，存储为 8 字节的 IEEE 浮点数字。
 TEXT | 值是一个文本字符串，使用数据库编码（UTF-8、UTF-16BE 或 UTF-16LE）存储。
 BLOB | 二进制

## 10、SQLite C 示例代码

### 10.1、利用SQL语句 简易调用C代码使用DMEO

```c
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
```

- 执行结果：

![执行](https://gitee.com/mFlying/flying_picgo_img/raw/master/sqlite_test1.png)

## 11、SQLite C 常用API

- An Introduction To The SQLite C/C++ Interface https://www.sqlite.org/cintro.html

### 11.1、打开数据库

```c
int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
int sqlite3_open16(
  const void *filename,   /* Database filename (UTF-16) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
int sqlite3_open_v2(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb,         /* OUT: SQLite db handle */
  int flags,              /* Flags */
  const char *zVfs        /* Name of VFS module to use */
);
```

### 11.2、执行数据库语句

```c
int sqlite3_exec(
  sqlite3*,                                  /* An open database */
  const char *sql,                           /* SQL to be evaluated */
  int (*callback)(void*,int,char**,char**),  /* Callback function */
  void *,                                    /* 1st argument to callback */
  char **errmsg                              /* Error msg written here */
);
```

### 11.3、 关闭数据库

```c
int sqlite3_close(sqlite3*);
int sqlite3_close_v2(sqlite3*);
```

### 11.4、 释放sqlite3 内部申请的资源

```c
void sqlite3_free(void*);
```

### 11.5、返回sqlite3 执行时错误信息

```c
char *sqlite3_errmsg(sqlite3*);
```

### 11.6、其他

- sqlite3_get_table
- sqlite3_prepare
- sqlite3_column
- sqlite3_bind
- sqlite3_step
- ……

## 12、资源

- sqlite3
  - 官网：`https://www.sqlite.org/index.html`
- SQLiteC++提供了对SQLite原生C api的封装，并提供了一些直观且文档良好的c++类。  
  - 源码路径：`https://github.com/SRombauts/SQLiteCpp`
- sqlitestudio (SQLite 数据库管理工具)
  - 官网：`https://sqlitestudio.pl/`
  - 源码路径：`https://github.com/pawelsalawa/sqlitestudio.git`

## 13、 SQLite 常用高级

### 13.1、PRAGMA

- SQLite 的 PRAGMA 命令是一个特殊的命令，可以用在 SQLite 环境内控制各种环境变量和状态标志。一个 PRAGMA 值可以被读取，也可以根据需求进行设置。
- eg: encoding Pragma 控制字符串如何编码及存储在数据库文件中

### 13.2、 约束

- 约束是在表的数据列上强制执行的规则。这些是用来限制可以插入到表中的数据类型。这确保了数据库中数据的准确性和可靠性。
- eg:限制 id不能为空 `NO NULL`

### 13.3、 别名

- 可以暂时把表或列重命名为另一个名字，这被称为别名。使用表别名是指在一个特定的 SQLite 语句中重命名表。重命名是临时的改变，在数据库中实际的表的名称不会改变。
- eg: 有两个表，里面都有id字段，需要用一天sql语句查询打印时

### 13.4、 触发器（Trigger）

- SQLite 触发器（Trigger）是数据库的回调函数，它会在指定的数据库事件发生时自动执行/调用
- eg :智能家居中，有场景表、设备表。删除设备时需要同步删除场景表中设备的内容。

### 13.5、 事务（Transaction）

- 事务（Transaction）是一个对数据库执行工作单元
- eg: 保证数据库完整性，sql语句太多时，设备掉电容易造成数据不完整，通过事务处理能保证数据完整性
