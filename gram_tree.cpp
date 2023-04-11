#include "gram_tree.h"

gramTree::gramTree()
{

}

void gramTree::error_report(QString line_num){
    QString tmp=line_num+" "+error;
    //可以在这里添加一个标志，让他存进去彩色的？
    gram_list.append(tmp);
}

void gramTree::gram_build(parser pars, QStringList strList, int lNum, int oper, bool &run_this){
    int pedding_length=0;
    QString tmp, line_num;
    QStringList tmplist;
    run_this=true;
    //使用tmplist先保存，遇到错误信息直接输出错误信息，没有错误在append tmplist
    line_num=QString::number(lNum,10);
    pars_gram=pars;

    if((strList.isEmpty()&&oper!=END)||(!pars.good_statement)){
        error_report(line_num);
        run_this=false;
        return;
    }
    if(oper==LET){
        //如果你是个str，那你会特殊一点点
        if(pars.isString){
            tmp=line_num+" LET =";
            tmplist.append(tmp);
            tmp="   "+pars.nowStrName;
            tmplist.append(tmp);
            tmp="   "+pars.stringTable.find(pars.nowStrName).value();
            tmplist.append(tmp);
            gram_list.append(tmplist);
            return;
        }
        //如果没有等于号 parser直接报错
        if(!strList.contains("=")){
            error_report(line_num);
            run_this=false;
            return;
        }
        tmp=line_num+" LET =";
        tmplist.append(tmp);
        Expression *left = pars.equal->getLHS();
        //如果没有左儿子 直接报错
        if(!left){
            error_report(line_num);
            run_this=false;
            return;
        }
        int nowValue;
        bool have_value=false;
        if(left->type()==IDENTIFIER){
            tmp=left->getIdentifierName();
            if(pars.context.symbolTable.contains(left->getIdentifierName()))
                have_value=true;
                nowValue=pars.context.getValue(left->getIdentifierName());
        }
        tmp="  "+tmp;
        tmplist.append(tmp);
        gram_buildTree(pars,pars.equal->getRHS(),pedding_length,tmplist);
        if(have_value){
            pars.context.symbolTable.insert(left->getIdentifierName(),nowValue);
        }
        if(!correct_flag){
            error_report(line_num);
            run_this=false;
            return;
         }
         gram_list.append(tmplist);
    }
    else if(oper==IF){
        bool int_flag=false;
        tmp=line_num+" IF THEN";
        tmplist.append(tmp);
        tmp=""; //clear
        if(!strList.contains("THEN")){
            error_report(line_num);
            run_this=false;
            return;
        }
        while(strList.front()!=">"&&strList.front()!="<"&&strList.front()!="="){
            tmp=tmp+strList.front()+" ";
            strList.pop_front();
            if(strList.isEmpty()){
                error_report(line_num);
                run_this=false;
                return;
            }
        }
        tmp.toInt(&int_flag,10);
        tmp.remove(" ");
        if(!int_flag&&!pars.context.symbolTable.contains(tmp)&&!pars.stringTable.contains(tmp)&&!pars.input_var.contains(tmp)){
            error_report(line_num);
            run_this=false;
            return;
        }
        line="   "+tmp;
        //存符号
        tmp="   "+strList.front();
        strList.pop_front();
        tmplist.append(tmp);
        tmplist.append(line);
        //存另一半
        tmp="";
        while(strList.front()!="THEN"){
            tmp=tmp+strList.front()+" ";
            strList.pop_front();
            if(strList.isEmpty()){
                error_report(line_num);
                run_this=false;
                return;
            }
        }
        tmp.toInt(&int_flag,10);
        tmp.remove(" ");
        if(!int_flag&&!pars.context.symbolTable.contains(tmp)&&!pars.stringTable.contains(tmp)&&!pars.input_var.contains(tmp)){
            error_report(line_num);
            run_this=false;
            return;
        }
        tmp="   "+tmp;
        tmplist.append(tmp);
        strList.removeOne("THEN");
        tmp="   "+strList.front();

        strList.front().toInt(&int_flag,10);
        if(!int_flag){
            error_report(line_num);
            run_this=false;
            return;
        }
        tmplist.append(tmp);  //最后的行号
        gram_list.append(tmplist);
    }
    else if(oper==GOTO){
        bool int_flag=false;
        tmp=line_num+" GOTO";
        tmplist.append(tmp);
        tmp="   "+strList.front();
        strList.front().toInt(&int_flag,10);
        if(!int_flag){
            error_report(line_num);
            run_this=false;
            return;
        }
        tmplist.append(tmp);
        gram_list.append(tmplist);
    }
    else if(oper==PRINT){
        tmp=line_num+" PRINT";
        tmplist.append(tmp);
        gram_buildTree(pars,pars.equal,pedding_length,tmplist);
        if(!correct_flag){
            error_report(line_num);
            run_this=false;
            return;
         }
        gram_list.append(tmplist);

    }
    else if(oper==8){
        tmp=line_num+" PRINTF";
        tmplist.append(tmp);
        gram_buildTree(pars,pars.equal,pedding_length,tmplist);
        if(!correct_flag){
            error_report(line_num);
            run_this=false;
            return;
         }
        gram_list.append(tmplist);
    }
    else if(oper==REM){
        tmp=line_num+" REM";
        gram_list.append(tmp);

        tmp="   "+strList.join(" ");
        gram_list.append(tmp);
    }
    else if(oper==INPUT){
        tmp=line_num+" INPUT";
        gram_list.append(tmp);
        tmp="   "+strList.front();
        gram_list.append(tmp);
    }
    else if(oper==INPUTS){
        tmp=line_num+" INPUTS";
        gram_list.append(tmp);
        tmp="   "+strList.front();
        gram_list.append(tmp);
    }
    else if(oper==END){
        tmp=line_num+" END";
        gram_list.append(tmp);
    }
}

void gramTree::gram_buildTree(parser pars,Expression* exp,int pedding_length,QStringList& tmplist){
    if(exp){
        if(pars.good_statement==false){
            correct_flag=false;
            return;
        }
        //处理自己
        pedding_length++;
        if(exp->type()==COMPOUND){
            line=exp->getOperator();
            for(int i=0;i<pedding_length;i++)
                line="  "+line;
            tmplist.append(line);
            gram_buildTree(pars,exp->getLHS(),pedding_length,tmplist);
            gram_buildTree(pars,exp->getRHS(),pedding_length,tmplist);
        }
        else if(exp->type()==CONSTANT){
            line=QString::number(exp->getConstantValue(),10);
            for(int i=0;i<pedding_length;i++)
                line="  "+line;
            tmplist.append(line);
        }
        else if(exp->type()==IDENTIFIER){
            line=exp->getIdentifierName();
            for(int i=0;i<pedding_length;i++)
                line="  "+line;
            tmplist.append(line);

        }
    }
}
