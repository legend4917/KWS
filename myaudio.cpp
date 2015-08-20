#include "myaudio.h"
#include <QHeaderView>
#include <QFileInfo>

QString handleAudio(QString audio_path, qint64 duration, QVector<QString> &audio_name){
    QFileInfo fi = QFileInfo(audio_path);
    QString file_suffix = fi.suffix();
    QString file_name = fi.fileName();  //获取文件名
    int index = file_name.lastIndexOf(".");
    file_name.truncate(index);      //不带扩展名的文件名
    if(file_suffix != "wav")
        audio_path = audio_toWav(audio_path, file_name);
    segmentAudio(audio_path, file_name, duration, audio_name);     //音频分割
    return file_name;
}

QString audio_toWav(QString audio_path, QString file_name){
    QString ffmpeg_cmd = "./ffmpeg/ffmpeg -y -i ";
    QString target_path = "./wav/";
    target_path.append(file_name).append(".wav");
    ffmpeg_cmd.append(audio_path).append(" ").append(target_path);
    QProcess *p = new QProcess();
    p->execute(ffmpeg_cmd);     //对当前音频文件进行格式转化为wav音频文件
    return target_path;
}

void segmentAudio(QString audio_path, QString file_name, qint64 duration, QVector<QString> &audio_name){
    duration = duration/1000;
    int start=0;
    QString tmp;
    while(start < duration){       //按300s时长进行分割音频文件
        tmp = "./record/";
        tmp.append(file_name).append(QString::number(audio_name.size(),10)).append(".wav");
        QString ffmpeg_cmd = "./ffmpeg/ffmpeg";
        ffmpeg_cmd.append(" -ss ").append(QString::number(start,10)).append(" -t ");
        if(start+300 < duration)            //若开始时间+300s小于总时长
            ffmpeg_cmd.append("300 -i ");   //从开始时间开始截取300s
        if(start+300 >= duration)      //若已经被截取过并且当前不够截取300s
            ffmpeg_cmd.append(QString::number(duration-start,10)).append(" -i ");   //截取剩余时间
        ffmpeg_cmd.append(audio_path).append(" ").append(tmp).append(" -y");
        QProcess *p = new QProcess();
        p->execute(ffmpeg_cmd);
        audio_name.push_back(tmp);
        start += 300;
    }
}

QString create_targetFile(int audio, QString file_name, QVector<QString> audio_name){
    QString mfcc_path = "./mfcc/";
    mfcc_path.append(file_name).append(QString::number(audio,10)).append(".mfcc");
    QString fileName = "./config/target/target";
    fileName.append(QString::number(audio,10)).append(".src");
    QFile file( fileName );      //新建文件target.src
    file.open( QIODevice::Text | QIODevice::WriteOnly );
    QTextStream out(&file);
    out << audio_name[audio] << " " << mfcc_path << endl;
    file.close();
    return fileName;
}

void get_mfcc(QString target_path){
    QProcess *p = new QProcess();
    QString HCopy_cmd = "./htk/HCopy -A -D -C ./config/analysis.conf -S ";
    HCopy_cmd.append(target_path);
    p->execute(HCopy_cmd);
}

QString create_scriptFile(int audio, QString file_name){
    QString mfcc_path = "./mfcc/";
    mfcc_path.append(file_name).append(QString::number(audio,10)).append(".mfcc");
    QString fileName = "./config/script/script";
    fileName.append(QString::number(audio,10)).append(".scp");
    QFile file(fileName);
    file.open( QIODevice::Text | QIODevice::WriteOnly );
    QTextStream out(&file);
    out<< mfcc_path << endl;
    file.close();
    return fileName;
}

void get_phone(QString script_path, QString file_name, int audio, QString name){
    QString mlf_path = "./result/";
    mlf_path.append(file_name).append(QString::number(audio)).append(".mlf");     //构造音频mlf文件路径名
    QString macros_path = "./config/";
    macros_path.append(name).append("/hmm/macros");     //构造macros路径
    QString hmmdefs_path = "./config/";
    hmmdefs_path.append(name).append("/hmm/hmmdefs");   //构造hmmdefs路径
    QString dict_path = "./config/";
    dict_path.append(name).append("/dict");         //构造字典dict路径
    QString triphones_path = "./config/";
    triphones_path.append(name).append("/triphones");   //构造triphones路径
    QString HVite = "./htk/HVite -H ";
    HVite.append(macros_path).append(" -H ").append(hmmdefs_path).append(" -S ").append(script_path).append(" -l * -i ");
    HVite.append(mlf_path).append(" -w ./config/wdnet -p 0.0 -s 5.0 ").append(dict_path).append(" ").append(triphones_path);
    QProcess *pHVite;
    pHVite->execute(HVite);
}

void makeGram(QString name){
    QString gram_path = "./config/";
    gram_path.append(name).append("/gram");
    QString HParse_cmd = "./htk/HParse ";
    HParse_cmd.append(gram_path).append(" ./config/wdnet");
    QProcess *HParse_pro;
    HParse_pro->execute(HParse_cmd);
}


