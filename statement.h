#ifndef STATEMENT_H
#define STATEMENT_H

//存放单条语句

#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextStream>

enum STAT {REM, LET, PRINT, INPUT, GOTO, IF, END, INPUTS, PRINTF};

class statement{

public:
    int num;
    int oper=-1;
    int cursor=0;
    bool error=false;

    bool toint = true;
    bool excu=true; //执行这一条

    QStringList stringlist;

    statement(){
        QString s = nullptr;
        stringlist = s.split(" ");
    };

    void read_input(QString input);     //读取语句并保存 调用get_op进行处理
    void get_op();      //重设自己的statement/command
    void split_statement();    //分解一条statement
};

#endif // STATEMENT_H
