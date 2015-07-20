/********************************************************************
**File name:	OperateSQLite.h
**Author:		titorx
**Version:		4.0
**Date:			2015.7.19
**Description:	OperateSQLite类用于完成对于SQLite数据库的各类操作
********************************************************************/

#pragma once
#include "sqlite3.h"
#include <iostream>
#include <string.h>
#include <string>
#include <list>

using namespace std;

typedef list<char**> TABLELIST;//存储查询结果的列表类型

class OperateSQLite
{
public:

    /******************************************************************
    **pDBName 要操作的数据库名
    **若该数据库不存在则自动创建该数据库
    **数据库名应为UTF8编码
    **该类用于便捷的操作SQLite3数据库
    **通过Exec方法执行SQL语句
    **GetError方法获取最近一次数据库执行错误
    **重载[]操作符用于获取查询结果
    **	O[i]	获取第i行		行越界则返回NULL
    **	O[i][j] 获取第i行第j列	列越界行为未定义
    ******************************************************************/
    OperateSQLite(string pDBName);


    //打开数据库
    bool OpenDB(string pDBName);


    //关闭数据库
    bool CloseDB(void);


    /******************************************************************
    **进行数据库操作
    **该方法对数据库的连接与关闭以及sqlite3_exec()函数进行了封装
    **使数据库操作更加简便
    **pErrMsg参数的使用请详细参考sqlite3_exec函数的用法
    ******************************************************************/
    bool Exec(
        string sql,				//要执行的SQL语句
        char **pErrMsg = NULL   //错误信息
    );


    /******************************************************************
    **CallBack函数用于sqlite3_exec函数的回调
    **保存查询返回的结果到lstResult
    **执行查询操作时，每查到一条记录调用一次CallBack函数
    **CallBack返回1表示停止查询
    **CallBack返回0表示继续查询
    ******************************************************************/
    static int CallBack(
        void *pv,   //传入回调函数的参数
        int argc,   //表的列数
        char **argv,//表的数据数组指针
        char **col  //表的列名数组指针
    );


    //清空存储查询结果的列表 彻底释放内存
    void ClearList(void);


    //获取最近一次的数据库执行错误
    int GetError(void);


    //OperateSQLite类的析构函数 释放所使用的内存
    ~OperateSQLite(void);


    //重载[]运算符 通过下标返回指定行 越界返回NULL
    char** operator [](int row);




    OperateSQLite *pThis;//指向自身的指针 在回调函数中使用

    /****************************************
    **列表中保存了上次查询所返回的结果
    **假设查询返回一个三行三列的表
    **则列表中的数据结构如下图所示
    **第1行    属性1--属性2--属性3
    **           |
    **第2行    属性1--属性2--属性3
    **           |
    **第3行    属性1--属性2--属性3
    ****************************************/
    TABLELIST lstResult;
    /****************************************
    **指向列表lstResult头的双向迭代器
    **使用方法：
    **		(*itResult)[0] 当前行第1个元素
    **		(*itResult)[1] 当前行第2个元素
    **		++ -- 切换上下行
    **		详情使用参考列表迭代器使用方法
    ****************************************/
    TABLELIST::iterator itResult;
    /****************************************
    **存储查询结果的字段名
    **其内部结构如下
    **         字段1--字段2--字段3-------
    **若要取第1个字段名:
    **         db.sColName[0]
    ****************************************/
    char **sColName;
    int iCols;//查询结果的列数
    int iRows;//查询结果的行数

    sqlite3 *hSQLiteDB;//数据库句柄
    string pDBName;//存储待操作的数据库文件名
    int iErrorMsg;//存储错误消息
    bool bOpen;//表明此时是否已经打开数据库
    bool bFirstSelect;//表明此时是否为第一次查询

};

