#ifndef WIDGET_H
#define WIDGET_H

#include "statement.h"
#include "program.h"

#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


struct line{
    int num;
    QStringList stringlist;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_Button_load_clicked();

    void on_Button_run_clicked();

    void on_Button_clear_clicked();

    void on_lineEdit_input_returnPressed();

private:
    Ui::Widget *ui;

    QString input;
    QList<QPair<int, QColor>> highlights;

    bool debug_error=false;
    bool Goto;
    int GotoNum;

    program program, result;

    void load(QFile &filename);
    void show_browser();
    void run();
    void debug_run();

    void prog_show();
    void result_show();
    void read();
    void input_wait();
    void bug_light();     //bug产生的红光
    void green_light();     //现在执行的绿色光

    void keyPressEvent(QKeyEvent *event) override;

    //signal相关操作
private slots:
    void input_num();
    void print();
    void printf();
    void help();
    void quit();
    void let();
    void gram_print();
    void compile_error();    //报错信息
    void line_error();
    void varList();

    void on_Button_debug_clicked();
};
#endif // WIDGET_H
