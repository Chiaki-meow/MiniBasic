#ifndef EXP_TREE_H
#define EXP_TREE_H

#include <QString>
#include <QMap>

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

class EvaluationContext {

public:

    void setValue(QString var, int value);
    int getValue(QString var);
    bool isDefined(QString var);

    QMap<QString,int> symbolTable;
};


class Expression {

public:
    Expression(){};
    ~Expression(){};
    virtual int eval(EvaluationContext & context) = 0;
    virtual QString toString() = 0;
    virtual ExpressionType type() = 0;

    /* Getter methods for convenience */

    virtual int getConstantValue(){}
    virtual QString getIdentifierName(){}
    virtual QString getOperator(){}
    virtual Expression *getLHS(){}
    virtual Expression *getRHS(){}
    virtual bool ifExist(){}

};


class ConstantExp: public Expression {

public:

    ConstantExp(int val):value(val){}

    virtual int eval(EvaluationContext & context);
    virtual QString toString(){return QString::number(value);}
    virtual ExpressionType type(){return CONSTANT;}

    virtual int getConstantValue(){return value;}
    virtual bool ifExist(){return exist;}

private:
    bool exist=false;
    int value;

};


class IdentifierExp: public Expression {

public:

    IdentifierExp(QString n):name(n){}

    virtual int eval(EvaluationContext & context);
    virtual QString toString(){return name;}
    virtual ExpressionType type(){return IDENTIFIER;}

    virtual QString getIdentifierName(){return name;}
    virtual bool ifExist(){return exist;}

private:
    QString name;
    bool exist=false;
};


class CompoundExp: public Expression {

public:

    CompoundExp(QString o, Expression *lh, Expression *rh):op(o),lhs(lh),rhs(rh){}

    virtual int eval(EvaluationContext & context);
    virtual QString toString(){return op;}
    virtual ExpressionType type(){return COMPOUND;}

    virtual QString getOperator(){return op;}
    virtual Expression *getLHS(){return lhs;}
    virtual Expression *getRHS(){return rhs;}
    virtual bool ifExist(){return exist;}

private:
    QString op;
    Expression *lhs, *rhs;
    bool exist=false;


};


#endif // EXP_H
