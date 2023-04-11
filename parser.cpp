#include "parser.h"

parser::parser(){}

void parser::readString(QStringList stringlist){
    init();
    //开始编辑
    QString part;
    QStringList printf;
    while(!stringlist.empty()){
        part = stringlist.front();

        if(isPrintf){
            part=stringlist.join(" ");
            printf=part.split(',');
            part=printf.first();
        }

        if(part=="=")
            haveequal++;
        if(haveequal>1){
            good_statement=false;
            return;
        }
        stringlist.pop_front();
        bool flag=false;
        int i = part.toInt(&flag,10);
        if(part=="+"||part=="-"||part=="*"||part=="/"||part=="("||part==")"||part=="="||part=="**"){
            op_flag=true;
            if(part==")"){
                while(operators.top()!="("){
                    if(operands.size()>2)
                        pushCom();
                    else{
                        good_statement=false;
                        return;
                    }                }
                if(operators.top()=="(")
                    operators.pop();
            }
            else if(part=="**"){
                while(!operators.isEmpty()&&operators.top()=="**")
                    if(operands.size()>2)
                        pushCom();
                    else{
                        good_statement=false;
                        return;
                    }
                operators.push(part);
            }
            else if(part=="*"||part=="/"){
                while(!operators.isEmpty()&&(operators.top()=="*"||operators.top()=="/"||operators.top()=="**")){
                    if(operands.size()>2)
                        pushCom();
                    else{
                        good_statement=false;
                        return;
                    }                }
                operators.push(part);
            }

            else if(part=="+"||part=="-"){
                //如果先遇到高优先级的符号
                while(!operators.isEmpty()&&operators.top()!="="&&operators.top()!="("){
                    if(operands.size()>2)
                        pushCom();
                    else{
                        good_statement=false;
                        return;
                    }
                }
                operators.push(part);
            }
            //compound
            else
                operators.push(part);
        }
        else if(flag){
            //是数字
            //constant
            ConstantExp* consNode=new ConstantExp(i);
            operands.push(consNode);
        }
        else if(isPrintf){
            QStringList tmpPrintf=printf;
            QString tmp;
            int countDan=part.count("\'");
            int countShuang=part.count("\"");
            tmpPrintf.pop_front();
            if((countDan==1&&countShuang==0)||(countDan==0&&countShuang==1)){
                printf.pop_front();
                while(!tmpPrintf.isEmpty()){
                    tmp=tmpPrintf.first();
                    if(tmp.contains("\'")||tmp.contains("\"")){
                        part=part+","+tmp;
                        tmpPrintf.pop_front();
                        printf.pop_front();
                        printf.push_front(part);
                        tmpPrintf.clear();
                    }
                    else {
                        part=part+","+tmp;
                        tmpPrintf.pop_front();
                        printf.pop_front();
                    }
                }
            }
            stringlist.clear();
            printf_check(printf);
            isPrintf=false;
        }
        else if(part.contains('\'')||part.contains('\"')){
            haveXiaoYinhao=part.count('\'');
            haveDaYinhao=part.count('\"');
            if(haveDaYinhao+haveXiaoYinhao>2||(haveDaYinhao==1&&haveXiaoYinhao==1)){
                //error 跳过这个循环 执行下个语句？
                good_statement=false;
                return;
            }
            part.replace("\'","");
            part.replace("\"","");
            //因为不参加表达式计算，可以直接退出
            isString=true;
            nowStrName=operands.pop()->getIdentifierName();
            good_statement=true;
            stringTable.insert(nowStrName,part);
            operands.clear();
            operators.clear();
            return;
        }
        else{
            //是名称
            //identity
            IdentifierExp* idenNode=new IdentifierExp(part);
            operands.push(idenNode);
            if(stringTable.contains(part)){
                isString=true;
            }
        }
    }
    while(!operators.empty()){
        if(operators.size()==1&&(haveequal==1||haveequal==0)){
            rhs=operands.pop();
            lhs=operands.pop();
            CompoundExp* compNode = new CompoundExp(operators.pop(),lhs,rhs);
            equal=compNode;
            return;
        }
        if(operands.size()>2)
            pushCom();
        //op全都出栈
        else{
            if(operands.size()==1&&operators.size()>0){
                //这是错的
                good_statement=false;
                return;
            }
        }
    }
    if(operators.empty()&&operands.size()==1){  //只有一个内容
        equal=operands.pop();
        op_flag=false;  //没有operator
    }
    //等于号左边写两个东西 会被合成成一个
    //有两个以上的等于号需要报错
    //有连续的符号（除了是（），但他们进不了这个阶段）需要报错
    return;
}

void parser::printf_check(QStringList stringlist){
    //开始编辑
    QString part;
    part = stringlist.front();
    stringlist.pop_front();
    if(part.contains('\'')||part.contains('\"')){
        haveXiaoYinhao=part.count('\'');
        haveDaYinhao=part.count('\"');
        if(haveDaYinhao+haveXiaoYinhao>2||haveDaYinhao==1||haveXiaoYinhao==1){
            //error 跳过这个循环 执行下个语句？
            good_statement=false;
            return;
        }
        //因为不参加表达式计算，可以直接退出

            //单独处理第一个
        part.replace("\'","");
        part.replace("\"","");
        int haveZuoKuohao=part.count('{');
        int haveYouKuohao=part.count('}');

        haveKuohao=part.count("{}");
        if(haveKuohao!=stringlist.size()||haveZuoKuohao!=haveYouKuohao){
            good_statement=false;
            return;
        }
        if(part.contains("{}")){
            printf_replace(stringlist);
            while (!replaceString.isEmpty()) {
                QString str=replaceString.front();
                replaceString.pop_front();
                part.replace(part.indexOf("{}"),2,str);

            }
        }

        isString=true;
        good_statement=true;
        operands.clear();
        operators.clear();
        PrintfResult=part;
        return;
    }
    else{
        good_statement=false;
        return;
    }
}
void parser::printf_replace(QStringList stringlist){
    QString part;
    while(!stringlist.empty()){
        part = stringlist.front();
        stringlist.pop_front();
        bool flag=false;
        int i = part.toInt(&flag,10);
        part.replace(QRegExp("^ "), "");
        if(flag){
            //是数字
            replaceString.push_back(part);
        }
        else if(part.contains('\'')||part.contains('\"')){
            haveXiaoYinhao=part.count('\'');
            haveDaYinhao=part.count('\"');
            if(haveDaYinhao+haveXiaoYinhao>2||(haveDaYinhao==1&&haveXiaoYinhao==1)){
                //error 跳过这个循环 执行下个语句？
                good_statement=false;
                return;
            }
            part.replace("\'","");
            part.replace("\"","");
            //因为不参加表达式计算，可以直接退出
            replaceString.push_back(part);
            isString=true;
            good_statement=true;
        }
        else{
            //是名称
            //identity
            QString strValue;
            int value;
            if(stringTable.contains(part))
                strValue=stringTable.find(part).value();

            else if(context.symbolTable.contains(part)){
                value=context.symbolTable.find(part).value();
                strValue.setNum(value,10);
            }
            else{
                good_statement=false;
                return;
            }
            replaceString.push_back(strValue);
        }
    }
    return;
}

void parser::pushCom(){
    rhs=operands.pop();
    lhs=operands.pop();
    CompoundExp* compNode = new CompoundExp(operators.pop(),lhs,rhs);
    operands.push(compNode);
}

bool parser::check_correct(Expression* exp){
    if(!exp)
        return false;
    if(exp->type()==COMPOUND){
        return(check_correct(exp->getLHS())&&check_correct(exp->getLHS()));
    }
    else if(exp->type()==CONSTANT){
        return true;
    }
    else if(exp->type()==IDENTIFIER){
        return true;
    }
    else
        return false;
}


bool parser::check_exist(Expression* exp){
    if(exp->type()==COMPOUND){
        return(check_exist(exp->getLHS())&&check_exist(exp->getLHS()));
    }
    else if(exp->type()==CONSTANT){
        return exp->ifExist();
    }
    else if(exp->type()==IDENTIFIER){
        return exp->ifExist();
    }
    else
        return false;
}

int parser::evaluation(Expression *&equal){
    int i;
    good_statement=true;
    if(check_correct(equal)){
//        if(check_exist(equal)){
            if(op_flag){
                i = equal->eval(context);
                if(!equal->ifExist())
                    good_statement=false;
            }
            else{
                if(equal->type()==CONSTANT)
                    i=equal->getConstantValue();
                if(equal->type()==IDENTIFIER){
                    i=equal->eval(context);
                    if(!equal->ifExist())
                        good_statement=false;
                }
            }
        //}
    }
    else{
        good_statement=false;
        i=0;
    }
    return i;
}

void parser::init(){
    //初始化
    haveequal=0;
    haveDaYinhao=0;
    haveXiaoYinhao=0;
    haveKuohao=0;
    isString=false;

    operands.clear();
    operators.clear();
    //先不处理内训
    equal=nullptr;
    lhs=nullptr;
    rhs=nullptr;
}
