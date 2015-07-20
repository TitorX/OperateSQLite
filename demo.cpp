#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include "OperateSQLite.h"


using namespace std;

int main(int argc, char *argv[])
{
    OperateSQLite *db = new OperateSQLite("demo.db");
    cout<<"OperateSQLite对象创建成功！"<<endl;
    /*
    **使用new创建一个类 打开名为demo.db的数据库.若数据库存在
    **则接下来的操作会针对该数据库进行。
    **若数据库不存在则会创建该数据库
    **注意！数据库的名字应为UTF8编码 若开发环境为vs 数据库名应使用纯英文命名
    */
    string sql;
    sql = "CREATE TABLE t1(name TEXT, sex TEXT)";

    db->Exec(sql);
    cout<<"创建t1表成功！"<<endl;
    /*
    **执行SQL语句创建名为t1的表
    **表中的两个字段 name sex 均为TEXT类型
    **TEXT为SQLite内置类型，更多关于SQLite数据类型的信息，到网上详细查阅
    */

    sql = "INSERT INTO t1 VALUES(\"Tom\", \"man\");"
          "INSERT INTO t1 VALUES(\"Lucy\", \"woman\");"
          "INSERT INTO t1 VALUES(\"Hellen\", \"woman\");";
    cout<<"执行sql语句："<<sql<<endl;


    db->Exec(sql);
    cout<<"插入数据Tom man;Lucy woman;Hellen woman成功！"<<endl;
    /*
    **执行一个插入语句 Exec方法可以一次执行多条语句
    **向 表t1 中插入两行数据 Tom man 以及 Lucy woman
    **SQLite支持多数SQL标准 但某些语句并不支持SQL标准
    **更多关于SQLite的语法 到网上查阅详细资料
    */
    sql = "SELECT * FROM t1 WHERE sex=\"woman\"";
    db->Exec(sql);
    cout<<"查询sex字段值为woman的数据"<<endl;
    /*
    **执行一次查询操作 返回 sex字段值为 "woman" 的行
    */

    cout<<"查询结果的列数："<<db->iCols<<endl;
    cout<<"查询结果的行数："<<db->iRows<<endl;
    /*
    **iCol iRow分别保存了查询所返回结果的列数与行数
    */

    if (db->iRows==0)
    {
        //若查询返回0个结果
        cout<<"并没有查询到符合条件的数据"<<endl;
    }
    else
    {
        //输出所返回的结果

        //输出查询的字段名
        cout<<setw(4)<<"0"<<"|";
        for(int i =0; i < db->iCols; ++i)
        {
            cout<<setw(10)<<db->sColName[i]<<";";
        }
        cout<<endl;

        //输出结果
        cout<<"--------------------------------------"<<endl;
        for (int i = 0; i < db->iRows; i++)
        {
            cout<<setw(4)<<(i+1)<<"|";
            for (int j = 0; j < db->iCols; j++)
            {
                cout<<setw(10)<<(*db)[i][j]<<";";
                /*
                **该类重载了[]运算符 db[i]返回char **类型的数组 其中存储了一行数据
                **对 db[i] 的返回值再次使用 []操作符 即可取出相应列的数据
                **注意！此处demo中db为指针 因此使用了 (*db)[][]的形式
                **使用时务必注意
                */
            }
            cout<<endl;
        }
    }


    delete db;
    //数据库使用完毕 使用delete删除new出来的对象 此时自动调用析构函数 完成内存的释放

    cout<<"数据库对象删除成功！"<<endl;

    getchar();

    return 0;
}
