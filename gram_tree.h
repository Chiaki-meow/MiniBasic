#ifndef GRAM_TREE_H
#define GRAM_TREE_H

#include "parser.h"
#include <QStringList>

//好像用错了 这个负责把语法树构建出来

class gramTree
{
public:
    parser pars_gram;   //和prog的相同
    QStringList gram_list;  //list保存代码
    QStringList colour_list;    //list保存错误行号

    QString line;   //保存在读一棵树的每一条

    QString error="Error";

    bool correct_flag=true;

    gramTree();
    void gram_build(parser pars, QStringList strList, int lNum, int oper, bool &run_this);
    void gram_buildTree(parser pars,Expression* exp,int pedding_length,QStringList& tmplist);
    void error_report(QString line_num);
};

#endif // GRAM_TREE_H
