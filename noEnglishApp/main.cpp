#include "widget.h"
#include <QApplication>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

bool OpenDataBase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    qDebug()<<"ODBC driver?"<<db.isValid();
    QString dsn = QString::fromLocal8Bit("QTDSN2");
    db.setHostName("localhost");
    db.setDatabaseName(dsn);
    db.setUserName("sa");
    db.setPassword("s1400117627");
    if(!db.open())
    {
        qDebug()<<db.lastError().text();
        QMessageBox::critical(0,QObject::tr("Database error"),db.lastError().text());
        return false;
    }
    else
        qDebug()<<"Database open success!";
    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!OpenDataBase())
        return 1;
    Widget w;

    w.show();
    return a.exec();
//    QMediaPlayer pron;
//    pron.setMedia(QUrl::fromLocalFile("http://res.iciba.com/resource/amp3/oxford/0/e1/42/e142d564aae5adeb3eab781855dc87b2.mp3"));
//    pron.play();
//    while(1){}
//    return 0;
}
