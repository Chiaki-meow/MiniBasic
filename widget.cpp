#include "widget.h"
#include "ui_widget.h"

#include <QLineEdit>
#include <QDir>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

QEventLoop loop;

bool compare(const line &A, const line &B){
    return A.num<B.num;
}


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle((tr("Basic Mini")));

    connect(&program,SIGNAL(sig_input()),this,SLOT(input_num()));
    connect(&program,SIGNAL(sig_inputs()),this,SLOT(input_num()));
    connect(&program,SIGNAL(sig_print()),this, SLOT(print()));
    connect(&program,SIGNAL(sig_printf()),this, SLOT(printf()));
    connect(&program,SIGNAL(sig_gram_tree_print()),this, SLOT(gram_print()));
    connect(&program,SIGNAL(sig_compile_error()),this, SLOT(compile_error()));
    connect(&program,SIGNAL(sig_line_error()),this, SLOT(line_error()));
    connect(&program,SIGNAL(sig_varList()),this, SLOT(varList()));

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_Button_load_clicked()
{
    program.state.clear();
    ui->textBrowser_code->clear();
    ui->textBrowser_result->clear();
    ui->textBrowser_tree->clear();
    ui->textBrowser_var->clear();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home"/*,
                                                    tr("Text files (*.txt)")*/);
    QFile file(fileName);
    load(file);
}

void Widget::on_Button_run_clicked()
{
    if(!program.debugging_flag)
        run();
    else{
        program.gramTree.gram_list.clear();
        program.rerun_gramTree();
        program.rerun_program();
        bug_light();

        if(program.debugging_flag==false){
            if(program.undebugging_state.isEmpty()){
                QMessageBox::information(this,"Success","被调试的程序正常结束");
                ui->Button_load->setEnabled(true);
                ui->Button_clear->setEnabled(true);
                program.running_flag=false;
                return;
            }
            else {
                QMessageBox::information(this,"Error","该语句有错误");
                program.debugging_flag=false;
                debug_error=false;
                ui->Button_load->setEnabled(true);
                ui->Button_clear->setEnabled(true);
                program.running_flag=false;
                return;
            }
        }
    }

}

void Widget::on_Button_clear_clicked()
{
    program.state.clear();
    program.gramTree.gram_list.clear();
    ui->textBrowser_code->clear();
    ui->textBrowser_result->clear();
    ui->textBrowser_tree->clear();
    ui->textBrowser_var->clear();
}

void Widget::on_Button_debug_clicked()
{
    debug_run();
}

void Widget::prog_show(){
    //显示输入的代码
    QList<statement>::Iterator item = program.state.begin(),itemend = program.state.end();
    program.cur=0;
    for (;item != itemend; item++){
        QString in1=QString::number(item->num)+" ";
        in1+=item->stringlist.join(" ");
        ui->textBrowser_code->append(in1);

        item->cursor=program.cur;
        program.cur+=in1.length()+1;    //加入换行符
    }
}

void Widget::read(){
    //识别输入者输入的代码
    //还没有规范输入
    program.gramTree.gram_list.clear();

    ui->textBrowser_code->clear();
    input=ui->lineEdit_input->text();
    ui->lineEdit_input->clear();
    if(input=="RUN")
        run();
    else if(input=="LOAD")
        on_Button_load_clicked();
    else if(input=="LIST")
        prog_show();
    else if(input=="CLEAR")
        on_Button_clear_clicked();
    else if(input=="HELP")
        help();
    else if(input=="QUIT")
        quit();
    else if(input=="")
        return;
    else
        program.set_program_input(input);


    //输出所输入的代码
    prog_show();
}

void Widget::load(QFile &file){
    //识别输入者输入的代码
    //还没有规范输入
    program.set_program_file(file);
    //输出所输入的代码
    prog_show();
    ui->lineEdit_input->clear();
}

void Widget::input_wait(){
    ui->lineEdit_input->textEdited(input);
}

void Widget::run(){
    program.running_flag=true;
    //新增
    program.pars.context.symbolTable.clear();
    program.pars.stringTable.clear();

    program.gramTree.gram_list.clear();
    ui->textBrowser_result->clear();
    ui->textBrowser_tree->clear();
    ui->textBrowser_var->clear();

    program.run_gramTree();
    bug_light();
    program.run_program();
    bug_light();

    program.running_flag=false;
}

void Widget::debug_run(){
    if(program.debugging_flag==false){
        program.error_line.clear();

        program.pars.context.symbolTable.clear();
        program.pars.stringTable.clear();

        ui->Button_load->setEnabled(false);
        ui->Button_clear->setEnabled(false);
        program.gramTree.gram_list.clear();
        ui->textBrowser_result->clear();
        ui->textBrowser_var->clear();

        program.debugging_flag=true;
        program.running_flag=true;
        program.debug_allgramcheck();

        program.undebugging_state=program.state;

        green_light();
    }
    else{
        program.GoTo_flag=Goto;
        program.GoTo_num=GotoNum;
        program.gramTree.gram_list.clear();
        program.gramTree_debug();

        program.run_debug();
        Goto=program.GoTo_flag;
        GotoNum=program.GoTo_num;
        green_light();

        if(program.debugging_flag==false){
            QMessageBox::information(this,"Success","被调试的程序正常结束");
            ui->Button_load->setEnabled(true);
            ui->Button_clear->setEnabled(true);
            program.running_flag=false;
            ui->textBrowser_tree->clear();
            return;
        }
        if(debug_error){
            QMessageBox::information(this,"Error","该语句有错误");
            program.debugging_flag=false;
            debug_error=false;
            ui->Button_load->setEnabled(true);
            ui->Button_clear->setEnabled(true);
            program.running_flag=false;
            ui->textBrowser_tree->clear();
            return;
        }
    }
}

void Widget::on_lineEdit_input_returnPressed()
{
    if(program.running_flag==false)
        read();
    else{
        input=ui->lineEdit_input->text();
        loop.exit();
    }
}

void Widget::input_num(){
    ui->lineEdit_input->setPlaceholderText("???");
    ui->Button_run->setEnabled(false);
    ui->Button_debug->setEnabled(false);
    //事件循环
    connect(this, SIGNAL(on_lineEdit_input_returnPressed()), &loop, SLOT(quit()));  //成功后会有返回响应
    loop.exec();  //设置等待,等到成功改变input，则退出等待
    //看看enter有没有按下来
    program.number=input;
    ui->lineEdit_input->clear();
    ui->lineEdit_input->setPlaceholderText("");
    ui->Button_run->setEnabled(true);
    ui->Button_debug->setEnabled(true);
}

void Widget::keyPressEvent(QKeyEvent *event){
    input=ui->lineEdit_input->text();
    program.number=input;
}

void Widget::print(){
    ui->textBrowser_result->append(program.number);
}

void Widget::printf(){
    ui->textBrowser_result->append(program.printfResult);
}


void Widget::help(){
    QFile file("help.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString help=in.readLine();

    while(!help.isNull()) {
        ui->textBrowser_result->append(help);
        help=in.readLine();
    }
}

void Widget::quit(){
    this->close();
}

void Widget::let(){
}

void Widget::gram_print(){
    ui->textBrowser_tree->clear();

    QString line;
    QStringList tree=program.gramTree.gram_list;
    while(!tree.empty()){
        line=tree.front();
        ui->textBrowser_tree->append(line);
        tree.pop_front();
    }
}

void Widget::compile_error(){
    ui->textBrowser_result->append("Compile Error");
}

void Widget::line_error(){
    ui->textBrowser_result->append("Line Error");
}

void Widget::varList(){
    ui->textBrowser_var->clear();
    QString tmp;
    while(!program.varList.isEmpty()){
        tmp=program.varList.front();
        ui->textBrowser_var->append(tmp);
        program.varList.pop_front();
    }
}

void Widget::bug_light(){
    QTextBrowser *code = ui->textBrowser_code;
    QTextCursor cursor(code->document());

    QList<QTextEdit::ExtraSelection> extras;
    QList<int> tmpError=program.error_line;

    while(!tmpError.isEmpty()){
        highlights.append({tmpError.first(), QColor(255, 100, 100)});
        tmpError.pop_front();
    }

    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
    QTextEdit::ExtraSelection h;
    h.cursor = cursor;
    // 下面这些的功能，请大家自行查看文档
    h.cursor.setPosition(line.first);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(line.second);
    extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);
    program.error_line.clear();
    highlights.clear();
}

void Widget::green_light(){
    QTextBrowser *code = ui->textBrowser_code;
    QTextCursor cursor(code->document());

    QList<QTextEdit::ExtraSelection> extras;
    QList<statement> tmpState=program.undebugging_state;
    QList<int> tmpError=program.error_line;

    while(!tmpError.isEmpty()){
        if(!tmpState.isEmpty()&&tmpState.first().cursor==tmpError.first()){
            debug_error=true;
        }
        highlights.append({tmpError.first(), QColor(255, 100, 100)});
        tmpError.pop_front();
    }

    if(!tmpState.isEmpty()){
        //只要第一个就好了
        highlights.append({tmpState.first().cursor, QColor(100, 255, 100)});
        tmpState.pop_front();
    }

    if(debug_error)
        highlights.clear();
    // 配置高亮，并加入到 extras 中
    for (auto &line : highlights) {
    QTextEdit::ExtraSelection h;
    h.cursor = cursor;
    // 下面这些的功能，请大家自行查看文档
    h.cursor.setPosition(line.first);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(line.second);
    extras.append(h);
    }
    // 应用高亮效果
    code->setExtraSelections(extras);
    highlights.clear();
}

//input原型
/*
//    if(input.contains(" ")){
//        Lsentence.stringlist=input.split(" ");
//        Lsentence.num=Lsentence.stringlist.first().toInt();
//        if(Lsentence.num!=0){
//            Lsentence.stringlist.pop_front();
//            list.append(Lsentence);

//            qSort(list.begin(), list.end(), compare);
//        }
//    }

//    QList<line>::Iterator it = list.begin(),itend = list.end();
//    for (;it != itend; it++){
//        QString in1=QString::number(it->num)+" ";
//        in1+=it->stringlist.join(" ");
//        ui->textBrowser_code->append(in1);
//    }


//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QTextStream in(&file);
//    QString Qline = in.readLine();

//file原型

//    while(!Qline.isNull()) {
//        if(Qline.contains(" ")){
//            Lsentence.stringlist=Qline.split(" ");
//            Lsentence.num=Lsentence.stringlist.first().toInt();
//            if(Lsentence.num!=0){
//                Lsentence.stringlist.pop_front();
//                list.append(Lsentence);

//                qSort(list.begin(), list.end(), compare);
//            }
//        }
//        Qline=in.readLine();
//    }
//    QList<line>::Iterator it = list.begin(),itend = list.end();
//    for (;it != itend; it++){
//        QString in1=QString::number(it->num)+" ";
//        in1+=it->stringlist.join(" ");
//        ui->textBrowser_code->append(in1);
//    }
*/

