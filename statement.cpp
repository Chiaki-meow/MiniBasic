#include "statement.h"


void statement::read_input(QString input){
    //检测语句是否含有  若含有 才可以进行下一步检查
    if(input.contains(" ")){
        stringlist=input.split(" ");
        num=stringlist.first().toInt(&toint);
        stringlist.pop_front();
        //把第一位数字pop出去
        get_op();   //顺便修改一下op
    }
    else{
        excu=false;
        toint=false;
    }
}

void statement::split_statement(){
    QStringList tmpList=stringlist;
    QStringList returnList;
    QString now,tmp,last;

    QStringList ifA,ifB;

    bool first_flag=true;

    returnList.append(stringlist.front());  //把功能输入
    QChar *itmp;
    tmpList.pop_front();    //pop掉功能

    if(tmpList.isEmpty()&&oper!=END){
        excu=false;
        return;
    }

    if(oper!=REM&&oper!=GOTO&&oper!=INPUT&&oper!=IF&&oper!=END&&oper!=INPUTS&&oper!=PRINTF){
        now=tmpList.join("");    //把他们连接起来一起处理
        for (QChar *it=now.begin(); it!=now.end(); ++it) {
            if(*it!='+'&&*it!='-'&&*it!='*'&&*it!='/'&&*it!='='&&*it!='('&&*it!=')')
            {
                tmp.append(*it);    //只要不是符号，就一直append
                last=tmp;
            }
            else{    //如果是符号或者是字符串
                //把变量/常量存进来
                if(*it=='-'){   //如果是-的话要考虑是不是负数的特殊处理
                    if(tmp.isEmpty()&&first_flag==true)   //如果tmp是空的 只有在-号并且他是第一个的时候push
                    {
                        tmp.append(*it);
                        last=tmp;
                    }
                    else if(last=="("||last=='='){
                        tmp.append(*it);
                    }
                    else{
                        if(!tmp.isEmpty()){
                            returnList.append(tmp);
                        }
                        tmp.clear();    //每次及时清空
                        tmp.append(*it);
                        last=tmp;
                        returnList.append(tmp);
                        tmp.clear();    //每次及时清空
                    }
                }
                else{
                    if(!tmp.isEmpty()){
                        returnList.append(tmp);
                    }
                    tmp.clear();    //每次及时清空

                    tmp.append(*it);
                    //检查符号 尤其是幂乘
                    itmp=it+1;
                    if(*it=='*'&&*itmp=='*'){
                        tmp.append('*');
                        it++;}
                    last=tmp;

                    returnList.append(tmp);
                    tmp.clear();    //每次及时清空
                }
                //如果是四个*放在一起的话可能需要报错
            }
            first_flag=false;
        }
        if(!tmp.isEmpty()){
            last=tmp;
            returnList.append(tmp);
        }
        stringlist=returnList;

    }
    else if(oper==IF){
        QString op_if;
        QStringList sec;
        if(tmpList.isEmpty()){
            return;
        }
        tmp=tmpList.front();
        while (tmp!=">"&&tmp!="<"&&tmp!="=") {
            if(!tmp.contains('>')&&!tmp.contains('<')&&!tmp.contains('=')){
                ifA.append(tmp);
                tmpList.pop_front();
                if(tmpList.isEmpty()){
                    break;
                }
                tmp=tmpList.front();
                op_if=tmp;
            }
            else {
                if(tmp.contains('<')){
                    tmpList.pop_front();
                    sec=tmp.split("<");
                    ifA.append(sec.front());
                    tmpList=sec+tmpList;
                    op_if="<";
                    if(tmpList.isEmpty()){
                        break;
                    }
                    tmp=tmpList.front();
                    break;
                }
                else if(tmp.contains('=')){
                    tmpList.pop_front();
                    sec=tmp.split("=");
                    ifA.append(sec.front());
                    tmpList=sec+tmpList;
                    op_if="=";
                    if(tmpList.isEmpty()){
                        break;
                    }
                    tmp=tmpList.front();
                    break;
                }
                else if(tmp.contains('>')){
                    tmpList.pop_front();
                    sec=tmp.split(">");
                    ifA.append(sec.front());
                    tmpList=sec+tmpList;
                    op_if=">";
                    if(tmpList.isEmpty()){
                        break;
                    }
                    tmp=tmpList.front();
                    break;
                }
            }
        }
        if(tmpList.isEmpty()){
            return;
        }
        tmpList.pop_front();    //pop掉第一个
        tmp.clear();

        now=ifA.join("");    //把他们连接起来一起处理
        for (QChar *it=now.begin(); it!=now.end(); ++it) {
            if(*it!='+'&&*it!='-'&&*it!='*'&&*it!='/'&&*it!='='&&*it!='('&&*it!=')')
            {
                tmp.append(*it);    //只要不是符号，就一直append
                last=tmp;
            }
            else{    //如果是符号
                //把变量/常量存进来
                if(*it=='-'){   //如果是-的话要考虑是不是负数的特殊处理
                    if(tmp.isEmpty()&&first_flag==true)   //如果tmp是空的 只有在-号并且他是第一个的时候push
                    {
                        tmp.append(*it);
                        last=tmp;
                    }
                    else if(last=="("||last=='='){
                        tmp.append(*it);
                    }
                    else{
                        if(!tmp.isEmpty()){
                            returnList.append(tmp);
                        }
                        tmp.clear();    //每次及时清空
                        tmp.append(*it);
                        last=tmp;
                        returnList.append(tmp);
                        tmp.clear();    //每次及时清空
                    }
                }
                else{
                    if(!tmp.isEmpty()){
                        returnList.append(tmp);
                    }
                    tmp.clear();    //每次及时清空

                    tmp.append(*it);
                    //检查符号 尤其是幂乘
                    itmp=it+1;
                    if(*it=='*'&&*itmp=='*'){
                        tmp.append('*');
                        it++;}
                    last=tmp;

                    returnList.append(tmp);
                    if(*it!='('&&*it!=')')
                    tmp.clear();    //每次及时清空
                }
                //如果是四个*放在一起的话可能需要报错
            }
            first_flag=false;
        }
        if(!tmp.isEmpty()){
            last=tmp;
            returnList.append(tmp);
        }
        returnList.append(op_if);
        if(tmpList.isEmpty())
            return;

        while(tmpList.front()!="THEN"){
            ifB.append(tmpList.front());
            tmpList.pop_front();
        }

        tmp.clear();

        now=ifB.join("");    //把他们连接起来一起处理
        for (QChar *it=now.begin(); it!=now.end(); ++it) {
            if(*it!='+'&&*it!='-'&&*it!='*'&&*it!='/'&&*it!='='&&*it!='('&&*it!=')')
            {
                tmp.append(*it);    //只要不是符号，就一直append
                last=tmp;
            }
            else{    //如果是符号
                //把变量/常量存进来
                if(*it=='-'){   //如果是-的话要考虑是不是负数的特殊处理
                    if(tmp.isEmpty()&&first_flag==true)   //如果tmp是空的 只有在-号并且他是第一个的时候push
                    {
                        tmp.append(*it);
                        last=tmp;
                    }
                    else if(last=="("||last=='='){
                        tmp.append(*it);
                    }
                    else{
                        if(!tmp.isEmpty()){
                            returnList.append(tmp);
                        }
                        tmp.clear();    //每次及时清空
                        tmp.append(*it);
                        last=tmp;
                        returnList.append(tmp);
                        tmp.clear();    //每次及时清空
                    }
                }
                else{
                    if(!tmp.isEmpty()){
                        returnList.append(tmp);
                    }
                    tmp.clear();    //每次及时清空

                    tmp.append(*it);
                    //检查符号 尤其是幂乘
                    itmp=it+1;
                    if(*it=='*'&&*itmp=='*'){
                        tmp.append('*');
                        it++;}
                    last=tmp;

                    returnList.append(tmp);
                    if(*it!='('&&*it!=')')
                    tmp.clear();    //每次及时清空
                }
                //如果是四个*放在一起的话可能需要报错
            }
            first_flag=false;
        }
        if(!tmp.isEmpty()){
            last=tmp;
            returnList.append(tmp);
        }

        returnList.append(tmpList);

        stringlist=returnList;
    }
}

//void statement::read_file(QFile &file){
//    //你没什么用了（）
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QTextStream in(&file);
//    QString Qline = in.readLine();

//    while(!Qline.isNull()) {
//        if(Qline.contains(" ")){
//            stringlist=Qline.split(" ");
//            num=stringlist.first().toInt();
//            stringlist.pop_front();
//        }
//        Qline=in.readLine();
//    }
//    get_op();
//}

void statement::get_op(){
    QString op=stringlist.front();

    oper=-1;
    if(op=="REM"){
        oper=REM;
    }
    if(op=="LET"){
        oper=LET;
    }
    if(op=="PRINT"){
        oper=PRINT;

    }
    if(op=="INPUT"){
        oper=INPUT;

    }
    if(op=="GOTO"){
        oper=GOTO;

    }
    if(op=="IF"){
        oper=IF;

    }
    if(op=="END"){
        oper=END;

    }
    if(op=="INPUTS"){
        oper=INPUTS;
    }
    if(op=="PRINTF"){
        oper=PRINTF;

    }
    }


