#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QTableWidget>
#include <QPushButton>
#include <QSignalMapper>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    bool isPressedWidget;
    QPoint last;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void init();    //将曾登陆过的账号存入combox中，并将最近登陆账号输出到lineEdit中
    void create_PushButton();  //创建账户下拉框中的删除按钮
private slots:

    void on_close_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_login_clicked();

    void on_newaccount_clicked();

    void on_repwd_clicked();

    void deleteAccount();

    void on_close_2_clicked();

    void on_enroll_clicked();

    void on_confirm_clicked();

    void on_close_3_clicked();

private:
    Ui::Widget *ui;
    QPushButton *button = NULL;
    QSignalMapper *myMapper = NULL;
    QPropertyAnimation *animation = NULL;

};
#endif // WIDGET_H
