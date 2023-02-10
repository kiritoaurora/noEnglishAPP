#ifndef NOENGLISH_H
#define NOENGLISH_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QtMultimedia/QMediaPlayer>
#include <QDateTime>
#include <QSignalMapper>
#include <QKeyEvent>
#include "dialog.h"
#include "ui_dialog.h"

namespace Ui {
class noEnglish;
}

struct Words     //单词结构体
{
    QString englishWord = NULL;
    QString pa = NULL;
    QString chineseWord = NULL;
    QString englishInstance1 = NULL;
    QString chineseInstance1 = NULL;
    QString pron = NULL;
    int count;
    bool forget = false;   //用于复习时判断单词是否忘记，默认为false
};

class noEnglish : public QWidget
{
    Q_OBJECT

public:
    explicit noEnglish(QWidget *parent = nullptr);
    ~noEnglish();
    QString uid;  //登陆账户的用户名
    void opening();  //登陆动画
    void init(); //初始化界面
    void load_newWords();  //获取新单词
    void load_reviewWords();  //获取待复习单词
    void learn_1(int i);   //选择中文词义界面,state = 0为newWords，state = 1为reviewWords
    void learn_2(int i);   //仅有例句界面
    void learn_3(int i);   //仅有中文词义界面
    void learn_4(int i);   //仅有中文词义界面
    void wordInfo(int i);   //单词详情界面
    void animationOfWidget();  //实现learn_1,learn_2,learn_3等界面的动画
    void memorizeNewWord();   //背单词函数
    void reviewWord();   //复习单词函数
    void selectButtonRestore();  //learn_1界面选项按钮重置
    void saveWord();    //将单词信息保存至数据库中
    void isOpenSpell();  //打开对话框，是否打开拼写界面
    void spell();       //spell界面
    void animationOfSpell();  //spell界面动画
    void keyPressEvent(QKeyEvent *event);   //拼写界面按下回车，即：按下确定按钮
    void waitAndsort();  //等待2秒，并对数组进行排序

private slots:
    void on_close_clicked();

    void on_Learn_clicked();

    void on_Review_clicked();

    void on_nextOne_clicked();

    void vocalPlay();    //播放音频

    void closeLearn(int i);   //退出Learn界面

    void selectAnswer(int i);  //Learn_1界面选项按钮

    void on_answer_clicked();

    void on_Yes_clicked();

    void on_No_clicked();

    void on_fault_clicked();

    void on_Yes_2_clicked();

    void on_No_2_clicked();

    void on_Yes_3_clicked();

    void on_No_3_clicked();

    void on_more_clicked();

    void on_rest_clicked();

    void on_check_clicked();

private:
    Ui::noEnglish *ui;
    QString sql;
    QSqlQuery query;
    QString addr;   //音频地址
    int surplus = 0;   //一组单词的剩余单词数
    int amount;   //一组单词的总数。
    int selectErrorNum;  //Learn_1界面选择错误次数
    int wordPosition = 0;  //单词下标
    int answerPosition = 0;   //记录learn_1界面正确选项位置
    bool state;   //当前处于学习单词状态或复习状态
    bool isSpellWidgetOpened;  //拼写界面是否已打开
    QDateTime currentDate;   //获取系统日期
    QMediaPlayer *pron = NULL;  //音频播放
    QPropertyAnimation *animation = NULL;  //动画
    int newNum,reviewNum;   //待学习单词数和当日待复习的单词数
    //struct Words newWords[10],reviewWords[10]; //待学单词与待复习单词，以十个为一组
    struct Words words[10];
    Dialog *messageBox = new Dialog(this);   //信息对话框
};

#endif // NOENGLISH_H
