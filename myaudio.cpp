#include <myaudio.h>
#include <QHeaderView>

void init_widget(Ui::KWS *ui){
    ui->pushButton_2->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_2->setEnabled(false);
    ui->horizontalSlider_2->setValue(50);
    ui->label_6->setPixmap(QPixmap(":/image/speaker.png"));
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0,110);
    ui->tableWidget->setColumnWidth(1,40);
    ui->tableWidget->setColumnWidth(2,40);
    QStringList list1;
    list1 << "录音文件" << "播放" << "删除";
    ui->tableWidget->setHorizontalHeaderLabels(list1);
    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget->setFont(QFont("微软雅黑"));
    ui->tableWidget->setShowGrid(false);

    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setColumnWidth(0,40);
    ui->tableWidget_2->setColumnWidth(1,130);
    ui->tableWidget_2->setColumnWidth(2,130);
    ui->tableWidget_2->setColumnWidth(3,130);
    QStringList list2;
    list2 << "序号" <<"时间段" << "置信度" << "操作";
    ui->tableWidget_2->setHorizontalHeaderLabels(list2);
    ui->tableWidget_2->verticalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget_2->setFont(QFont("微软雅黑"));
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
    ui->tableWidget->setVisible(false);
}


void create_file(QString audio_path,QString file_mfcc_name){
    QString fileName = "./config/target.src";
    QFile file( fileName );      //新建文件target.src
    file.open( QIODevice::WriteOnly );
    QTextStream out(&file);
    out << audio_path <<" "<<file_mfcc_name;  //向target.src写入audio_path和mfcc_name
    file.close();
}

QString audio_toWav(QString audio_path, QProcess *pro){
    QFileInfo fi = QFileInfo(audio_path);
    QString finame = fi.fileName();  //获取文件名
    QString file_suffix = fi.suffix();  //获取文件扩展名
    int index = finame.lastIndexOf(".");
    finame.truncate(index);      //不带扩展名的文件名
    if(file_suffix.compare("wav") == 0)
        return finame;
    QString ffmpeg_cmd = "./ffmpeg/ffmpeg -i ";
    QString target_path = " ./wav/";
    target_path.append(finame).append(".wav");
    ffmpeg_cmd.append(audio_path).append(target_path);
    pro->start(ffmpeg_cmd);
    return finame;
}

void get_mfcc(QString file_mfcc_name,QProcess *pro){
    qDebug() << HCopy_cmd;
    pro->start(HCopy_cmd);
    qDebug() <<"sucess";
    QString fileName = "./config/script.scp";
    QFile file(fileName);
    file.open( QIODevice::WriteOnly );
    QTextStream out(&file);
    out << file_mfcc_name;
    file.close();
}

void init_player(QMediaPlayer *player,QString audio_path){
    player->setMedia(QUrl::fromLocalFile(audio_path));
    player->setVolume(50);
}

void playorPause(QMediaPlayer *player,Ui::KWS *ui){        //播放、暂停切换
    if(ui->pushButton_2->text()==QObject::tr("播放")) {
        player->play();
        ui->pushButton_2->setText(QObject::tr("暂停"));
    } else {
        player->pause();
        ui->pushButton_2->setText(QObject::tr("播放"));
    }
}

void rawTOwav(int rec_number){
    QString path = "./record/record";
    path.append(QString::number(rec_number,10));
    QString raw_path = path;
    raw_path.append(".raw");
    QString wav_path = path;
    wav_path.append(".wav");
    typedef struct
    {
        char fccID[4]; //"RIFF"
        unsigned long dwSize;//length-8
        char fccType[4]; //"WAVE"
    }HEADER;
    typedef struct
    {
        char fccID[4]; //"fmt "
        unsigned long dwSize; //16
        unsigned short wFormatTag; //1
        unsigned short wChannels; //1 or 2
        unsigned long dwSamplesPerSec; //44100
        unsigned long dwAvgBytesPerSec; //
        unsigned short wBlockAlign; //声道数*量化数/8
        unsigned short uiBitsPerSample; //量化数 8 or 16
    }FMT;
    typedef struct
    {
        char fccID[4]; //"data"
        unsigned long dwSize; //length-44
    }DATA;

    HEADER pcmHEADER;
    FMT pcmFMT;
    DATA pcmDATA;
    unsigned long m_pcmData;
    FILE *fp, *fpCopy;
    fp = fopen(raw_path.toLatin1(), "rb");
    fpCopy = fopen(wav_path.toLatin1(), "wb+");
    if(!fp){
        qDebug() << "open pcm file error";
        return;
    }
    if(!fpCopy){
        qDebug() << "create wave file error";
        return;
    }
    qstrcpy(pcmHEADER.fccID, "RIFF");
    qstrcpy(pcmHEADER.fccType, "WAVE");
    fseek(fpCopy, sizeof(HEADER), 1);
    pcmFMT.dwSamplesPerSec = 8000;
    pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec*sizeof(m_pcmData);
    pcmFMT.uiBitsPerSample = 16;
    qstrcpy(pcmFMT.fccID, "fmt ");
    pcmFMT.dwSize = 16;
    pcmFMT.wBlockAlign = 4;
    pcmFMT.wChannels = 2;
    pcmFMT.wFormatTag = 1;
    fwrite(&pcmFMT, sizeof(FMT), 1, fpCopy);
    qstrcpy(pcmDATA.fccID, "data");
    pcmDATA.dwSize = 0;
    fseek(fpCopy, sizeof(DATA), 1);
    fread(&m_pcmData, sizeof(unsigned long), 1, fp);
    while(!feof(fp))
    {
        pcmDATA.dwSize += 4; //计算数据的长度；每读入一个数据，长度就加一；
        fwrite(&m_pcmData, sizeof(unsigned long), 1, fpCopy); //将数据写入.wav文件;
        fread(&m_pcmData, sizeof(unsigned long), 1, fp); //从.pcm中读入数据
    }
    fclose(fp);
    pcmHEADER.dwSize = 44+pcmDATA.dwSize-8; //根据pcmDATA.dwsize得出pcmHEADER.dwsize的值
    rewind(fpCopy); //将fpCpy变为.wav的头，以便于写入HEADER和DATA;
    fwrite(&pcmHEADER, sizeof(HEADER), 1, fpCopy); //写入HEADER
    fseek(fpCopy, sizeof(FMT), 1);  //跳过FMT,因为FMT已经写入
    fwrite(&pcmDATA, sizeof(DATA), 1, fpCopy);   //写入DATA;
    fclose(fpCopy);
}

void get_record_mfcc(int rec_number){
    QString audio_path = "./record/record";
    audio_path.append(QString::number(rec_number,10)).append(".wav");
    QString file_mfcc_name = "./record/record";
    file_mfcc_name.append(QString::number(rec_number,10)).append(".mfcc");
    create_file(audio_path,file_mfcc_name);
    QProcess *p;
    p->execute(HCopy_cmd);
}

void get_phone(QString path_name,QString desc_name){
    QString fileName = "./config/script.scp";
    QFile file( fileName );
    file.open( QIODevice::WriteOnly );
    QTextStream out(&file);
    out << path_name;  //写入待识别录音的mfcc文件路径
    file.close();
    QString HVite = "./htk/HVite -H ./config/hmm/macros -H ./config/hmm/hmmdefs -S ./config/script.scp -l * -i ";
    HVite.append(desc_name).append(" -w ./config/wdnet -p 0.0 -s 5.0 ./config/dict/dict ./config/list/triphones");
    QProcess *pHVite;
    pHVite->execute(HVite);
}
