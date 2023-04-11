#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

//?您是用来干嘛的我现在也不知道

class evalstate
{
public:
    QStringList value_line;
    QMap<QString,int> evalmap;

    evalstate();
    void check_varify();    //利用表达式树检查合理性

    void create_tree();
};

#endif // EVALSTATE_H
