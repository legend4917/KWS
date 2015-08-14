#include "kws.h"
#include "init.h"
#include "thread.h"
#include "myaudio.h"
#include "edit.h"

KWS::KWS(QWidget *parent) : QWidget(parent),ui(new Ui::KWS) {
    ui->setupUi(this);
    init(ui, &qDialog);      //初始化内部控件
    init_widgetButton();    //初始化窗口关闭、最小化按钮
    init_dict();            //初始化字典
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(windowclose()) );    //窗口关闭
    connect(ui->minButton, SIGNAL(clicked()), this, SLOT(windowmin()));         //窗口最小化
    player = new QMediaPlayer();
    connect(ui->horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
    connect(ui->horizontalSlider_2,SIGNAL(sliderMoved(int)), this, SLOT(setVoice(int)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(handlePlayerStateChanged(QMediaPlayer::State)));
}

KWS::~KWS(){
    delete ui;
}


//播放器模块
void KWS::on_pushButton_clicked(){      //提取音频的mfcc参数，并初始化播放器
    audio_path = QFileDialog::getOpenFileName(this,"open this document",QDir::currentPath(),"allfiles(*.*)");
    if(audio_path != ""){
        ui->lineEdit->setText(audio_path);
        player->setMedia(QUrl::fromLocalFile(audio_path));  //初始化播放器
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider_2->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
    }
}

void KWS::on_pushButton_2_clicked(){    //播放和暂停
    if(ok){
        player->play();
        ok = false;
    }else{
        player->pause();
        ok = true;
    }
}

void KWS::positionChanged(qint64 position){     //监听当前播放时长是否发生变化
    ui->horizontalSlider->setValue(position);
    int seconds = (position/1000) % 60;
    int minutes = (position/60000) % 60;
    int hours = (position/3600000) % 24;
    QTime time(hours, minutes,seconds);
    ui->label_2->setText(time.toString());
}

void KWS::durationChanged(qint64 duration){     //监听音频总时长是否发生变化
    ui->horizontalSlider->setRange(0, duration);
    int seconds = (duration/1000) % 60;
    int minutes = (duration/60000) % 60;
    int hours = (duration/3600000) % 24;
    QTime time(hours, minutes,seconds);
    ui->label_3->setText(time.toString());
    durat = duration;
}

void KWS::handlePlayerStateChanged(QMediaPlayer::State state){      //监听播放器的状态并进行相应的操作
    QIcon icon;
    switch (state) {
        case QMediaPlayer::StoppedState:{
            QPixmap pixmap2(":/image/play.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            ok = true;
            break;
        }
        case QMediaPlayer::PausedState:{
            QPixmap pixmap2(":/image/play.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            break;
        }
        case QMediaPlayer::PlayingState:{
            QPixmap pixmap2(":/image/pause.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            break;
        }
        default:
            break;
    }
}

void KWS::setPosition(int position){        //设置播放当前时间点的音频
    player->setPosition(position);
}

void KWS::setVoice(int position){           //播放器音量控制
    player->setVolume(position);
    QPixmap voice;
    if(position <= 30)
        voice.load(":/image/voice1.png");
    if(position > 30 && position <= 70)
        voice.load(":/image/voice2.png");
    if(position > 70)
        voice.load(":/image/voice3.png");
    QPixmap scaledPixmap = voice.scaled(QSize(35,35),Qt::KeepAspectRatio);
    ui->label_6->setPixmap(scaledPixmap);
}




//写入待检索拼音模块
void KWS::on_pushButton_5_clicked(){        //写入拼音并提交到listWidget
    QString input = ui->lineEdit_2->text();
    if(input == ""){
        QMessageBox::information(this,"提示","不可输入空字符串！！！",QMessageBox::Ok);
        return;
    }
    if(!isInDict(input)){       //判断输入是否合法
        QMessageBox::information(this,"提示","请输入合法的拼音！！！",QMessageBox::Ok);
        return;
    }
    QWidget *wgtContainter = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(wgtContainter);
    QToolButton *pDeleteBtn = new QToolButton;
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    pDeleteBtn->setIcon(closePix);
    connect(pDeleteBtn,SIGNAL(clicked()),this,SLOT(handlepDeleteBtnClick()));
    QLabel *filename = new QLabel(input);
    filename->setStyleSheet("font-family:微软雅黑");
    pDeleteBtn->resize(45,20);
    pDeleteBtn->setCursor(Qt::PointingHandCursor);
    pDeleteBtn->setWhatsThis(QString::number(ui->listWidget->count(),10));
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
    ui->lineEdit_2->setText("");
}

bool KWS::isInDict(QString input){
    QStringList inputlist = input.split(" ");
    int i,j;
    for(i=0; i<inputlist.length(); i++){
        for(j=0; j<dict_length; j++)
            if(inputlist[i] == dict[j])
                break;
        if(j >= dict_length)
            return false;
    }
    return true;
}

void KWS::handlepDeleteBtnClick(){      //删除拼音操作
    QToolButton *button = (QToolButton*)sender();
    int m = button->whatsThis().toInt();
    QList<QToolButton *> pDelbtn = ui->listWidget->findChildren<QToolButton *>();
    for(int q=m+1; q<pDelbtn.length(); q++){
        int n = pDelbtn[q]->whatsThis().toInt();
        pDelbtn[q]->setWhatsThis(QString::number(n-1,10));
    }
    QListWidgetItem *item = ui->listWidget->takeItem(m);
    delete item;
}

void KWS::on_listWidget_clicked(const QModelIndex &index){      //显示结果前检查是否已经检索
    if(flag){
        m = index.row();
        select_result();
    }
    else
        QMessageBox::information(this,"提示","请先点击检索！！！",QMessageBox::Ok);
}



//检索模块
void KWS::on_pushButton_6_clicked(){        //点击开始检索流程
    if(ui->lineEdit->text() == ""){
        QMessageBox::information(this,"提示","请先选择本地音频文件！！！",QMessageBox::Ok);
    }
    else if(ui->listWidget->count() == 0){
        QMessageBox::information(this,"提示","请先输入待检索关键词！！！",QMessageBox::Ok);
    } else {
        Thread *thread = new Thread(-1);
        connect(thread, SIGNAL(started()), this, SLOT(showDialog()));
        connect(thread, SIGNAL(showDialog()), this, SLOT(readySearch()), Qt::DirectConnection);
        connect(thread, SIGNAL(finished()), this, SLOT(closeDialog()));
        thread->start();
    }
}

void KWS::readySearch(){        //初始化相关检索需要的变量
    QString file_name;
    QLabel *label = qDialog.findChild<QLabel *>();
    if(!flag){
        label->setText("正在解析音频文件...");
        file_name = handleAudio(audio_path, durat, &cnt, label);
    }
    label->setText("正在提取关键词...");
    QString input = getKeyWord();

    label->setText("正在生成语法...");
    makeGram(input);

    label->setText("正在识别音素...");
    QString mlf_name = get_phone(file_name);    //提取音频文件的音素，并写入mlf文件

    readMLF(mlf_name);
    label->setText("正在检索，请稍候...");
    for(int t=0;t<ui->listWidget->count();t++){
        Thread *thread = new Thread(t);
        connect(thread, SIGNAL(startedit(int)), this, SLOT(startSearch(int)));
        thread->start();
    }
    flag = true;
}

void KWS::showDialog(){         //线程开始，显示弹窗提示
    qDialog.exec();
}

void KWS::closeDialog(){        //线程结束，关闭弹窗提示
    qDialog.close();
}

QString KWS::getKeyWord(){
    QString input;
    for(int t=0;t<ui->listWidget->count();t++){
        QString text = QString(ui->listWidget->item(t)->whatsThis());
        QStringList textlist = text.split(" ");
        pin_length[t]=textlist.length();
        pin[t] = "";
        for(int t1=0;t1<textlist.length();t1++){
            pin[t].append(textlist.at(t1));
        }
        input.append(text);
        if(t!=ui->listWidget->count()-1){
            input.append(" ");
        }
    }
    return input;
}

void KWS::readMLF(QString mlf_name){
    QFile file1(mlf_name);   //读取音频文件mlf数据
    file1.open( QIODevice::ReadOnly );
    QTextStream in(&file1);
    in.readLine();
    int k = 0;
    for(int i=0; i<cnt-1; i++){
        in.readLine();
        QString line = in.readLine();
        while(line != "."){
            QStringList strlist = line.split(" ");
            spea_mlf[k].start = (qint64)strlist.at(0).toLongLong()/10000+i*300000;
            spea_mlf[k].end = (qint64)strlist.at(1).toLongLong()/10000+i*300000;
            spea_mlf[k].phone = strlist.at(2);
            spea_mlf[k].poss = strlist.at(3).toDouble();
            line = in.readLine();
            k++;
        }
    }
    spea_length = k;
    file1.close();
}

void KWS::startSearch(int order){       //使用距离编辑算法开始检索
    dist_length[order] = spea_length-pin_length[order];
    for(int t1=0;t1<dist_length[order];t1++){
        QString str1=spea_mlf[t1].phone;
        for(int t2=t1+1;t2<pin_length[order]+t1;t2++){    //拼接与录音音节相同数量的音频音节
            str1.append(spea_mlf[t2].phone);
        }
        dist[order][t1] = edit(str1,pin[order]);  //最短距离编辑算法
    }
}



//显示检索结果模块
void KWS::select_result(){
    ui->comboBox->setEnabled(true);
    ui->comboBox_2->setEnabled(true);
    QString lowstr = ui->comboBox->currentText();
    QString highstr = ui->comboBox_2->currentText();
    lowstr = lowstr.left(lowstr.length()-1);
    highstr = highstr.left(highstr.length()-1);
    int low = (100 - lowstr.toInt())/10;
    int high = (100 - highstr.toInt())/10;
    count_line=0;
    for(int t=0;t<dist_length[m];t++)
        if(dist[m][t]>=high && dist[m][t]<=low)
            sele[count_line++] = t;
    total_page = count_line/100 + 1;
    QString text = "总共 ";
    text.append(QString::number(total_page,10)).append(" 页");
    ui->label_totalpage->setText(text);
    ui->lineEdit_page->setEnabled(true);
    ui->toolButton_goto->setEnabled(true);
    current_page = 1;
    show_result();
}

void KWS::show_result(){        //显示检索结果
    int current_line = (current_page-1)*100;
    int end_line = current_line + 100;
    if(current_page == 1)
        ui->toolButton_prev->setEnabled(false);
    else
        ui->toolButton_prev->setEnabled(true);
    if(current_page == total_page){
        ui->toolButton_next->setEnabled(false);
        end_line = count_line;
    }
    else
        ui->toolButton_next->setEnabled(true);
    QString text = "当前第 ";
    text.append(QString::number(current_page, 10)).append(" 页");
    ui->label_currentpage->setText(text);
    ui->lineEdit_page->setText(QString::number(current_page, 10));
    ui->tableWidget_2->setRowCount(end_line-current_line);
    int num = 0;
    for(int i=current_line; i<end_line; i++){
        ui->tableWidget_2->setItem(num,0,new QTableWidgetItem(QString::number(num+1,10)));
        QString time = QString::number(spea_mlf[sele[i]].start,10);
        time.append("-");
        time.append(QString::number(spea_mlf[sele[i]+pin_length[m]-1].end,10));
        ui->tableWidget_2->setItem(num,1,new QTableWidgetItem(time));
        QString diststr;
        if(dist[m][sele[i]]>=10){
            diststr = "0";
        }else{
            diststr = QString::number(10-dist[m][sele[i]],10).append("0%");
        }
        ui->tableWidget_2->setItem(num,2,new QTableWidgetItem(diststr));
        QPushButton *button = new QPushButton();
        button->setText("点击播放");
        button->setCursor(Qt::PointingHandCursor);
        button->setWhatsThis(QString::number(sele[i],10));
        button->setStyleSheet("QPushButton{font-family:微软雅黑;border:none}");
        ui->tableWidget_2->setCellWidget(num,3,button);
        connect(button,SIGNAL(clicked()),this,SLOT(handleButtonClickEvent()));
        num++;
    }
}

void KWS::on_toolButton_next_clicked(){
    current_page = current_page+1;
    show_result();
}

void KWS::on_toolButton_prev_clicked(){
    current_page = current_page-1;
    show_result();
}

void KWS::on_toolButton_goto_clicked(){
    current_page = ui->lineEdit_page->text().toInt();
    if(current_page > total_page)
        QMessageBox::information(this,"提示","不可大于总页数！！！",QMessageBox::Ok);
    else
        show_result();
}

void KWS::handleButtonClickEvent(){         //点击相应检索结果播放音频
    ok = false;
    QPushButton *button  = (QPushButton*) sender();
    int m = button->whatsThis().toInt();
    player->setPosition(spea_mlf[m].start);
    player->play();
}





//置信度调节模块
void KWS::on_comboBox_currentTextChanged(const QString &arg1){      //监听置信下限的变化
    select_result();
}

void KWS::on_comboBox_2_currentTextChanged(const QString &arg1){        //监听置信上限的变化
    select_result();
}



//初始化窗口组件模块
void KWS::init_dict(){          //初始化字典
    QFile file1("./config/dict/dict");   //读取音频文件mlf数据
    file1.open( QIODevice::ReadOnly );
    QTextStream in(&file1);
    dict_length=0;
    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList linelist = line.split(" ");
        dict[dict_length++] = linelist[0];
    }
}

void KWS::init_widgetButton(){      //初始化窗口关闭和最小化按钮
    QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    ui->minButton->setIcon(minPix);
    ui->closeButton->setIcon(closePix);
}

void KWS::windowclose(){        //关闭窗口
    QString path[5] = {"./mfcc","./record","./result","./wav"};
    for(int i=0; i<4; i++){
        QDir d(path[i]);
        for(int j=0; j<d.count(); j++)
            d.remove(d[j]);
    }
    this->close();
}

void KWS::windowmin(){          //最小化窗口
    this->showMinimized();
}

void KWS::mousePressEvent(QMouseEvent *e){      //重载鼠标点击事件
    last = e->globalPos();
    QPoint p = e->pos();
    if(p.x()<40 || p.x()>930 || p.y()>50)
        flag1 = false;
    else
        flag1 = true;
}

void KWS::mouseMoveEvent(QMouseEvent *e){       //重载鼠标移动事件，实现窗口拖动功能
    if(flag1 && e->buttons()== Qt::LeftButton) {
        QPoint newpos = e->globalPos();
        QPoint upleft = mapToParent(newpos - last);     //计算距原位置的偏移
        move(upleft);
        last = newpos;      //更新原位置到最新的位置
    }
}

void KWS::keyPressEvent(QKeyEvent *e){          //重载键盘输入回车键事件
    if(ui->lineEdit_2->hasFocus()){
        if (  e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            on_pushButton_5_clicked();
    }
    if(ui->lineEdit_page->hasFocus()){
        if (  e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            on_toolButton_goto_clicked();
    }
}

void KWS::on_lineEdit_textChanged(const QString &arg1){     //监听输入音频是否发生变化
    ui->listWidget->clear();
    ui->tableWidget_2->setRowCount(0);
    flag=false;
    ok = true;
}
