#include "kws.h"
#include "myaudio.h"
#include "edit.h"
#include <QDebug>
#include <QMessageBox>
#include <QMovie>

KWS::KWS(QWidget *parent) : QWidget(parent),ui(new Ui::KWS) {
    QFile styleFile(":/style/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    qApp->setStyleSheet(styleSheet);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    ui->setupUi(this);
    rec_number=0;
    init_widget(ui);
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
//        int index = file_name.lastIndexOf(".");
//        file_name.truncate(index);  //获取无后缀的文件名
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

void KWS::on_pushButton_3_clicked(){
    QString name = "record";
    QString path = "./record/record";
    if(ui->pushButton_3->text()==tr("录音")) {
        ui->pushButton_4->setText(tr("00:00"));
        i=0;j=0;k=0;l=0;
        name.append(QString::number(rec_number,10)).append(".raw");  //构造递增的录音文件名
        path.append(QString::number(rec_number,10)).append(".raw");  //构造递增的录音路径名
        rec_name[rec_number] = name;
        start_record(path);
        ui->pushButton_3->setText(tr("停止"));
        return;
    }
    if(ui->pushButton_3->text()==tr("停止")){
        stop_record(rec_name[rec_number]);
        flag[rec_number]=false;     //表示第rec_number录音文件存在
        rec_number++;
        ui->pushButton_3->setText(tr("录音"));
        return;
    }
}

void KWS::on_pushButton_5_clicked(){
    QString input = ui->lineEdit_2->text();
    ui->listWidget->addItem(new QListWidgetItem(input));
    ui->lineEdit_2->setText("");
}

void KWS::on_pushButton_6_clicked(){
    if(ui->lineEdit->text() == ""){
        QMessageBox::information(this,"提示","请先选择本地音频文件！！！",QMessageBox::Ok);
    } else {
        start();
        int index = ui->tabWidget->currentIndex();
        if(index==0){
            int rec_num;
            for(rec_num=0; rec_num<rec_number; rec_num++){  //提取录音文件的mfcc参数
                if(!flag[rec_num]){
                    get_record_mfcc(rec_num);    //提取录音的mfcc参数
                    QString path_name = "./record/record";
                    path_name.append(QString::number(rec_num,10)).append(".mfcc");
                    QString desc_name = "./result/record";
                    desc_name.append(QString::number(rec_num,10)).append(".mlf");
                    get_phone(path_name,desc_name);     //提取录音文件的音素，并写入mlf文件
                    rec_mlf_name[rec_num]=desc_name;    //保存录音的mlf文件名
                }
            }
            QString desc_name = "./result/";
            desc_name.append(file_name).append(".mlf");     //构造音频mlf文件路径名
            get_phone(file_mfcc_name,desc_name);    //提取音频文件的音素，并写入mlf文件
            spea_mlf_name=desc_name;     //保存音频的mlf文件名

            QFile file(spea_mlf_name);   //读取音频文件mlf数据
            file.open( QIODevice::ReadOnly );
            QTextStream in(&file);
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
            file.close();
            int recd_tmp_length;
            for(int t=0;t<rec_number;t++){  //读取录音文件mlf数据
                if(!flag[t]){  //第t个录音文件未被删除
                    QFile file(rec_mlf_name[t]);
                    file.open( QIODevice::ReadOnly );
                    QTextStream in(&file);
                    in.readLine();
                    in.readLine();
                    recd_tmp_length=0;   // 当前录音文件长度
                    recd_mlf_str[t]="";   //清空其内容
                    while(!in.atEnd()) {
                        QString line = in.readLine();
                        if(line != "."){
                            QStringList strlist = line.split(" ");
                            recd_mlf[t][recd_tmp_length].start = (qint64)(strlist.at(0).toLongLong()/10000);
                            recd_mlf[t][recd_tmp_length].end = (qint64)(strlist.at(1).toLongLong()/10000);
                            recd_mlf[t][recd_tmp_length].phone = strlist.at(2);
                            recd_mlf[t][recd_tmp_length].poss = strlist.at(3).toDouble();
                            recd_mlf_str[t].append(recd_mlf[t][recd_tmp_length].phone);
                            recd_tmp_length++;
                            recd_length[t]=recd_tmp_length;
                            dist_length[t]=spea_length-recd_length[t];
                        }
                    }
                    file.close();
                }
            }
            for(int t2=0;t2<rec_number;t2++){   //录音文件数遍历
                if(!flag[t2]){
                    for(int t=0;t<dist_length[t2];t++){     //音频内容遍历
                        QString str1=spea_mlf[t].phone;
                        for(int t1=t+1;t1<recd_length[t2];t1++){    //拼接与录音音节相同数量的音频音节
                            str1.append(spea_mlf[t1].phone);
                        }
                        dist[t2][t] = edit(str1,recd_mlf_str[t2]);  //最短距离编辑算法
                    }
                }
            }
        }
        else{
            QString input;
            for(int t=0;t<ui->listWidget->count();t++){
                QString text = QString(ui->listWidget->item(t)->text());
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
        }
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

void KWS::displayTime(){    //显示当前录音时长
    i++;
    if(i>=10){
        j++;
        i=0;
    }
    if(j>=10){
        k++;
        j=0;
    }
    if(k>=10){
        l++;
        k=0;
    }
    QString s=QString::number(l,10);
    s.append(QString::number(k,10));
    s.append(":");
    s.append(QString::number(j,10));
    s.append(QString::number(i,10));
    ui->pushButton_4->setText(s);
}

void KWS::start_record(QString record_name){           //开始录音
    outputFile.setFileName(record_name);
    outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    QAudioFormat format;
    format.setSampleRate(8000);     //采样率
    format.setChannelCount(2);      //信道数值
    format.setSampleSize(16);        //样本大小
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
       qWarning()<<"default format not supported try to use nearest";
       format = info.nearestFormat(format);
    }
    audio_record = new QAudioInput(format, this);
    timer = new QTimer();
    connect(audio_record, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleRecordStateChanged(QAudio::State)));
    audio_record->start(&outputFile);
}


void KWS::stop_record(QString name){    //停止录音
    audio_record->stop();
    outputFile.close();
    audio_record = NULL;
    rawTOwav(rec_number);
    QPushButton *button1 = new QPushButton();
    button1->setText(name);
    button1->setWhatsThis(QString::number(rec_number,10));
    button1->setStyleSheet("font-family:微软雅黑");
    QPushButton *button2 = new QPushButton();
    QPixmap image1(":/image/play.png");
    button2->setIcon(image1);
    button2->setCursor(QCursor(Qt::PointingHandCursor));
    button2->setFixedSize(30,30);
    button2->setWhatsThis(QString::number(rec_number,10));
    QPushButton *button3 = new QPushButton();
    QPixmap image2(":/image/delete.png");
    button3->setIcon(image2);
    button3->setCursor(QCursor(Qt::PointingHandCursor));
    button3->setFixedSize(30,30);
    button3->setWhatsThis(QString::number(rec_number,10));
    int rows = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(rows+1);
    button3->setAccessibleName(QString::number(rows,10));
    ui->tableWidget->setCellWidget(rows,0,button1);
    ui->tableWidget->setCellWidget(rows,1,button2);
    ui->tableWidget->setCellWidget(rows,2,button3);
    map1.insert(rec_number,button1);
    map2.insert(rec_number,button2);
    map3.insert(rec_number,button3);
    connect(button1,SIGNAL(clicked()),this,SLOT(handleButton1ClickEvent()));
    connect(button2,SIGNAL(clicked()),this,SLOT(handleButton2ClickEvent()));
    connect(button3,SIGNAL(clicked()),this,SLOT(handleButton3ClickEvent()));
}


void KWS::handleButton1ClickEvent(){
    QPushButton *button = (QPushButton*)sender();
    int m = button->whatsThis().toInt();
    show_result(m);
}

void KWS::handleButton2ClickEvent(){
    QPushButton *button  = (QPushButton*) sender();
    int m = button->whatsThis().toInt();
    play_record(m);
}

void KWS::handleButton3ClickEvent(){
    QPushButton *button  = (QPushButton*) sender();
    int m = button->whatsThis().toInt();
    flag[m] = true;     //表示第m个录音文件被删除了
    ui->tableWidget->removeRow(map3.value(m)->accessibleName().toInt());
    button = new QPushButton();
    button->setAccessibleName(QString::number(m,10));
    map3.insert(m,button);
    QString raw_path = "./record/";
    raw_path.append(rec_name[m]);
    QFile::remove(raw_path);
    QString wav_path = "./record/record";
    wav_path.append(QString::number(m,10)).append(".wav");
    QFile::remove(wav_path);
    QString mfcc_path = "./record/record";
    mfcc_path.append(QString::number(m,10)).append(".mfcc");
    QFile::remove(mfcc_path);
    for(int x=m+1;x<rec_number;x++){    //删除录音的后面的录音文件所对应的表格行-1
        int line = map3.value(x)->accessibleName().toInt() - 1;
        map3.value(x)->setAccessibleName(QString::number(line,10));
    }
}

void KWS::handleRecordStateChanged(QAudio::State newState){    //监听录音状态的变化
    switch (newState) {
        case QAudio::StoppedState:
            timer->stop();
            break;

        case QAudio::ActiveState:
            displayTime();
            connect(timer, SIGNAL(timeout()), this, SLOT(displayTime()));
            timer->start(10);
            break;

        default:
            break;
    }
}

void KWS::play_record(int m){    //播放录音
    QString path = "./record/";
    path.append(rec_name[m]);
    inputFile.setFileName(path);
    inputFile.open(QIODevice::ReadOnly);
    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return;
    }
    audio_play = new QAudioOutput(format, this);
    connect(audio_play,SIGNAL(stateChanged(QAudio::State)),SLOT(finish_play(QAudio::State)));
    audio_play->start(&inputFile);
}

void KWS::finish_play(QAudio::State state){     //播放结束
   if(state == QAudio::IdleState) {
     audio_play->stop();
     inputFile.close();
     audio_play = NULL;
   }
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

void KWS::on_tabWidget_tabBarClicked(int index)
{
    if(index==0){
        ui->tableWidget->setVisible(true);
        ui->listWidget->setVisible(false);
    }else{
        ui->tableWidget->setVisible(false);
        ui->listWidget->setVisible(true);
    }
}

void KWS::on_listWidget_clicked(const QModelIndex &index)
{
    show_result(index.row());
}

void KWS::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();
    ui->tableWidget_2->setRowCount(0);
}
