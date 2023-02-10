#include "noenglish.h"
#include "ui_noenglish.h"
#include <QTime>
#include <QMessageBox>
#include <QtGlobal>

noEnglish::noEnglish(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::noEnglish)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//取消系统标题
    currentDate = QDateTime::currentDateTime();
    messageBox->setModal(true);  //设置对话框为模态
    QSignalMapper *myMapper,*myMapper2,*myMapper3;   //整合按钮信号转发到指定槽函数
    //整合learn界面和wordInfo界面的vocal按钮信号转发到指定槽函数vocalPlay()
    myMapper = new QSignalMapper(this);
    myMapper->setMapping(ui->vocal,0);
    myMapper->setMapping(ui->vocal_2,1);
    myMapper->setMapping(ui->vocal_3,2);
    myMapper->setMapping(ui->vocal_4,3);
    myMapper->setMapping(ui->vocal_5,4);
    connect(ui->vocal,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
    connect(ui->vocal_2,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
    connect(ui->vocal_3,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
    connect(ui->vocal_4,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
    connect(ui->vocal_5,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
    connect(myMapper,SIGNAL(mapped(int)),this,SLOT(vocalPlay()));
    //整合learn界面和wordInfo界面的关闭按钮信号转发到指定槽函数closeLearn(int)
    myMapper2 = new QSignalMapper(this);
    myMapper2->setMapping(ui->close_2,0);
    myMapper2->setMapping(ui->close_3,1);
    myMapper2->setMapping(ui->close_4,2);
    myMapper2->setMapping(ui->close_5,3);
    myMapper2->setMapping(ui->close_6,4);
    myMapper2->setMapping(ui->close_7,5);
    connect(ui->close_2,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(ui->close_3,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(ui->close_4,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(ui->close_5,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(ui->close_6,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(ui->close_7,SIGNAL(clicked(bool)),myMapper2,SLOT(map()));
    connect(myMapper2,SIGNAL(mapped(int)),this,SLOT(closeLearn(int)));
    //整合learn界面选项按钮信号转发到指定槽函数selectAnswer(int)
    myMapper3 = new QSignalMapper(this);
    myMapper3->setMapping(ui->no_1,0);
    myMapper3->setMapping(ui->no_2,1);
    myMapper3->setMapping(ui->no_3,2);
    myMapper3->setMapping(ui->no_4,3);
    connect(ui->no_1,SIGNAL(clicked(bool)),myMapper3,SLOT(map()));
    connect(ui->no_2,SIGNAL(clicked(bool)),myMapper3,SLOT(map()));
    connect(ui->no_3,SIGNAL(clicked(bool)),myMapper3,SLOT(map()));
    connect(ui->no_4,SIGNAL(clicked(bool)),myMapper3,SLOT(map()));
    connect(myMapper3,SIGNAL(mapped(int)),this,SLOT(selectAnswer(int)));
    //设置Learn_1界面四个选项按钮的样式
    selectButtonRestore();
}

noEnglish::~noEnglish()
{
    delete ui;
}

//界面初始动画
void noEnglish::opening()
{   //延时2秒，关闭opWidget
    QTime time = QTime::currentTime().addMSecs(2000);
    while(QTime::currentTime() < time)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }

    animation = new QPropertyAnimation(ui->opWidget,"geometry");
    animation->setDuration(500);   //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));  //动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,-600,400,600));  //动画结束状态，结束坐标0，-600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//初始化，获取用户待学习单词数、当日待复习单词数
void noEnglish::init()
{
    sql = QString("exec numOfNewWord '%1'").arg(uid);
    query.exec(sql);
    query.next();
    newNum = query.value(0).toInt();   //待学单词数
    sql = QString("exec amountOfReviewWords '%1'").arg(uid);
    query.exec(sql);
    query.next();
    reviewNum = query.value(0).toInt();  //当日待复习单词数
    //将待学单词数和当日待复习单词数显示到窗口
    QString num = QString::number(newNum);
    QString space = "    ";
    QString text = space + num;
    ui->label_6->setText(text);
    num = QString::number(reviewNum);
    text = space + num;
    ui->label_7->setText(text);
}

//播放音频
void noEnglish::vocalPlay()
{
    pron = new QMediaPlayer;
    pron->setMedia(QMediaContent(QUrl::fromLocalFile(addr)));
    pron->play();
}

//Learn界面关闭按钮槽函数
void noEnglish::closeLearn(int i)
{
    switch(i)
    {
        case 0 :animation = new QPropertyAnimation(ui->learn_1,"geometry"); break;
        case 1 :animation = new QPropertyAnimation(ui->learn_2,"geometry"); break;
        case 2 :animation = new QPropertyAnimation(ui->learn_3,"geometry"); break;
        case 3 :animation = new QPropertyAnimation(ui->wordInfo,"geometry"); break;
        case 4 :animation = new QPropertyAnimation(ui->learn_4,"geometry"); break;
        case 5 :animation = new QPropertyAnimation(ui->spell,"geometry"); break;
    }

    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));  //动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,600,400,600));  //动画结束状态，结束坐标0，600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//获取新单词，存放到结构体数组newWords中
void noEnglish::load_newWords()
{
    if(state == true)
        sql = QString("exec newWords '%1'").arg(uid);
    else
        sql = QString("exec reviewWords '%1'").arg(uid);
    query.exec(sql);
    int i;
    for(i = 0;query.next();i++)
    {
        words[i].englishWord = query.value(0).toString();
        words[i].pa = query.value(1).toString();
        words[i].chineseWord = query.value(2).toString();
        words[i].englishInstance1 = query.value(3).toString();
        words[i].chineseInstance1 = query.value(4).toString();
        words[i].pron = query.value(5).toString();
    }
    surplus = i;   //剩余待学单词数
    amount = i;
    for(i = 0;i < amount;i++)
    {
        if(state == true)
            words[i].count = 0;
        else
            words[i].count = 3;
    }
}

//关闭按钮，退出程序
void noEnglish::on_close_clicked()
{
    this->close();
    QApplication::exit();  //退出应用程序
}

//实现learn_1,learn_2,learn_3等界面的动画
void noEnglish::animationOfWidget()
{
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(400,0,400,600));  //动画开始状态，初始坐标400，0，大小400*600
    animation->setEndValue(QRect(0,0,400,600));  //动画结束状态，结束坐标0，0，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//设置Learn_1界面四个选项按钮的样式
void noEnglish::selectButtonRestore()
{
    ui->no_1->setStyleSheet("QPushButton{text-align: left; border:none;border-radius:15px;"
                            "background-color: rgba(220, 220, 220, 120);}");
    ui->no_2->setStyleSheet("QPushButton{text-align: left; border:none;border-radius:15px;"
                            "background-color: rgba(220, 220, 220, 120);}");
    ui->no_3->setStyleSheet("QPushButton{text-align: left; border:none;border-radius:15px;"
                            "background-color: rgba(220, 220, 220, 120);}");
    ui->no_4->setStyleSheet("QPushButton{text-align: left; border:none;border-radius:15px;"
                            "background-color: rgba(220, 220, 220, 120);}");
}

//Learn_1界面选项按钮槽函数
void noEnglish::selectAnswer(int i)
{
    if(i != answerPosition)
    {   //选项错误变为红色
        switch(i)
        {
            case 0 :ui->no_1->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(255, 14, 66);}"); break;
            case 1 :ui->no_2->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(255, 14, 66);}"); break;
            case 2 :ui->no_3->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(255, 14, 66);}"); break;
            case 3 :ui->no_4->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(255, 14, 66);}"); break;
        }
        words[wordPosition].count = 0;
        selectErrorNum++;
    }
    else
    {
        ui->answer->setEnabled(false);  //设置“看答案”按钮不可点击
        //选项正确，变为绿色
        switch(i)
        {
            case 0 :ui->no_1->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(0, 170, 127);}"); break;
            case 1 :ui->no_2->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(0, 170, 127);}"); break;
            case 2 :ui->no_3->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(0, 170, 127);}"); break;
            case 3 :ui->no_4->setStyleSheet("QPushButton{text-align: left;border:none;border-radius:15px;"
                                            "background-color:rgb(0, 170, 127);}"); break;
        }
        vocalPlay(); //播放音频
        //等待1秒，进入单词详情界面
        QTime time = QTime::currentTime().addMSecs(1000);
        while(QTime::currentTime() < time)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }
        wordInfo(wordPosition);  //初始化单词详情界面
        ui->fault->hide();   //隐藏单词详情界面“记错了”按钮
        ui->nextOne->move(140,550);
        animation = new QPropertyAnimation(ui->wordInfo,"geometry");
        animationOfWidget();  //单词详情界面滑入
        ui->close_2->clicked();  //Learn_1退出
        selectButtonRestore();  //重置选项按钮样式
        ui->answer->setEnabled(true);  //恢复“看答案”按钮为可点击状态
        //若一次选中则单词count++
        if(selectErrorNum == 0)
            words[wordPosition].count++;
    }
}

//Learn_1界面看答案按钮
void noEnglish::on_answer_clicked()
{
    selectAnswer(answerPosition);  //选择正确选项
    selectErrorNum = 1;  //设置选择次数为1，确保count不加1
}

//选择中文词义界面,state = true为newWords，state = false为reviewWords
void noEnglish::learn_1(int i)
{
    QString answers[4];   //四个选项
    ui->englishWord_4->setText(words[i].englishWord);
    ui->pa_4->setText(words[i].pa);
    addr = words[i].pron;
    answers[0] = words[i].chineseWord;

    QVector<int> v;  //存放随机数
    qsrand(QTime::currentTime().msec());  //产生随机数
    int count = 1;   //计数，确保已有四个选项，初始值为1
    while(count < 4)
    {
        int randnum = qrand()%amount;    //生成0~amount之间的随机数
        if(randnum != i && v.contains(randnum) != 1)   //若随机数等于当前单词的下标，则再次循环
        {
            v.append(randnum);  //将随机数放入QVector容器中，用contains（）函数判断
            answers[count] = words[randnum].chineseWord;
            count++;
        }
    }
    for(int j = 0;j < 4;j++)   //对四个选项进行洗牌
    {
        QString tmp = answers[j];
        int randnum = qrand()%4;
        answers[j] = answers[randnum];
        answers[randnum] = tmp;
    }
    for(int j = 0;j < 4;j++)   //记录正确选项的位置
    {
        if(answers[j] == words[wordPosition].chineseWord)
        {
            answerPosition = j;
            break;
        }
    }
    //将四个选项显示到四个按钮上
    ui->no_1->setText(answers[0]);
    ui->no_2->setText(answers[1]);
    ui->no_3->setText(answers[2]);
    ui->no_4->setText(answers[3]);
}

//Learn_2，“记住了”按钮
void noEnglish::on_Yes_clicked()
{
    words[wordPosition].count++;
    wordInfo(wordPosition);  //初始化单词详情界面
    animation = new QPropertyAnimation(ui->wordInfo,"geometry");
    animationOfWidget();  //单词详情界面滑入
    ui->close_3->clicked();  //Learn_2退出
}

//Learn_2，“忘记了”按钮
void noEnglish::on_No_clicked()
{   //将该单词的count数置为0
    words[wordPosition].count = 0;
    wordInfo(wordPosition);  //初始化单词详情界面
    ui->fault->hide();    //隐藏单词详情界面“记错了”按钮
    ui->nextOne->move(140,550);  //设置“下一词”按钮位置
    animation = new QPropertyAnimation(ui->wordInfo,"geometry");
    animationOfWidget();  //单词详情界面滑入
    ui->close_3->clicked();  //Learn_2退出
}

//仅有例句界面,state = true为newWords，state = false为reviewWords
void noEnglish::learn_2(int i)
{
    ui->englishWord_2->setText(words[i].englishWord);
    ui->pa_2->setText(words[i].pa);
    ui->englishInstance_2->setText(words[i].englishInstance1);
    addr = words[i].pron;
}

//将单词信息保存至数据库中
void noEnglish::saveWord()
{
    if(state == true)  //在复习表中插入新学习的单词
    {
        for(int i = 0;i < amount;i++)
        {
            sql = QString("exec insertNewReviewWord '%1','%2'")
                    .arg(uid).arg(words[i].englishWord);
            query.exec(sql);
        }
    }
    else  //更新复习表中单词的复习信息，失败或成功
    {
        for(int i = 0;i < amount;i++)
        {
            if(words[i].forget == false)  //复习成功
            {
                sql = QString("exec updateSuReWordInfo '%1','%2'")
                        .arg(uid).arg(words[i].englishWord);
                query.exec(sql);
            }
            else   //复习失败
            {
                sql = QString("exec disableTrigger ");  //禁用review表上的触发器，以防更新为复习单词的复习日期失败
                query.exec(sql);

                sql = QString("exec updateDeReWordInfo '%1','%2'")
                        .arg(uid).arg(words[i].englishWord);
                query.exec(sql);

                sql = QString("exec enableTrigger ");  //启用review表上的触发器
                query.exec(sql);
            }
        }
    }
}

//打开对话框，是否打开拼写界面
void noEnglish::isOpenSpell()
{
    wordPosition = 0;
    messageBox->show();     //打开对话框
    if(messageBox->exec() == QDialog::Accepted)  //开始拼写，打开拼写界面
    {
        animation = new QPropertyAnimation(ui->spell,"geometry");
        surplus = amount;        
        isSpellWidgetOpened = true;
        spell();
        animationOfWidget();
    }
    else if(messageBox->exec() == QDialog::Rejected)  //放弃拼写，打开finish界面
    {
        for(int i = 0;i < amount;i++)
            ui->listWidget->addItem(words[i].englishWord);
        animation = new QPropertyAnimation(ui->finish,"geometry");
        animation->setDuration(200);   //动画持续时间
        animation->setStartValue(QRect(0,-600,400,600));  //动画开始状态，初始坐标0，-600，大小400*600
        animation->setEndValue(QRect(0,0,400,600));  //动画结束状态，结束坐标0，0，大小400*600
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void noEnglish::animationOfSpell()  //spell界面动画
{
    animation = new QPropertyAnimation(ui->chineseWord_4,"geometry");
//    animation->setDuration(200);   //动画持续时间
//    animation->setStartValue(QRect(30,280,340,110));  //动画开始状态，初始坐标0，-600，大小400*600
//    animation->setEndValue(QRect(-400,280,340,110));  //动画结束状态，结束坐标0，0，大小400*600
//    animation->start();
    spell();  //更换中文词义
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(400,280,340,110));  //动画开始状态，初始坐标0，-600，大小400*600
    animation->setEndValue(QRect(30,280,340,110));  //动画结束状态，结束坐标0，0，大小400*60
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//拼写界面，“确定按钮”
void noEnglish::on_check_clicked()
{
    //ui->lineEdit->setFocus();
    QString spellWord = ui->lineEdit->text();
    if(spellWord == words[wordPosition].englishWord)
    {
        ui->lineEdit->setStyleSheet("border:none;color: rgb(0, 170, 127);");  //拼写正确，字体为绿色
        vocalPlay(); //播放音频
        waitAndsort();
        ui->lineEdit->clear();
        ui->lineEdit->setStyleSheet("border:none;");
    }
    else
    {
        ui->lineEdit->setStyleSheet("border:none;color: rgb(243, 0, 0);");  //拼写正确，字体为红色       
        ui->lineEdit->setText(words[wordPosition].englishWord);
        vocalPlay(); //播放音频
        //等待2秒
        QTime time = QTime::currentTime().addMSecs(2000);
        while(QTime::currentTime() < time)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        }       
        ui->lineEdit->clear();
        ui->lineEdit->setStyleSheet("border:none;");
    }

    if(surplus > 0)
    {
        wordPosition = (1 + wordPosition)%surplus;
        animationOfSpell();  //中文词义文本框动画
    }
    else    //拼写完成，打开finish界面
    {
        for(int i = 0;i < amount;i++)
            ui->listWidget->addItem(words[i].englishWord);

        animation = new QPropertyAnimation(ui->finish,"geometry");
        animation->setDuration(200);   //动画持续时间
        animation->setStartValue(QRect(0,-600,400,600));  //动画开始状态，初始坐标0，-600，大小400*600
        animation->setEndValue(QRect(0,0,400,600));  //动画结束状态，结束坐标0，0，大小400*600
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        ui->close_7->clicked();
        isSpellWidgetOpened = false;
    }
}

//拼写界面按下回车，即：按下确定按钮
void noEnglish::keyPressEvent(QKeyEvent *event)
{
    if(isSpellWidgetOpened == true && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
    {
        ui->check->clicked();
    }
}

//spell界面
void noEnglish::spell()
{
    ui->lineEdit->setFocus();
    ui->chineseWord_4->setText(words[wordPosition].chineseWord);
    addr = words[wordPosition].pron;
}

//等待2秒，对数组进行排序，surplus减1
void noEnglish::waitAndsort()
{
    QTime time = QTime::currentTime().addMSecs(2000);
    while(QTime::currentTime() < time)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }

    if(surplus > 1)   //剩余量大于1时，对单词进行移动
    {
        struct Words tmp = words[wordPosition];  //暂存单词信息
        for(int i = wordPosition;i < surplus;i++)
        {
            if(i < 9)   //防止出现words[9] = words[10]的情况
                words[i] = words[i + 1];
        }
        words[surplus-1] = tmp;
    }
    surplus--;
    wordPosition--;
}

//Learn_3界面，“记住了”按钮
void noEnglish::on_Yes_2_clicked()
{
    ui->Yes_2->setEnabled(false);
    ui->No_2->setEnabled(false);  //设置两个按钮不可点击
    ui->englishWord_3->setText(words[wordPosition].englishWord);
    ui->pa_3->setText(words[wordPosition].pa);
    words[wordPosition].count++;

    vocalPlay();  //播放音频
    waitAndsort();  //对数组进行排序，surplus减1

    if(surplus > 0)  //单词剩余量不为0
    {
        wordPosition = (wordPosition + 1)%surplus; //下标向后移1位
        //wordPosition = 0;
        ui->close_4->clicked();  //退出Learn_3界面
        ui->englishWord_3->setText(NULL);
        ui->pa_3->setText(NULL);   //设置单词和音标为空
        ui->Yes_2->setEnabled(true);
        ui->No_2->setEnabled(true);  //恢复两个按钮为可点击状态
        memorizeNewWord();  //调用背单词函数
    }
    else  //单词剩余量为0，即：一组单词已学完，打开结束界面
    {
        saveWord();   //将单词信息保存至数据库中
        init();   //再次初始化主界面
        isOpenSpell();  //打开对话框，是否打开拼写界面       
        ui->close_4->clicked();  //退出Learn_3界面
    }
}

//Learn_3界面，“忘记了”按钮
void noEnglish::on_No_2_clicked()
{
    //将该单词的count数置为0
    words[wordPosition].count = 0;
    wordInfo(wordPosition);  //初始化单词详情界面
    ui->fault->hide();    //隐藏单词详情界面“记错了”按钮
    ui->nextOne->move(140,550);
    animation = new QPropertyAnimation(ui->wordInfo,"geometry");
    animationOfWidget();  //单词详情界面滑入
    ui->close_4->clicked();  //退出Learn_3界面
}

//Learn_3界面,仅有中文词义界面,state = true为newWords，state = false为reviewWords
void noEnglish::learn_3(int i)
{
    ui->chineseWord_2->setText(words[i].chineseWord);
    addr = words[i].pron;
}

//Learn_4界面,“记住了”按钮
void noEnglish::on_Yes_3_clicked()
{
    ui->Yes_3->setEnabled(false);
    ui->No_3->setEnabled(false);  //设置两个按钮不可点击
    words[wordPosition].count++;  //单词count数+1
    ui->chineseWord_3->setText(words[wordPosition].chineseWord); //显示中文意思

    vocalPlay();  //播放音频
    waitAndsort();  //等待2秒，并对数组进行排序，surplus减1

    if(surplus > 0)  //单词剩余量不为0
    {
        wordInfo(surplus);  //初始化单词详情界面
        animation = new QPropertyAnimation(ui->wordInfo,"geometry");
        animationOfWidget();  //单词详情界面滑入
        ui->close_6->clicked();  //退出Learn_4界面
        ui->chineseWord_3->setText(NULL);
        ui->Yes_3->setEnabled(true);
        ui->No_3->setEnabled(true);  //恢复两个按钮为可点击状态       
        //wordPosition = (wordPosition + 1)%surplus; //下标向后移1位
    }
    else  //单词剩余量为0，即：一组单词已学完，打开结束界面
    {   //qDebug()<<amount;
        saveWord();   //将单词信息保存至数据库中
        init();   //再次初始化主界面
        isOpenSpell();  //打开对话框，是否打开拼写界面
        ui->close_6->clicked();  //退出Learn_4界面
    }
}

//Learn_4界面，“忘记了”按钮
void noEnglish::on_No_3_clicked()
{
    words[wordPosition].forget = true;
    words[wordPosition].count = 0;
    wordInfo(wordPosition);  //初始化单词详情界面
    vocalPlay();  //播放音频
    ui->fault->hide();    //隐藏单词详情界面“记错了”按钮
    ui->nextOne->move(140,550);  //设置“下一词”按钮位置
    animation = new QPropertyAnimation(ui->wordInfo,"geometry");
    animationOfWidget();  //单词详情界面滑入
    ui->close_6->clicked();  //Learn_4退出
}

//Learn_4,复习单词界面，仅有单词
void noEnglish::learn_4(int i)
{
    ui->englishWord_5->setText(words[i].englishWord);
    ui->pa_5->setText(words[i].pa);
    addr = words[i].pron;
}

//单词详情界面，“下一词”按钮
void noEnglish::on_nextOne_clicked()
{
    wordPosition = (wordPosition + 1)%surplus; //下标向后移1位
    memorizeNewWord();  //调用背单词函数
    ui->close_5->clicked();
    ui->fault->show();   //恢复单词详情界面“记错了”按钮
    ui->nextOne->move(70,550);  //恢复位置
}

//单词详情界面，“记错了”按钮
void noEnglish::on_fault_clicked()
{
    //将该单词的count数置为0
    if(state == false)  //复习模式
    {
        words[surplus].forget = true;
        words[surplus].count = 0;
        surplus++;
    }
    else
        words[wordPosition].count = 0;
    on_nextOne_clicked();  //下一词
}

//单词详情界面
void noEnglish::wordInfo(int i)
{
    ui->englishWord->setText(words[i].englishWord);
    ui->pa->setText(words[i].pa);
    addr = words[i].pron;
    ui->chineseWord->setText(words[i].chineseWord);
    ui->englishInstance->setText(words[i].englishInstance1);
    ui->chineseInstance->setText(words[i].chineseInstance1);
}

//背单词函数
void noEnglish::memorizeNewWord()
{
    if(words[wordPosition].count == 0)  //显示learn_1界面，即选择界面
    {
        selectErrorNum = 0;   //初始化选错次数为0
        learn_1(wordPosition);
        animation = new QPropertyAnimation(ui->learn_1,"geometry");
        animationOfWidget();
        vocalPlay(); //播放音频
    }
    else if(words[wordPosition].count == 1)  //显示learn_2界面，即仅有英文例句的界面
    {
        learn_2(wordPosition);
        animation = new QPropertyAnimation(ui->learn_2,"geometry");
        animationOfWidget();
        vocalPlay(); //播放音频
    }
    else if(words[wordPosition].count == 2)  //显示learn_3界面，即仅有中文词义的界面
    {
        learn_3(wordPosition);
        animation = new QPropertyAnimation(ui->learn_3,"geometry");
        animationOfWidget();
    }
    else if(words[wordPosition].count == 3)  //显示learn_4界面，即仅有单词的界面
    {
        learn_4(wordPosition);
        animation = new QPropertyAnimation(ui->learn_4,"geometry");
        animationOfWidget();
    }
}

//Learn按钮
void noEnglish::on_Learn_clicked()
{
    if(newNum != 0)   //待学单词不为0
    {
        if(reviewNum != 0)  //复习任务未完成
        {
            QMessageBox::information(this,"Learning","请先复习！");
        }
        else  //已复习完毕，可以学习新单词
        {
            state = true;
            if(surplus == 0)  //一组单词已背完，则加载新单词
                load_newWords();  //加载待学单词到结构体数组中
            memorizeNewWord();  //调用背单词函数
        }
    }
}

//Review按钮
void noEnglish::on_Review_clicked()
{
    if(reviewNum != 0)  //待复习单词不为0
    {
         state = false;
         if(surplus == 0)  //一组单词已背完，则加载新单词
            load_newWords();  //加载复习单词到结构体数组中
         memorizeNewWord();  //调用背单词函数
    }
}

//finish界面，“再学一组”按钮
void noEnglish::on_more_clicked()
{
    animation = new QPropertyAnimation(ui->finish,"geometry");
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));  //动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,-600,400,600));  //动画结束状态，结束坐标0，-600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    ui->listWidget->clear();
    on_Learn_clicked();
}

//finish界面，“休息一下”按钮
void noEnglish::on_rest_clicked()
{
    animation = new QPropertyAnimation(ui->finish,"geometry");
    animation->setDuration(200);   //动画持续时间
    animation->setStartValue(QRect(0,0,400,600));  //动画开始状态，初始坐标0，0，大小400*600
    animation->setEndValue(QRect(0,-600,400,600));  //动画结束状态，结束坐标0，-600，大小400*600
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    ui->listWidget->clear();
}



















