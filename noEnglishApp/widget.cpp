#include "widget.h"
#include "ui_widget.h"
#include "noenglish.h"
#include "ui_noenglish.h"
#include <QPoint>
#include <QMouseEvent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//取消系统标题
    //this->setAttribute(Qt::WA_DeleteOnClose,NULL);   //关闭窗口后释放内存
    ui->tableWidget->setColumnWidth(0,180);
    ui->tableWidget->setColumnWidth(1,30);
    init();  //初始化登陆界面的combox和lineEdit

}

Widget::~Widget()
{
    delete ui;
}

/********************************************************************************/
//初始化登陆界面的combox和lineEdit，将最近登陆过的账号显示到lineEdit上，并将曾有过登陆记录的账号
//输入到combox中
/********************************************************************************/
void Widget::init()
{   //查询有登陆记录的账号的用户名
    QString sql=QString("select userName from users where lastLogin is not NULL order by lastLogin DESC");
    QSqlQuery query;
    query.exec(sql);

    myMapper = new QSignalMapper(this);
    ui->comboBox->setView(ui->tableWidget);
    ui->comboBox->setModel(ui->tableWidget->model());

    for(int i = 0;query.next();i++)   //将用户名插入到combox中
    {
        int rowcount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowcount);   //插入一行

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(query.value(0).toString()));
        //ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        create_PushButton();    //生成一个按钮
        myMapper->setMapping(button,i);  //将按钮和整数i关联起来
        connect(button,SIGNAL(clicked(bool)),myMapper,SLOT(map())); //将clicked信号连接到map()槽函数上
        ui->tableWidget->setCellWidget(i,1,button);
    }
    connect(myMapper,SIGNAL(mapped(int)),this,SLOT(deleteAccount()));//将按钮的信号连接到deleteAccount()函数上
    //查询最近登陆过的账号信息
    sql = QString("select TOP 1 * from users order by lastLogin DESC");
    query.exec(sql);
    query.next();

    int isRemeberPwd;
    QString uid,pwd;
    //读取用户名，密码和记住密码情况信息
    uid = query.value(0).toString();
    pwd = query.value(1).toString();
    isRemeberPwd = query.value(2).toInt();
    //判定是否曾记住密码
    if(isRemeberPwd == 1)
    {
        ui->lineEdit->setText(uid);
        ui->lineEdit_2->setText(pwd);
        ui->checkBox->setChecked(1);
    }
    else
    {
        ui->lineEdit->setText(uid);
    }
}

void Widget::deleteAccount()   //账户下拉框按钮，删除账户登录信息
{
    int row = ui->tableWidget->currentIndex().row();
    QString uid = ui->tableWidget->item(row,0)->text();
    ui->tableWidget->removeRow(row);
    //qDebug()<<uid;
    QString sql=QString("update users set lastLogin = NULL where userName = '%1'").arg(uid);
    QSqlQuery query;
    query.exec(sql);
}

void Widget::create_PushButton()   //创建一个button，用于账户下拉框
{
    button = new QPushButton();
    button->setFlat(true);
    button->setStyleSheet("QPushButton:!hover{background-color:transparent;border:none;image: url(:/icon/minClose.png);}"
                          "QPushButton:hover{background-color:transparent;border:none;image: url(:/icon/minClose2.png);}");

}

void Widget::mousePressEvent(QMouseEvent *event)  //鼠标按下按键，获取鼠标当前的全局坐标，准备窗口移动
{
    isPressedWidget = true;     //窗口移动信号
    last = event->globalPos();  //起始坐标
}

void Widget::mouseMoveEvent(QMouseEvent *event)   //计算鼠标移动距离，移动窗口位置
{
    if(isPressedWidget)
    {
        int dx = event->globalX() - last.x();   //计算x轴和y轴移动的距离
        int dy = event->globalY() - last.y();
        last = event->globalPos();   //更新起始坐标的坐标值
        move(this->pos().x()+dx,this->pos().y()+dy);  //移动窗口
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)   //释放鼠标，窗口位置固定
{
    int x = event->globalX() - last.x();   //计算x轴和y轴移动的距离
    int y = event->globalY() - last.y();
    move(this->pos().x()+x,this->pos().y()+y);  //移动窗口
    isPressedWidget = false;   //解除窗口移动
}

void Widget::on_close_clicked()  //关闭按钮
{
    this->close();
    QApplication::exit();  //退出应用程序
}

void Widget::on_comboBox_currentIndexChanged(int index)  //选择账号
{
    index = 0;
    QString uid;
    uid = ui->comboBox->currentText();
    //查询该帐号相关信息
    QString sql=QString("select * from users where userName = '%1'").arg(uid);
    QSqlQuery query;
    query.exec(sql);
    query.next();

    int isRemeberPwd;
    QString pwd;
    //获取密码和记住密码情况
    pwd = query.value(1).toString();
    isRemeberPwd = query.value(2).toInt();
    //判定该账号是否曾记住密码
    if(isRemeberPwd == 1)
    {
        ui->lineEdit->setText(uid);
        ui->lineEdit_2->setText(pwd);
        ui->checkBox->setChecked(1);
    }
    else
    {
        ui->lineEdit->setText(uid);
        ui->lineEdit_2->clear();
        ui->checkBox->setChecked(0);
    }
}

void Widget::on_login_clicked()  //登录按钮
{
    QString uid,pwd;
    uid = ui->lineEdit->text();
    pwd = ui->lineEdit_2->text();
    //查询用户名和密码是否正确
    QString sql=QString("select * from users where userName='%1' and passWord='%2'")
            .arg(uid).arg(pwd);
    QSqlQuery query;
    if(uid==""||pwd=="")
    {
       QMessageBox::information(this,"登录信息","用户名或密码不能为空！");
    }
    else if(query.exec(sql)==true&&query.next()==true) //由此用户且密码正确
    {
        //获取登陆时间,并更新登陆时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString current_date_time = currentDateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");

        //更新登陆时间
        sql = QString("update users set lastLogin = '%1' where userName = '%2'")
                .arg(current_date_time).arg(uid);
        query.exec(sql);

        //判定是否勾选记住密码，若勾选则设置isRemeberPwd为1，反之则为0
        if(ui->checkBox->checkState() == Qt::Checked)
        {
            sql = QString("update users set isRemeberPwd = 1 where userName = '%1'").arg(uid);
        }
        else
        {
            sql = QString("update users set isRemeberPwd = 0 where userName = '%1'").arg(uid);
        }
        query.exec(sql);

        sql = QString("exec disableTrigger ");  //禁用review表上的触发器，以防更新为复习单词的复习日期失败
        query.exec(sql);

        sql = QString("exec slackWords '%1'").arg(uid);   //更新未复习单词的复习日期
        query.exec(sql);

        sql = QString("exec enableTrigger ");  //启用review表上的触发器
        query.exec(sql);

        //关闭登陆界面打开背单词界面
        this->close();
        noEnglish *noEnglish_widget = new noEnglish;
        noEnglish_widget->uid = uid;
        noEnglish_widget->init();
        noEnglish_widget->show();
        noEnglish_widget->opening();
    }
    else
    {
        QMessageBox::information(this,"登录信息","密码或用户名错误！");
        ui->lineEdit_2->clear();
    }
}

void Widget::on_newaccount_clicked()  //打开注册界面按钮
{   
    animation = new QPropertyAnimation(ui->register_2,"geometry");
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(0,-600,400,600));  //动画开始状态，初始坐标0，-600，大小400*600
    animation->setEndValue(QRect(0,0,400,600));  //动画结束状态，结束坐标0，0，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::on_close_2_clicked()  //关闭注册界面按钮
{
    animation = new QPropertyAnimation(ui->register_2,"geometry");
    animation->setDuration(200); //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));//动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,-600,400,600));//动画结束状态，结束坐标0，-600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::on_enroll_clicked()  //注册界面注册按钮
{   //获取注册信息
    QString uid = ui->lineEdit_3->text();
    QString pwd = ui->lineEdit_4->text();
    QString pwd_2 = ui->lineEdit_5->text();
    QString sql=QString("select * from users where userName='%1'")
            .arg(uid);
    QSqlQuery query;
    if(uid=="" || pwd=="" || pwd_2=="")  //检查信息是否填写完全
    {
       QMessageBox::information(this,"注册信息","信息填写不完全！");
    }
    else if(ui->lineEdit_4->text() != ui->lineEdit_5->text())  //两次输入的密码是否一致
    {
        QMessageBox::information(this,"注册信息","密码不一致！");
    }
    else if(query.exec(sql)==true&&query.next()==true) //信息填写完全但用户已存在
    {
        QMessageBox::information(this,"注册信息","该账号已注册！");
    }
    else   //信息填写完全且用户不存在，可以注册
    {
        QMessageBox::information(this,"注册信息","注册成功！");
        on_close_2_clicked();   //关闭注册界面，并清空信息
        ui->lineEdit_3->clear();
        ui->lineEdit_4->clear();
        ui->lineEdit_5->clear();
        //将新账号信息写入到数据库中
        sql = QString("insert into users(userName,passWord) values('%1','%2')").arg(uid).arg(pwd);
        query.exec(sql);
    }
}

void Widget::on_repwd_clicked()   //打开忘记密码界面
{
    QString uid = ui->lineEdit->text();
    ui->lineEdit_6->setText(uid);
    animation = new QPropertyAnimation(ui->chpwd,"geometry");
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(0,600,400,600));  //动画开始状态，初始坐标0，600，大小400*600
    animation->setEndValue(QRect(0,0,400,600));  //动画结束状态，结束坐标0，0，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::on_close_3_clicked()  //关闭忘记密码界面
{
    animation = new QPropertyAnimation(ui->chpwd,"geometry");
    animation->setDuration(200); //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));//动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,600,400,600));//动画结束状态，结束坐标0，600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Widget::on_confirm_clicked()  //忘记密码界面确认按钮
{   //获取信息
    QString uid = ui->lineEdit_6->text();
    QString pwdCheck = ui->lineEdit_7->text();
    QString pwd = ui->lineEdit_8->text();
    QString pwd_2 = ui->lineEdit_9->text();
    QString sql=QString("select * from users where userName='%1'")
            .arg(uid);
    QSqlQuery query;
    if(uid=="" || pwdCheck=="" || pwd=="" || pwd_2=="") //信息填写是否完全
    {
       QMessageBox::information(this,"密码信息","信息填写不完全！");
    }
    else if(query.exec(sql)==true&&query.next()==true) //填写完全且用户存在
    {
        QString realPwdCh = query.value(4).toString();  //获取设置的密保
        if(pwdCheck == realPwdCh)  //密保认证正确
        {
            sql = QString("update users set passWord = '%1' where userName = '%2'").arg(uid).arg(pwd);
            query.exec(sql);
        }
        else  //密保认证失败
        {
            QMessageBox::information(this,"密码信息","密保认证失败！");
        }
    }
    else //信息填写完全但用户不存在
    {
         QMessageBox::information(this,"密码信息","该用户不存在！");
    }
}


