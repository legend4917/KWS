#include "kws.h"
#include "myaudio.h"
#include "edit.h"
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <QToolButton>
#include <QPalette>

KWS::KWS(QWidget *parent) : QWidget(parent),ui(new Ui::KWS) {
    QFile styleFile(":/style/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    qApp->setStyleSheet(styleSheet);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ui->setupUi(this);
    rec_number=0;
    flag=false;
    init_widgetIn(ui);      //初始化内部控件
    init_widgetButton();    //初始化窗口关闭、最小化按钮
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(windowclose()) );    //窗口关闭
    connect(ui->minButton, SIGNAL(clicked()), this, SLOT(windowmin()));         //窗口最小化
    player = new QMediaPlayer();
    connect(ui->horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(handlePlayerStateChanged(QMediaPlayer::State)));
    connect(ui->horizontalSlider_2,SIGNAL(sliderMoved(int)), this, SLOT(setVoice(int)));
    pro = new QProcess();
    connect(pro, SIGNAL(started()), SLOT(slotStarted()));
    connect(pro, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotFinished(int, QProcess::ExitStatus)));
}

KWS::~KWS(){
    delete ui;
}

void KWS::on_pushButton_clicked(){      //提取音频的mfcc参数，并初始化播放器
    QString audio_path=QFileDialog::getOpenFileName(this,"open this document",QDir::currentPath(),"allfiles(*.*)");
    if(audio_path != ""){
        ui->lineEdit->setText(audio_path);
        file_name = audio_toWav(audio_path, pro);
        file_mfcc_name = "./tmp/";
        file_mfcc_name.append(file_name).append(".mfcc");   //创建相应的mfcc文件名
        create_file(audio_path,file_mfcc_name);  //创建target.src文件
        get_mfcc(file_mfcc_name,pro);    //提取mfcc
        init_player(player,audio_path);  //初始化播放器
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider_2->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
    }
}

void KWS::on_pushButton_2_clicked(){
    playorPause(player,ui);
}

void KWS::on_pushButton_5_clicked(){
    QString input = ui->lineEdit_2->text();
    QWidget *wgtContainter = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(wgtContainter);
    QToolButton *pDeleteBtn = new QToolButton;
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    pDeleteBtn->setIcon(closePix);
    connect(pDeleteBtn,SIGNAL(clicked()),this,SLOT(handlepDeleteBtnClick()));
    QLabel *filename = new QLabel(input);
    filename->setStyleSheet("font-family:微软雅黑");
    pDeleteBtn->resize(45,20);
    pDeleteBtn->setWhatsThis(QString::number(ui->listWidget->count()-1,10));
//    pDeleteBtn->setVisible(false);
//    pDeleteBtn->setStyleSheet("QToolButton:hover{cursor:pointer;}");
    hLayout->addWidget(filename);
    hLayout->addStretch(1);
    hLayout->addWidget(pDeleteBtn);
    hLayout->setContentsMargins(5,0,0,5);
    wgtContainter->setLayout(hLayout);
    wgtContainter->show();
    QListWidgetItem *fileItem = new QListWidgetItem(ui->listWidget);
    ui->listWidget->setItemWidget(fileItem,wgtContainter);
    fileItem->setSizeHint(QSize(100,40));
    ui->listWidget->item(ui->listWidget->count()-1)->setWhatsThis(input);
    qDebug() << ui->listWidget->count();
    ui->lineEdit_2->setText("");
}

void KWS::handlepDeleteBtnClick(){
    QToolButton *button = (QToolButton*)sender();
    int m = button->whatsThis().toInt();
    QListWidgetItem *item = ui->listWidget->takeItem(m);
    delete item;
}

void KWS::on_pushButton_6_clicked(){
    if(ui->lineEdit->text() == ""){
        QMessageBox::information(this,"提示","请先选择本地音频文件！！！",QMessageBox::Ok);
    } else {
        start();
        QString input;
        for(int t=0;t<ui->listWidget->count();t++){
            QString text = QString(ui->listWidget->item(t)->whatsThis());
            QStringList textlist = text.split(" ");
            pin_length[t]=textlist.length();
            for(int t1=0;t1<textlist.length();t1++){
                pin[t].append(textlist.at(t1));
            }
            input.append(text);
            if(t!=ui->listWidget->count()-1){
                input.append(" ");
            }
        }
        QStringList list=input.split(" ");
        QString str1 = "$chinese = a | ai | an | ang | ao | e |  en | eng | er |silence;";
        QString str2 = "$key= ";
        for(int t=0;t<list.length();t++){
            if(t!=list.length()-1){
                str2.append(list.at(t)).append("|");
            }else{
                str2.append(list.at(t)).append(";");
            }
        }
        QString str3 = " (SENT-START <$chinese|$key> SENT-END)";
        QFile file("./config/gram");
        file.open( QIODevice::WriteOnly );
        QTextStream out(&file);
        out<<str1<<endl<<str2<<endl<<str3;
        file.close();
        QString HParse_cmd = "./htk/HParse ./config/gram ./config/wdnet";
        QProcess *HParse_pro;
        HParse_pro->execute(HParse_cmd);
        QString desc_name = "./result/";
        desc_name.append(file_name).append(".mlf");     //构造音频mlf文件路径名
        get_phone(file_mfcc_name,desc_name);    //提取音频文件的音素，并写入mlf文件
        spea_mlf_name=desc_name;     //保存音频的mlf文件名

        QFile file1(spea_mlf_name);   //读取音频文件mlf数据
        file1.open( QIODevice::ReadOnly );
        QTextStream in(&file1);
        in.readLine();
        in.readLine();
        int spea_length=0;      //保存音频文件内容的长度
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(line != "."){
                QStringList strlist = line.split(" ");
                spea_mlf[spea_length].start = (qint64)strlist.at(0).toLongLong()/10000;
                spea_mlf[spea_length].end = (qint64)strlist.at(1).toLongLong()/10000;
                spea_mlf[spea_length].phone = strlist.at(2);
                spea_mlf[spea_length].poss = strlist.at(3).toDouble();
                spea_length++;
            }
        }
        file1.close();

        for(int t=0;t<spea_length;t++)
            qDebug()<<spea_mlf[t].start<<"--"<<spea_mlf[t].end<<"--"<<spea_mlf[t].phone<<"--"<<spea_mlf[t].poss;

        for(int t=0;t<ui->listWidget->count();t++){
            dist_length[t] = spea_length-pin_length[t];
            for(int t1=0;t1<dist_length[t];t1++){
                QString str1=spea_mlf[t1].phone;
                for(int t2=t1+1;t2<pin_length[t]+t1;t2++){    //拼接与录音音节相同数量的音频音节
                    str1.append(spea_mlf[t2].phone);
                }
                dist[t][t1] = edit(str1,pin[t]);  //最短距离编辑算法
            }
        }
        flag = true;
    }
    finish();
}\

void KWS::show_result(int m){
    record_th = m;
    ui->comboBox->setEnabled(true);
    ui->comboBox_2->setEnabled(true);
    ui->tableWidget_2->setRowCount(0);
    QString lowstr = ui->comboBox->currentText();
    QString highstr = ui->comboBox_2->currentText();
    lowstr = lowstr.left(lowstr.length()-1);
    highstr = highstr.left(highstr.length()-1);
    int low = (100 - lowstr.toInt())/10;
    int high = (100 - highstr.toInt())/10;
    int count=0;
    for(int t=0;t<dist_length[m];t++){
        if(dist[m][t]>=high && dist[m][t]<=low){
            count++;
            ui->tableWidget_2->setRowCount(count);
            ui->tableWidget_2->setItem(count-1,0,new QTableWidgetItem(QString::number(count,10)));
            QString time = QString::number(spea_mlf[t].start,10);
            time.append("-");
            time.append(QString::number(spea_mlf[t].end,10));
            ui->tableWidget_2->setItem(count-1,1,new QTableWidgetItem(time));
            QString diststr;
            if(dist[m][t]>=10){
                diststr = "0";
            }else{
                diststr = QString::number(10-dist[m][t],10).append("0%");
            }
            ui->tableWidget_2->setItem(count-1,2,new QTableWidgetItem(diststr));
            QPushButton *button = new QPushButton();
            button->setText("点击播放");
            button->setWhatsThis(QString::number(t,10));
            button->setStyleSheet("font-family:微软雅黑");
            ui->tableWidget_2->setCellWidget(count-1,3,button);
            connect(button,SIGNAL(clicked()),this,SLOT(handleButtonClickEvent()));
        }
    }
}

void KWS::handleButtonClickEvent(){
    QPushButton *button  = (QPushButton*) sender();
    int m = button->whatsThis().toInt();
    player->setPosition(spea_mlf[m].start);
    player->play();
}

void KWS::slotStarted(){    //提取mfcc进程开始触发
    pDialog.setFixedSize(250,100);
    pDialog.setWindowTitle("正在解析文件");
    QLabel *label = new QLabel(&pDialog);
    label->setGeometry(30,30,180,20);
    QMovie* movie = new QMovie(":/image/dialog.gif");
    label->setScaledContents(true);
    label->setMovie(movie);
    movie->start();
    pDialog.exec();
}

void KWS::slotFinished(int, QProcess::ExitStatus){  //提取mfcc进程结束触发
    pDialog.close();
}

void KWS::positionChanged(qint64 position){     //获取当前播放时长
    ui->horizontalSlider->setValue(position);
    int seconds = (position/1000) % 60;
    int minutes = (position/60000) % 60;
    int hours = (position/3600000) % 24;
    QTime time(hours, minutes,seconds);
    ui->label_2->setText(time.toString());
}

void KWS::durationChanged(qint64 duration){     //获取总播放时长
    ui->horizontalSlider->setRange(0, duration);
    int seconds = (duration/1000) % 60;
    int minutes = (duration/60000) % 60;
    int hours = (duration/3600000) % 24;
    QTime time(hours, minutes,seconds);
    ui->label_3->setText(time.toString());
}

void KWS::handlePlayerStateChanged(QMediaPlayer::State state){
    switch (state) {
        case QMediaPlayer::StoppedState:
            ui->pushButton_2->setText("播放");
            break;

        case QMediaPlayer::PausedState:
            ui->pushButton_2->setText("播放");
            break;

        case QMediaPlayer::PlayingState:
            ui->pushButton_2->setText("暂停");
            break;

        default:
            break;
    }
}

void KWS::setPosition(int position){    //
    player->setPosition(position);
}

void KWS::setVoice(int position){       //音量控制
    player->setVolume(position);
    ui->label_4->setText(QString::number(position,10));
}

void KWS::on_comboBox_currentTextChanged(const QString &arg1){
    show_result(record_th);
}

void KWS::on_comboBox_2_currentTextChanged(const QString &arg1){
    show_result(record_th);
}

void KWS::start(){
    qDialog.setFixedSize(250,100);
    qDialog.setWindowTitle("提示");
    QVBoxLayout *DLayout = new QVBoxLayout();
    QLabel *label2 = new QLabel();
    label2->setText("正在检索，等耐心等待...");
    DLayout->addWidget(label2,0,0);
    qDialog.setLayout(DLayout);
    qDialog.show();
}

void KWS::finish(){
    qDialog.close();
}

void KWS::on_listWidget_clicked(const QModelIndex &index){
    qDebug() << index.row();
    if(flag){
        show_result(index.row());
    }
    else
        QMessageBox::information(this,"提示","请先点击检索！！！",QMessageBox::Ok);
}

void KWS::on_lineEdit_textChanged(const QString &arg1){
    ui->listWidget->clear();
    ui->tableWidget_2->setRowCount(0);
}


void KWS::init_widgetButton(){
    QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    ui->minButton->setIcon(minPix);
    ui->closeButton->setIcon(closePix);
}

void KWS::windowclose(){
    this->close();
}

void KWS::windowmin(){
    this->showMinimized();
}

void KWS::mousePressEvent(QMouseEvent *e){
    last = e->globalPos();
    QPoint p = e->pos();
    if(p.x()>937 || p.y()>24)
        flag1 = false;
    else
        flag1 = true;
}

void KWS::mouseMoveEvent(QMouseEvent *e){
    if(flag1 && e->buttons()== Qt::LeftButton) {
        QPoint newpos = e->globalPos();
        QPoint upleft = mapToParent(newpos - last);     //计算距原位置的偏移
        move(upleft);
        last = newpos;      //更新原位置到最新的位置
    }
}

//void KWS::paintEvent(QPaintEvent *event)
//{
//    setAttribute(Qt::WA_TranslucentBackground);
//    QPainterPath path;
//    path.setFillRule(Qt::WindingFill);
//    path.addRect(10, 10, this->width()-20, this->height()-20);

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.fillPath(path, QBrush(Qt::white));

//    QColor color(0, 0, 0, 50);
//    for(int i=0; i<10; i++)
//    {
//        QPainterPath path;
//        path.setFillRule(Qt::WindingFill);
//        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
//        color.setAlpha(150 - qSqrt(i)*50);
//        painter.setPen(color);
//        painter.drawPath(path);
//    }
//}
