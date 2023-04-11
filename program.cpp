 #include "program.h"
#include <iostream>

program::program(){}

bool compare(const statement &A, const statement &B){
    return A.num<B.num;
}

void program::check_same(int num){
    QList<statement>::Iterator item = state.begin(),itemend = state.end();
    for (;item != itemend; item++){
        if(num==item->num)  state.erase(item);
    }
}

bool program::check_num(QString str){
    std::string temp = str.toStdString();
    for (int i = 0; i < str.length(); i++)
        if (temp[i]<'0' || temp[i]>'9')
            return false;
    return true;
}

void program::set_program_input(QString input){
    int value;
    statement tmp;
    input=input.simplified();
    if(check_num(input)){
        //是个数字
        tmp.stringlist=input.split(" ");
        tmp.num=tmp.stringlist.first().toInt();
        if(tmp.num!=0){
            check_same(tmp.num);
        }
    }
    else if(input.left(5)=="PRINT"){
        running_flag=true;

        input.push_front("0 ");
        tmp.read_input(input);
        tmp.split_statement();
        tmp.stringlist.pop_front();
        prog_print(tmp.stringlist,value);

        running_flag=false;
    }
    else if(input.left(3)=="LET"){
        running_flag=true;

        input.push_front("0 ");
        tmp.read_input(input);
        tmp.split_statement();
        tmp.stringlist.pop_front();
        prog_let(tmp.stringlist,value);

        running_flag=false;
    }
    else if(input.left(5)=="INPUT"){
        running_flag=true;

        input.push_front("0 ");
        tmp.read_input(input);
        tmp.split_statement();
        emit_input();
        tmp.stringlist.pop_front();
        prog_input(tmp.stringlist,value);

        running_flag=false;

    }
    else{
        tmp.read_input(input);
        if(!tmp.toint){
            emit_line_error();
            if(!error_line.contains(tmp.cursor))
                error_line.append(tmp.cursor);
            return;
        }
        tmp.split_statement();
        //如果他不是0 那就检查一下
        if(tmp.num!=0){
            check_same(tmp.num);
            state.append(tmp);
        }
    }
    qSort(state.begin(), state.end(), compare);
}

void program::set_program_file(QFile &file){
    //需要循环 就放在外面实现了
    statement tmp;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString Qline = in.readLine();
    Qline=Qline.simplified();

    while(!Qline.isNull()) {
        if(Qline.contains(" ")){
            Qline=Qline.simplified();
            tmp.stringlist=Qline.split(" ");
            tmp.num=tmp.stringlist.first().toInt();
            tmp.stringlist.pop_front();
            //如果他不是0 那就检查一下
            if(tmp.num!=0){
                check_same(tmp.num);
                tmp.get_op();
                tmp.split_statement();
                state.append(tmp);
            }
        }
        Qline=in.readLine();
    }

    qSort(state.begin(), state.end(), compare);
}

void program::run_gramTree(){
    QList<statement> tmplist=state;
    QList<statement> resultlist;
    statement tmpstate;
    while(tmplist.size()>0){
        tmpstate=tmplist.first();
        build_gramTree(tmpstate);
        resultlist.append(tmpstate);
        tmplist.pop_front();
    }
    state=resultlist;
    emit_gram();
}

void program::debug_allgramcheck(){
    QList<statement> tmplist=state;
    QList<statement> resultlist;
    statement tmpstate;
    while(tmplist.size()>0){
        tmpstate=tmplist.first();
        build_gramTree(tmpstate);
        resultlist.append(tmpstate);
        tmplist.pop_front();
    }
    gramTree.gram_list.clear();
    pars.context.symbolTable.clear();
    pars.stringTable.clear();
    state=resultlist;
}

void program::gramTree_debug(){
    if(undebugging_state.isEmpty())
        return;

    if(GoTo_flag){
        statement tmp=state.first();     //重置一下
        undebugging_state=state;
        while(!undebugging_state.isEmpty()){
            if(tmp.num==GoTo_num){
                GoTo_flag=false;    //执行过一次 结束
                break;    //调整到他们都相等之后跳出本次循环
            }
            else{
                undebugging_state.pop_front();
                tmp=undebugging_state.first();
            }
        }
    }
    if(GoTo_flag){  //循环之后还没改过
        if(!error_line.contains(GOTOcur))
            error_line.append(GOTOcur);
        emit_compile_error();
        GoTo_flag=false;
        running_flag=false;
        debugging_flag=false;
        return;
    }

    QList<statement> tmplist=undebugging_state;
    QList<statement> resultlist;

    statement tmpstate;
    tmpstate=tmplist.first();
    build_gramTree(tmpstate);
    resultlist.append(tmpstate);
    tmplist.pop_front();

    emit_gram();
}

void program::build_gramTree(statement& stalist){
    int value;
    QString strValue;
    int staop=stalist.oper;
    int line_num=stalist.num;
    bool run_this=stalist.excu;
    QStringList stringlist=stalist.stringlist;
    QStringList ifA,ifB;

    stringlist.pop_front();
    if(staop==REM||staop==IF||staop==GOTO||staop==END){
        pars.good_statement=true;
        gramTree.gram_build(pars,stringlist,line_num,staop,run_this);
    }
    else if(staop==LET){
        pars.readString(stringlist);
        if(pars.isString)
            strValue=pars.stringTable.find(pars.nowStrName).value();
        else
            value=pars.evaluation(pars.equal);

        gramTree.gram_build(pars,stringlist,line_num,staop,run_this);
    }
    else if(staop==INPUT||staop==INPUTS){
        QStringList tmp=stringlist;
        pars.input_var.append(tmp.front());
        gramTree.gram_build(pars,stringlist,line_num,staop,run_this);
    }
    else if(staop==PRINT){
        pars.readString(stringlist);
        if(pars.isString)
            strValue=pars.stringTable.find(pars.nowStrName).value();
        else
            value=pars.evaluation(pars.equal);
        gramTree.gram_build(pars,stringlist,line_num,staop,run_this);
    }
    else if(staop==PRINTF){
        pars.isPrintf=true;
        pars.readString(stringlist);
        printfResult=pars.PrintfResult;
        gramTree.gram_build(pars,stringlist,line_num,staop,run_this);
    }
    else{
        number=QString::number(line_num)+" Error";
        gramTree.gram_list.append(number);
        if(!error_line.contains(stalist.cursor))
            error_line.append(stalist.cursor);
        stalist.error=true;
    }
    stalist.excu=run_this;
    if(!stalist.excu){
        if(!error_line.contains(stalist.cursor))
            error_line.append(stalist.cursor);
        stalist.error=true;
    }
}

void program::run_program(){
    //读取每个语句并且执行直接输出。
    QList<statement>::Iterator tmp = state.begin();
    QList<statement>::Iterator item = state.begin(),itemend = state.end();
    for (;item != itemend; item++){
        if(running_flag){
            if(GoTo_flag){
                tmp=state.begin();     //重置一下
                for (;tmp != itemend; tmp++){
                    if(tmp->num==GoTo_num){
                        GoTo_flag=false;    //执行过一次 结束
                        item=tmp;     //结束的时候会++，提前调整
                        break;    //调整到他们都相等之后跳出本次循环
                    }
                }
            }
            if(GoTo_flag){  //循环之后还没改过
                if(!error_line.contains(GOTOcur))
                    error_line.append(GOTOcur);
                emit_compile_error();
                GoTo_flag=false;
                running_flag=false;
                debugging_flag=false;
                return;
            }
            if(item->oper==INPUT){
                emit_input();
            }
            if(item->oper==INPUTS){
                emit_inputs();
            }
            if(item->oper==END)
                break;
            else
                state_change_prog(item);
            varList_refresh();
        }
    }
    if(GoTo_flag){
        if(!error_line.contains(GOTOcur))
            error_line.append(GOTOcur);
        emit_compile_error();
        GoTo_flag=false;
        running_flag=false;
        debugging_flag=false;
        return;
    }
    //程序结束了，则程序不再继续运行。
    running_flag=false;
}

void program::rerun_gramTree(){
    QList<statement> tmplist=undebugging_state;
    QList<statement> resultlist;
    statement tmpstate;
    while(tmplist.size()>0){
        tmpstate=tmplist.first();
        build_gramTree(tmpstate);
        resultlist.append(tmpstate);
        tmplist.pop_front();
    }
    undebugging_state=resultlist;
    emit_gram();
}

void program::rerun_program(){
    //读取每个语句并且执行直接输出。
    QList<statement>::Iterator tmp = state.begin();
    QList<statement>::Iterator item = undebugging_state.begin(),itemend = undebugging_state.end();
    while (!undebugging_state.isEmpty()) {
        item=undebugging_state.begin();
        if(debugging_flag){
            if(GoTo_flag){
                tmp=state.begin();     //重置一下
                undebugging_state=state;
                for (;tmp != itemend; tmp++){
                    if(tmp->num==GoTo_num){
                        GoTo_flag=false;    //执行过一次 结束
                        item=tmp;     //结束的时候会++，提前调整
                        break;    //调整到他们都相等之后跳出本次循环
                    }
                    else{
                        undebugging_state.pop_front();
                    }
                }
            }
            if(GoTo_flag){  //循环之后还没改过
                if(!error_line.contains(GOTOcur))
                    error_line.append(GOTOcur);
                emit_compile_error();
                GoTo_flag=false;
                running_flag=false;
                debugging_flag=false;
                return;
            }
            if(item->oper==INPUT){
                emit_input();
            }
            if(item->oper==INPUTS){
                emit_inputs();
            }
            if(item->oper==END)
                break;
            else
                state_change_prog(item);
            varList_refresh();
            undebugging_state.pop_front();
        }
        else {
            return;
        }
    }
    if(GoTo_flag){
        if(!error_line.contains(GOTOcur))
            error_line.append(GOTOcur);
        emit_compile_error();
        GoTo_flag=false;
        running_flag=false;
        debugging_flag=false;
        return;
    }
    //程序结束了，则程序不再继续运行。
    debugging_flag=false;
}

void program::state_change_prog(QList<statement>::Iterator &sta){
    QString name;
    QString var;
    QString strValue;
    int value;
    int staop=sta->oper;
    int line_num=sta->num;
    QStringList stringlist=sta->stringlist;
    QStringList ifA,ifB;

    stringlist.pop_front();

    switch(staop){
    case REM :{
        //把注释发过去
        break;
    }
    case LET:{
        pars.readString(stringlist);
//        if(pars.isString)
//            strValue=pars.stringTable.find(pars.nowStrName).value();
//        else
//            value=pars.evaluation(pars.equal);
        if(!pars.good_statement){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
        }
        break;
    }
    case PRINT :{
        pars.readString(stringlist);
        if(pars.isString)
            number=pars.stringTable.find(pars.nowStrName).value();
        else{
            value=pars.evaluation(pars.equal);
            number.setNum(value,10);
        }
        if(!pars.good_statement){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
        }
        if(pars.good_statement)
            emit_print();
        break;
    }
    case INPUT:
        input_val=stringlist.front();
        input_num++;
        input_var[input_num]=input_val;
        value=number.toInt();
        pars.context.setValue(input_var[input_num],value);
        break;
    case GOTO:
    {   //如果用map goto很好实现
        GOTOcur=sta->cursor;
        GoTo_flag=true;
        var=stringlist.front();
        GoTo_num=var.toInt();   //保存了跳转的地址
    }
        break;
    case IF:{
        if(!stringlist.contains("THEN")){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
            return;
        }
        var=stringlist.front();
        while (var!=">"&&var!="<"&&var!="=") {
            ifA.append(var);
            stringlist.pop_front();
            if(stringlist.isEmpty()){
                emit_compile_error();
                running_flag=false;
                debugging_flag=false;
                return;
            }
            var=stringlist.front();
        }
        pars.readString(ifA);
        value = pars.evaluation(pars.equal);

        op=stringlist.front();//保存一下符号
        if(stringlist.isEmpty()){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
            return;
        }
        stringlist.pop_front();

        while(stringlist.front()!="THEN"){
            if(stringlist.isEmpty()){
                emit_compile_error();
                running_flag=false;
                debugging_flag=false;
                return;
            }
            ifB.append(stringlist.front());
            stringlist.pop_front();
        }

        if(stringlist.isEmpty()){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
            return;
        }
        pars.readString(ifB);
        stringlist.pop_front();

        if(op==">"){
            if(value>pars.evaluation(pars.equal))
            {
                GoTo_flag=true;
                if(stringlist.isEmpty()){
                    emit_compile_error();
                    running_flag=false;
                    debugging_flag=false;
                    return;
                }
                var=stringlist.front();
                GoTo_num=var.toInt();   //保存了跳转的地址
             }
            else
                break;
        }
        else if(op=="<"){
            if(value<pars.evaluation(pars.equal))
            {
                GoTo_flag=true;
                if(stringlist.isEmpty()){
                    emit_compile_error();
                    running_flag=false;
                    debugging_flag=false;
                    return;
                }
                var=stringlist.front();
                GoTo_num=var.toInt();   //保存了跳转的地址
             }
            else
                break;
        }
        else if(op=="="){
            if(value==pars.evaluation(pars.equal))
            {
                GoTo_flag=true;
                if(stringlist.isEmpty()){
                    emit_compile_error();
                    running_flag=false;
                    debugging_flag=false;
                    return;
                }
                var=stringlist.front();
                GoTo_num=var.toInt();   //保存了跳转的地址
            }
            else
                break;
        }
        break;
    }
    case INPUTS:{
        input_val=stringlist.front();
        input_num++;
        input_var[input_num]=input_val;
        pars.stringTable.insert(input_val,number);
        break;
    }
    case PRINTF:{
        pars.isPrintf=true;
        pars.readString(stringlist);
        printfResult=pars.PrintfResult;

        if(!pars.good_statement){
            emit_compile_error();
            running_flag=false;
            debugging_flag=false;
        }
        if(pars.good_statement)
            emit_printf();
        break;
    }
    default:{
        number=line_num;
        emit_compile_error();
        debugging_flag=false;
        running_flag=false;
    }
        break;
        }
    }

void program::run_debug(){
    //读取每个语句并且执行直接输出。
    if(undebugging_state.isEmpty()){
        debugging_flag=false;
        return;
    }
    QList<statement>::Iterator tmp = state.begin(),itemend=state.end();
    QList<statement>::Iterator item = undebugging_state.begin();
        if(running_flag){

            if(item->oper==INPUT){
                emit_input();
            }
            if(item->oper==INPUTS){
                emit_inputs();
            }
            if(item->oper==END)
                return;
            else
                state_change_prog(item);
            varList_refresh();
        }
    //程序结束了，则程序不再继续运行。
    undebugging_state.pop_front();
}

void program::varList_refresh(){
    QString tmpStr,num;
    QMap<QString, QString>::const_iterator i;
    for (i = pars.stringTable.begin(); i != pars.stringTable.end(); ++i) {
        tmpStr=i.key()+": STR = "+i.value();
        varList.append(tmpStr);
    }
    QMap<QString, int>::const_iterator j;
    for (j = pars.context.symbolTable.begin(); j != pars.context.symbolTable.end(); ++j) {
        num=QString::number(j.value());
        tmpStr=j.key()+": INT = "+num;
        varList.append(tmpStr);
    }
    emit_varList();
}

void program::prog_let(QStringList stringlist,int value){
    QString strValue;
    pars.readString(stringlist);
    if(pars.isString)
        strValue=pars.stringTable.find(pars.nowStrName).value();
    else
        value=pars.evaluation(pars.equal);
    if(!pars.good_statement){
        emit_compile_error();
        running_flag=false;
        debugging_flag=false;
    }
}

void program::prog_print(QStringList stringlist, int value){
    pars.readString(stringlist);
    if(pars.isString)
        number=pars.stringTable.find(pars.nowStrName).value();
    else{
        value=pars.evaluation(pars.equal);
        number.setNum(value,10);
    }
    if(!pars.good_statement){
        emit_compile_error();
        running_flag=false;
        debugging_flag=false;
    }
    if(pars.good_statement)
        emit_print();
}

void program::prog_input(QStringList stringlist, int value){
    input_val=stringlist.front();
    value=number.toInt();
    pars.context.setValue(input_val,value);
}
