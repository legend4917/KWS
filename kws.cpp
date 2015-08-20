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
    init_dialog();
    ui->pushButton_2->installEventFilter(this);
    ui->horizontalSlider->installEventFilter(this);
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
        ui->horizontalSlider->setMouseTracking(true);
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

bool KWS::eventFilter(QObject *obj, QEvent *e){
    if(obj == ui->pushButton_2){
        QIcon icon;
        QPixmap pixmap1;
        if(e->type() == QEvent::Enter){
            if(ui->pushButton_2->whatsThis() == "pause")
                pixmap1.load(":/image/play2.png");
            else
                pixmap1.load(":/image/pause2.png");
            icon.addPixmap(pixmap1);
            ui->pushButton_2->setIcon(icon);
            ui->pushButton_2->setIconSize(QSize(40,40));
        }
        if(e->type() == QEvent::Leave){
            if(ui->pushButton_2->whatsThis() == "play")
                pixmap1.load(":/image/pause1.png");
            else
                pixmap1.load(":/image/play1.png");
            icon.addPixmap(pixmap1);
            ui->pushButton_2->setIcon(icon);
            ui->pushButton_2->setIconSize(QSize(40,40));
        }
    }
    if(obj == ui->horizontalSlider){
        if(e->type() == QEvent::MouseButtonPress){
            QMouseEvent *mevent;
            mevent = static_cast<QMouseEvent *>(e);
            qint64 position = mevent->pos().x()/701.0 * durat;
            setPosition(position);
        }
    }
    return QWidget::eventFilter(obj, e);
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

void KWS::handleSliderPressed(){

}

void KWS::handlePlayerStateChanged(QMediaPlayer::State state){      //监听播放器的状态并进行相应的操作
    QIcon icon;
    switch (state) {
        case QMediaPlayer::StoppedState:{
            QPixmap pixmap2(":/image/play1.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            ui->pushButton_2->setWhatsThis("pause");
            ok = true;
            break;
        }
        case QMediaPlayer::PausedState:{
            QPixmap pixmap2(":/image/play1.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            ui->pushButton_2->setWhatsThis("pause");
            break;
        }
        case QMediaPlayer::PlayingState:{
            QPixmap pixmap2(":/image/pause1.png");
            icon.addPixmap(pixmap2);
            ui->pushButton_2->setIcon(icon);
            ui->pushButton_2->setWhatsThis("play");
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
    if(position == 0)
        voice.load(":/image/volume0.png");
    if(position > 0 && position <= 30)
        voice.load(":/image/volume1.png");
    if(position > 30 && position <= 70)
        voice.load(":/image/volume2.png");
    if(position > 70)
        voice.load(":/image/volume3.png");
    QPixmap scaledPixmap = voice.scaled(QSize(27,27),Qt::KeepAspectRatio);
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
        for(j=0; j<dict.size(); j++)
            if(inputlist[i] == dict[j])
                break;
        if(j >= dict.size())
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
    for(int i=m; i<ui->listWidget->count()-1; i++){
        dist[i].clear();
        for(int j=0; j<dist[i+1].size(); j++)
            dist[i].push_back(dist[i+1][j]);
    }
    dist[ui->listWidget->count()-1].clear();
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
        QRadioButton *pbtn = qobject_cast<QRadioButton*>(getModeSelect->checkedButton());
        name = pbtn->objectName();
        qDialog.flag = true;
        ui->tableWidget_2->setRowCount(0);
        mainThread = new Thread(-1);
        connect(mainThread, SIGNAL(started()), this, SLOT(showDialog()));
        connect(mainThread, SIGNAL(showDialog()), this, SLOT(readySearch()), Qt::DirectConnection);
        connect(mainThread, SIGNAL(finished()), this, SLOT(closeDialog()));
        qDialog.thread = mainThread;
        mainThread->start();
    }
}

void KWS::readySearch(){        //初始化相关检索需要的变量
    makeGram(name);
    QLabel *label = qDialog.findChild<QLabel *>();
    if(!flag){
        label->setText("正在预处理音频文件...");
        file_name = handleAudio(audio_path, durat, audio_name);
    }
    label->setText("正在进行解码，请稍候...");
    if(!flag){
        threadNum = 0;
        for(int i=0; i<audio_name.size(); i++){
            Thread *thread = new Thread(-2);
            thread->audio = i;
            connect(thread, SIGNAL(initFile(int)), this, SLOT(startInitFile(int)), Qt::DirectConnection);
            connect(thread, SIGNAL(finished()), this, SLOT(countNum()));
            thread->start();
        }
    }
    while(true){
        label->setText("正在进行解码，请稍候...");
        if(threadNum >= audio_name.size())
            break;
    }
    readMLF();      //读取解码结果
    label->setText("正在提取关键词...");
    getKeyWord();
    label->setText("正在检索，请稍候...");
    threadNum = 0;
    for(int t=0;t<ui->listWidget->count();t++){
        Thread *thread = new Thread(t);
        connect(thread, SIGNAL(startedit(int)), this, SLOT(startSearch(int)), Qt::DirectConnection);
        connect(thread, SIGNAL(started()), this, SLOT(countNum()));
        thread->start();
    }
    while(threadNum < ui->listWidget->count()){}      //等待子线程结束
    flag = true;
}

void KWS::showDialog(){         //线程开始，显示弹窗提示
    start_time = QTime::currentTime();
    qDialog.exec();
}

void KWS::closeDialog(){        //线程结束，关闭弹窗提示
    end_time = QTime::currentTime();
    qDialog.close();
    if(qDialog.flag){
        int start_m = start_time.minute();
        int start_s = start_time.second();
        int end_m = end_time.minute();
        int end_s = end_time.second();
        int use_time;
        if(end_m == start_m)
            use_time = end_s-start_s;
        else if(end_m > start_m)
            use_time = (end_m-start_m)*60 + end_s - start_s;
        else
            use_time = (60-start_m+end_m)*60 + end_s - start_s;
        QString str = "检索成功，耗时";
        str.append(QString::number(use_time,10)).append("S");
        QMessageBox::information(this,"提示",str,QMessageBox::Ok);
    }
}

void KWS::startInitFile(int audio){
    QString target_path = create_targetFile(audio, file_name, audio_name);
    get_mfcc(target_path);
    QString script_path = create_scriptFile(audio, file_name);
    get_phone(script_path, file_name, audio, name);
}

void KWS::countNum(){
    threadNum++;
    QLabel *label = qDialog.findChild<QLabel *>();
    label->setText("正在进行解码，请稍候...");
}

void KWS::getKeyWord(){
    for(int t=0;t<ui->listWidget->count();t++){
        QString text = QString(ui->listWidget->item(t)->whatsThis());
        QStringList textlist = text.split(" ");
        pin_length[t]=textlist.length();
        pin[t] = "";
        for(int t1=0;t1<textlist.length();t1++)
            pin[t].append(textlist.at(t1));
    }
}

void KWS::readMLF(){
    spea_mlf.clear();
    for(int i=0; i<audio_name.size(); i++){
        QString mlf_path = "./result/";
        mlf_path.append(file_name).append(QString::number(i,10)).append(".mlf");
        QFile file(mlf_path);   //读取音频文件mlf数据
        file.open( QIODevice::ReadOnly );
        QTextStream in(&file);
        result tmp;
        in.readLine();
        in.readLine();
        QString line = in.readLine();
        while(line != "."){
            QStringList strlist = line.split(" ");
            tmp.start = (qint64)strlist.at(0).toLongLong()/10000+i*300000;
            tmp.end = (qint64)strlist.at(1).toLongLong()/10000+i*300000;
            tmp.phone = strlist.at(2);
            tmp.poss = strlist.at(3).toDouble();
            spea_mlf.push_back(tmp);
            line = in.readLine();
        }
        file.close();
    }
}

void KWS::startSearch(int order){       //使用距离编辑算法开始检索
    dist[order].clear();
    for(int t1=0;t1<spea_mlf.size()-pin_length[order];t1++){
        QString str1=spea_mlf[t1].phone;
        for(int t2=t1+1;t2<pin_length[order]+t1;t2++){    //拼接与录音音节相同数量的音频音节
            str1.append(spea_mlf[t2].phone);
        }
        dist[order].push_back(edit(str1,pin[order]));       //最短距离编辑算法
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
    sele.clear();
    for(int t=0;t<dist[m].size();t++)
        if(dist[m][t]>=high && dist[m][t]<=low)
            sele.push_back(t);
    total_page = sele.size()/100 + 1;
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
        end_line = sele.size();
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
        QString time;
        int min = spea_mlf[sele[i]].start/60000;
        int sec = spea_mlf[sele[i]].start/1000 - min*60;
        int msec = spea_mlf[sele[i]].start - min*60000 - sec*1000;
        if(min < 10)
            time = "0";
        else
            time = "";
        time.append(QString::number(min,10)).append(":");
        if(sec < 10)
            time.append("0");
        time.append(QString::number(sec,10)).append(":");
        if(msec < 10)
            time.append("00");
        else if(msec < 100)
            time.append("0");
        time.append(QString::number(msec,10)).append("-");
        min = spea_mlf[sele[i]].end/60000;
        sec = spea_mlf[sele[i]].end/1000 - min*60;
        msec = spea_mlf[sele[i]].end - min*60000 - sec*1000;
        if(min < 10)
            time.append("0");
        time.append(QString::number(min,10)).append(":");
        if(sec < 10)
            time.append("0");
        time.append(QString::number(sec,10)).append(":");
        if(msec < 10)
            time.append("00");
        else if(msec < 100)
            time.append("0");
        time.append(QString::number(msec,10));
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
        connect(button,SIGNAL(clicked()),this,SLOT(handleButtonClickEvent()));
        ui->tableWidget_2->setCellWidget(num,3,button);
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
    dict.clear();
    QString fileName = "./config/";
    fileName.append(name).append("/dict");
    QFile file1(fileName);   //读取音频文件mlf数据
    file1.open( QIODevice::ReadOnly );
    QTextStream in(&file1);
    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList linelist = line.split(" ");
        dict.push_back(linelist[0]);
    }
}

void KWS::init_widgetButton(){      //初始化窗口关闭和最小化按钮
    QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    ui->minButton->setIcon(minPix);
    ui->closeButton->setIcon(closePix);

    dialog = new QDialog();
    QPixmap pixmap1(":/image/add.png");
    QIcon icon;
    icon.addPixmap(pixmap1);
    ui->pushButton_3->setIcon(icon);
    ui->pushButton_3->setIconSize(QSize(13,13));
    ui->pushButton_3->setCursor(Qt::PointingHandCursor);
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(addPatterm()));

    getModeSelect = new QButtonGroup(this);
    readList();
}

void KWS::readList(){
    QFile file("./config/list.txt");   //读取list文件
    file.open( QIODevice::ReadOnly );
    QTextStream in(&file);
    QString list[100];
    int n=0;
    while(!in.atEnd()){
        list[n++] = in.readLine();
    }
    file.close();
    ui->tableView->setColumnCount(n);
    for(int i=0; i<n; i++){
        QRadioButton *button = new QRadioButton();
        button->setText(list[i]);
        button->setObjectName(list[i]);
        button->setStyleSheet("font-family:微软雅黑");
        button->setCursor(Qt::PointingHandCursor);
        getModeSelect->addButton(button,i);
        ui->tableView->setCellWidget(0,i,button);

        ui->tableView->setColumnWidth(i,list[i].length()*10+10+35);
        connect(button, SIGNAL(clicked()), this, SLOT(handleRadioClick()));
        if(list[i] == "女"){
            button->setChecked(true);
            name = button->objectName();
            init_dict();
        }
    }
}

void KWS::handleRadioClick(){
    QRadioButton *btn = (QRadioButton *)sender();
    name = btn->objectName();
    init_dict();
    flag = false;
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
    QRect label_move_area = ui->label_move->frameGeometry();
    if(label_move_area.contains(p))
        inArea = true;
    else
        inArea = false;
}

void KWS::mouseMoveEvent(QMouseEvent *e){       //重载鼠标移动事件，实现窗口拖动功能
    if(inArea && e->buttons()== Qt::LeftButton) {
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
    spea_mlf.clear();
    for(int i=0; i<ui->listWidget->count(); i++){
        dist[i].clear();
        pin_length[i] = 0;
    }
    ui->listWidget->clear();
    ui->tableWidget_2->setRowCount(0);
    flag=false;
    inArea = false;
    ok = true;
    audio_name.clear();
}



//添加模式库
void KWS::addPatterm(){
    dialog->exec();
}

void KWS::init_dialog(){
    QGridLayout *gridLayout = new QGridLayout(dialog);

    QLabel *model_name_label = new QLabel("名称");
    model_name_label->setStyleSheet("height:25px;font-size:17px;font-family:微软雅黑");
    gridLayout->addWidget(model_name_label,0,0);
    QLineEdit *model_name_lineEdit = new QLineEdit();
    model_name_lineEdit->setStyleSheet("height:25px;width:250px;font-size:15px");
    gridLayout->addWidget(model_name_lineEdit,0,1);

    QLabel *model_dict_label = new QLabel("字典");
    model_dict_label->setStyleSheet("height:25px;font-size:17px;font-family:微软雅黑");
    gridLayout->addWidget(model_dict_label,1,0);
    QLineEdit *model_dict_lineEdit = new QLineEdit();
    model_dict_lineEdit->setStyleSheet("height:25px;width:250px;font-size:15px");
    gridLayout->addWidget(model_dict_lineEdit,1,1);
    QToolButton *model_dict_btn = new QToolButton();
    model_dict_btn->setStyleSheet("height:25px;width:50px;font-size:17px;font-family:微软雅黑");
    model_dict_btn->setText("选择");
    model_dict_btn->setWhatsThis("1");
    connect(model_dict_btn, SIGNAL(clicked()), this, SLOT(handleSelect()));
    gridLayout->addWidget(model_dict_btn,1,2);

    QLabel *model_gram_label = new QLabel("语法");
    model_gram_label->setStyleSheet("height:25px;font-size:17px;font-family:微软雅黑");
    gridLayout->addWidget(model_gram_label,2,0);
    QLineEdit *model_gram_lineEdit = new QLineEdit();
    model_gram_lineEdit->setStyleSheet("height:25px;width:250px;font-size:15px");
    gridLayout->addWidget(model_gram_lineEdit,2,1);
    QToolButton *model_gram_btn = new QToolButton();
    model_gram_btn->setStyleSheet("height:25px;width:50px;font-size:17px;font-family:微软雅黑");
    model_gram_btn->setText("选择");
    model_gram_btn->setWhatsThis("2");
    connect(model_gram_btn, SIGNAL(clicked()), this, SLOT(handleSelect()));
    gridLayout->addWidget(model_gram_btn,2,2);


    QLabel *model_phone_label = new QLabel("音素");
    model_phone_label->setStyleSheet("height:25px;font-size:17px;font-family:微软雅黑");
    gridLayout->addWidget(model_phone_label,3,0);
    QLineEdit *model_phone_lineEdit = new QLineEdit();
    model_phone_lineEdit->setStyleSheet("height:25px;width:250px;font-size:15px");
    gridLayout->addWidget(model_phone_lineEdit,3,1);
    QToolButton *model_phone_btn = new QToolButton();
    model_phone_btn->setStyleSheet("height:25px;width:50px;font-size:17px;font-family:微软雅黑");
    model_phone_btn->setText("选择");
    model_phone_btn->setWhatsThis("3");
    connect(model_phone_btn, SIGNAL(clicked()), this, SLOT(handleSelect()));
    gridLayout->addWidget(model_phone_btn,3,2);

    QLabel *model_hmm_label = new QLabel("模型");
    model_hmm_label->setStyleSheet("height:25px;font-size:17px;font-family:微软雅黑");
    gridLayout->addWidget(model_hmm_label,4,0);
    QLineEdit *model_hmm_lineEdit = new QLineEdit();
    model_hmm_lineEdit->setStyleSheet("height:25px;width:250px;font-size:15px");
    gridLayout->addWidget(model_hmm_lineEdit,4,1);
    QToolButton *model_hmm_btn = new QToolButton();
    model_hmm_btn->setStyleSheet("height:25px;width:50px;font-size:17px;font-family:微软雅黑");
    model_hmm_btn->setText("选择");
    model_hmm_btn->setWhatsThis("4");
    connect(model_hmm_btn, SIGNAL(clicked()), this, SLOT(handleSelect()));
    gridLayout->addWidget(model_hmm_btn,4,2);

    QToolButton *submit = new QToolButton(dialog);
    submit->setText("提交");
    submit->setStyleSheet("height:25px;width:300px;font-size: 18px");
    connect(submit, SIGNAL(clicked()), this, SLOT(handleSubmit()));
    gridLayout->addWidget(submit,5,1);
    dialog->setLayout(gridLayout);
    dialog->setWindowTitle("添加模式库");
}

void KWS::handleSubmit(){
    QList<QLineEdit *> lineEdit = dialog->findChildren<QLineEdit *>();
    for(int i=0; i<lineEdit.length(); i++){
        if(lineEdit[i]->text() == ""){
            QMessageBox::information(dialog,"提示","输入不能为空！！！",QMessageBox::Ok);
            return;
        }
    }
    QDir hmm(lineEdit[4]->text());
    QFileInfoList list = hmm.entryInfoList();
    if(list.count() != 4){
        QMessageBox::information(dialog,"提示","内容不正确，该文件夹下只能有模型文件和宏文件！！！",QMessageBox::Ok);
        return;
    }
    QString path = "./config/";
    path.append(lineEdit[0]->text());
    QDir *temp = new QDir;
    if(temp->exists(path)){
        QMessageBox::information(dialog,"提示","模式库名称已存在，请重新命名！！！",QMessageBox::Ok);
        return;
    }
    path.append("/");
    if(temp->mkdir(path)){
        QFile *file = new QFile();
        QString fileName[3] = {"dict","gram","triphones"};
        for(int i=1; i<lineEdit.length()-1; i++){
            QString dest_file = path;
            dest_file.append(fileName[i-1]);
            file->copy(lineEdit[i]->text(), dest_file);
        }
        path.append("hmm/");
        temp->mkdir(path);
        for(int i=2; i<list.length(); i++){
            QString dest_file = path;
            if(list.at(i).size() < 1000){
                dest_file.append("macros");
                file->copy(list.at(i).absoluteFilePath(), dest_file);
            }else{
                dest_file.append("hmmdefs");
                file->copy(list.at(i).absoluteFilePath(), dest_file);
            }
        }
    }
    dialog->close();
    QMessageBox::information(this,"提示","模式库添加成功！！！",QMessageBox::Ok);
    QFile file("./config/list.txt");   //读取音频文件mlf数据
    file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append );
    QTextStream out(&file);
    out << lineEdit[0]->text() << endl;
    file.close();
    readList();
}

void KWS::handleSelect(){
    QToolButton *button = (QToolButton*)sender();
    int i = button->whatsThis().toInt();
    QString file_path;
    if(i == 4)
        file_path = QFileDialog::getExistingDirectory(dialog, tr("Open Directory"),QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    else
        file_path = QFileDialog::getOpenFileName(dialog,"open this document",QDir::currentPath(),"allfiles(*.*)");
    if(file_path != ""){
        QList<QLineEdit *> lineEdit = dialog->findChildren<QLineEdit *>();
        lineEdit[i]->setText(file_path);
    }
}
