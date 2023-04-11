#include "exp_tree.h"
#include "QtMath"

void EvaluationContext::setValue(QString var, int value){
    symbolTable.insert(var,value);
}

int EvaluationContext::getValue(QString var){
    return symbolTable.find(var).value();
}

bool EvaluationContext::isDefined(QString var){
    return symbolTable.contains(var);
}

int ConstantExp::eval(EvaluationContext & context) {
    exist=true;
    return value;
}

int IdentifierExp::eval(EvaluationContext & context) {
    if (!context.isDefined(name)){
        return 0;
    }
    if(this->getIdentifierName()=="") //如果名字为空
        return 0;   //数值是0；那不影响
    exist=true;
    return context.getValue(name);
}

int CompoundExp::eval(EvaluationContext & context) {
    bool nameProper=true;
    int tmpLeft;
    int right = rhs->eval(context);
    if (op == "=") {
        exist=rhs->ifExist();
        if(rhs->ifExist()){
            if(lhs->type()==IDENTIFIER)
                context.setValue(lhs->getIdentifierName(), right);
            else{
                exist=false;
            }
        }    //直接把右边num存入context里？每次取出的时候都进行计算
        return right;
    }
    int left = lhs->eval(context);
    if (op == "**") {
        exist=rhs->ifExist()&&lhs->ifExist();
        return qPow(left, right);//直接用qpower求乘方
    }
    if (op == "+") {
        exist=rhs->ifExist()&&lhs->ifExist();
        return left + right;
    }
    if (op == "-") {
        exist=rhs->ifExist()&&lhs->ifExist();
        return left - right;
    }
    if (op == "*") {
        exist=rhs->ifExist()&&lhs->ifExist();
        return left * right;}
    if (op == "/") {
        if (right == 0){
            exist=false;
            return 0;
        }
        exist=rhs->ifExist()&&lhs->ifExist();
        return left / right;
    }
    //   error("Illegal operator in expression");
    exist=false;
    return 0;
}
