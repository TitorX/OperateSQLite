/********************************************************************
**File name:	OperateSQLite.cpp
**Author:		titorx
**Version:		4.0
**Date:			2015.7.19
**Description:	OperateSQLite类用于完成对于SQLite数据库的各类操作
********************************************************************/

#include "OperateSQLite.h"


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
OperateSQLite::OperateSQLite(string pDBName)
{
    this->pThis = this;    //指向自身以用于回调函数
    this->hSQLiteDB = NULL;//初始化数据库句柄
    this->iRows = 0 ;      //将查询结果列数置零
    this->iCols = 0 ;      //将查询结果行数置零
    this->iErrorMsg = SQLITE_OK;//初始化错误信息
    this->pDBName = pDBName;    //保存数据库名
    this->bOpen = false;  //初始化数据库打开状态
    this->bFirstSelect = false;//初始化查询状态
}


//打开数据库
bool OperateSQLite::OpenDB(string pDBName)
{
    if( true == bOpen )
    {
        //若数据库已处于连接状态则连接数据库
        return true;
    }

    iErrorMsg = sqlite3_open(pDBName.c_str(), &hSQLiteDB);//连接数据库


    if ( SQLITE_OK == iErrorMsg )
    {
        //数据库打开成功
        bOpen = true;//表明当前处于打开数据库状态
        return true;
    }
    else
    {
        //数据库打开失败
        return false;
    }

}


//关闭数据库
bool OperateSQLite::CloseDB(void)
{
    if( false == bOpen )
    {
        //没有数据库处于打开状态
        return true;
    }

    iErrorMsg = sqlite3_close(hSQLiteDB);

    if ( SQLITE_OK == iErrorMsg )
    {
        bOpen = false;//表明当前没有打开的数据库
        hSQLiteDB = NULL;//数据库句柄重置
        return true;
    }
    else
    {
        return false;//数据库关闭失败
    }
}


/******************************************************************
**Exec方法适用于进行基础的数据库操作
**进行数据库操作
**该方法对数据库的连接与关闭以及sqlite3_exec()函数进行了封装
**pErrMsg参数的使用请详细参考sqlite3_exec函数的用法
******************************************************************/
bool OperateSQLite::Exec(
    string sql,		 //要执行的SQL语句
    char **pErrMsg)  //错误信息
{
    ClearList();//清除上次的查询结果

    if( false == OpenDB(pDBName.c_str()))//打开数据库
    {
        //若数据库连接失败
        return false;
    }


    bFirstSelect = true;//标志开始进行查询

    iErrorMsg = sqlite3_exec(hSQLiteDB,               //数据库句柄
                             sql.c_str(),                     //执行的sql语句
                             OperateSQLite::CallBack, //回调函数
                             pThis,                   //指向自身的指针
                             pErrMsg);//执行SQL语句

    bFirstSelect = false;//标志结束查询

    CloseDB();//关闭数据库

    return true;
}


/******************************************************************
**CallBack函数用于sqlite3_exec函数的回调
**保存查询返回的结果到lstResult
**执行查询操作时，每查到一条记录调用一次CallBack函数
**CallBack返回1表示停止查询
**CallBack返回0表示继续查询
******************************************************************/
int OperateSQLite::CallBack(
    void *pv,   //传入回调函数的参数
    int argc,   //表的列数
    char **argv,//表的数据数组指针
    char **col) //表的列名数组指针
{
    char **pResult;//存储一行数据
    pResult	= new char *[argc];
    for (int i = 0; i < argc; i++)
    {
        if (argv[i]!=NULL)
        {
            pResult[i] = new char [strlen(argv[i])+1];
            memcpy(pResult[i], argv[i], strlen(argv[i])+1);
        }
        else
        {
            pResult[i] = NULL;
        }
    }//将argv中的数据存入到pResult中

    OperateSQLite *poSQLite = (OperateSQLite*)pv;

    TABLELIST *lstResult = &poSQLite->lstResult;

    lstResult->push_back(pResult);//将pResult存入list末尾

    poSQLite->iRows += 1 ; //每存储一条记录行数自加1

    if( true == poSQLite->bFirstSelect )
    {
        //若当前是第一次查询则保存字段名并保存列数

        poSQLite->iCols = argc;//保存列数
        char **Name;
        Name = new char *[argc];

        for (int i = 0; i < argc; i++)
        {
            if (col[i]!=NULL)
            {
                Name[i] = new char [strlen(col[i])+1];
                memcpy(Name[i], col[i], strlen(col[i])+1);
            }
            else
            {
                Name[i] = NULL;
            }
        }//将col中的数据存入到Name中

        poSQLite->sColName = Name;//保存字段名
    }

    return 0;
}


//彻底释放内存
void OperateSQLite::ClearList(void)
{
    //清除存储查询结果的列表
    TABLELIST::iterator itNext = lstResult.begin();
    TABLELIST::iterator itNow  = itNext;
    while (itNext!=lstResult.end())
    {
        ++itNext;

        for (int i = 0; i < iCols; ++i)
        {
            if ((*itNow)[i]!=NULL)
            {
                delete[] (*itNow)[i];
            }
        }
        delete[] (*itNow);
        lstResult.erase(itNow);

        itNow = itNext;
    }


    //清除字段名占用的内存
    for(int i = 0; i < iCols; ++i)
    {
        delete[] sColName[i];
    }
    delete[] sColName;

    iCols = 0;
    iRows = 0;
}


//获取最近一次的数据库执行错误
int OperateSQLite::GetError(void)
{
    return iErrorMsg;
}


//OperateSQLite类的析构函数
OperateSQLite::~OperateSQLite(void)
{
    CloseDB();  //关闭数据库连接
    ClearList();//清除查询结果
}


//重载[]运算符 通过下标返回指定行 越界返回NULL
char** OperateSQLite::operator [](int row)
{
    itResult = lstResult.begin();//创建迭代器
    if (row>=0 && row<iRows)	 //判断给出的下标是否越界
    {
        //下标未越界
        for (int i = 0; i <row ; i++)
        {
            ++itResult;//通过循环将迭代器指向需要的行
        }
        return (*itResult);//返回行
    }
    else
    {
        //下标越界
        return NULL;
    }
}
