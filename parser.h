#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QPair>
#include <QStack>
#include <QMap>

#include "statement.h"
#include "exp_tree.h"

//把一个句子变成树
//gram_tree里实现的才是对应的parser功能
//每一棵树不是一直保存的，而是需要调用的时候才会生成

class parser
{
public:
    parser();

    int haveequal= 0;
    int haveXiaoYinhao=0;
    int haveDaYinhao=0;
    int haveKuohao=0;
    bool op_flag=true;
    bool isString=false;
    bool isPrintf=false;

    QString PrintfResult;
    QString nowStrName;

    bool good_statement=true;

    QStack<QString> operators;
    QStack<Expression*> operands;
    QList<QString> replaceString;

    EvaluationContext context;
    QMap<QString,QString> stringTable;
    QList<QString> input_var;

    Expression *equal; //一棵树
    Expression *lhs, *rhs;

    void readString(QStringList stringlist);
    void pushCom();
    void printf_check(QStringList stringlist);
    void printf_replace(QStringList stringlist);
    int evaluation(Expression *&equal);
    bool check_correct(Expression* exp);
    bool check_exist(Expression* exp);
    void init();
};

#endif // PARSER_H
