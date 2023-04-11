#ifndef PROGRAM_H
#define PROGRAM_H

#include "statement.h"
#include "evalstate.h"
#include "parser.h"
#include "gram_tree.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QMap>


class program : public QObject
{
    Q_OBJECT
public:
    QList <statement> state;

    parser pars;    //保存每一个当时的树
    gramTree gramTree;      //保存语法树

    QString op;
    QString number;
    QString printfResult;

    int GOTOcur;

    int cur=0;    //保存目前的字符个数
    QList<int> error_line;

    //为了实现单点需要的变量
    bool debugging_flag=false;
    QList<statement> undebugging_state;

    QStringList varList;

    //为了input实现所需要的变量
    QString input_val;
    int input_num = 0;
    QString input_var[10000];
    bool running_flag = false;

    //为了goto实现所需要的变量
    bool GoTo_flag = false;
    int GoTo_num = 0;

    program();

    void run_program();
    void state_change_prog(QList<statement>::Iterator &sta);

    void run_gramTree();
    void build_gramTree(statement& stalist);

    //debug Mode
    void run_debug();
    void debug_allgramcheck();
    void gramTree_debug();

    void rerun_gramTree();
    void rerun_program();

    void prog_let(QStringList stringlist,int value);
    void prog_print(QStringList stringlist,int value);
    void prog_input(QStringList stringlist,int value);

    bool check_num(QString str);
    void varList_refresh();

//prog函数
    void set_program_input(QString input);
    void set_program_file(QFile &file);
    void check_same(int num);

private slots:
    void emit_print(){emit sig_print();}
    void emit_printf(){emit sig_printf();}
    void emit_input(){emit sig_input();}
    void emit_inputs(){emit sig_inputs();}

    void emit_gram(){emit sig_gram_tree_print();}
    void emit_compile_error(){emit sig_compile_error();}
    void emit_line_error(){emit sig_line_error();}
    void emit_varList(){emit sig_varList();}

public:
signals:
    void sig_print();
    void sig_printf();
    void sig_input();
    void sig_inputs();
    void sig_gram_tree_print();
    void sig_compile_error();
    void sig_line_error();
    void sig_varList();
};

#endif // PROGRAM_H
