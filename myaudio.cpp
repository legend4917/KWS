#include "myaudio.h"
#include <QHeaderView>
#include <QFileInfo>

QString handleAudio(QString audio_path, qint64 duration, int *cnt_m, QLabel *label){
    QFileInfo fi = QFileInfo(audio_path);
    QString file_suffix = fi.suffix();
    QString file_name = fi.fileName();  //获取文件名
    int index = file_name.lastIndexOf(".");
    file_name.truncate(index);      //不带扩展名的文件名
    if(file_suffix != "wav")
        audio_path = audio_toWav(audio_path, file_name);
    QString audio_name[20], mfcc_name[20];
    label->setText("正在分割音频...");
    *cnt_m = segmentAudio(audio_path, file_name, duration, audio_name);     //音频分割
    int cnt = *cnt_m;
    qDebug() << cnt;
    for(int i=0; i<cnt-1; i++)
        qDebug() << audio_name[i];

    create_targetFile(audio_name, file_name, mfcc_name, cnt);  //创建target.src文件
    label->setText("正在提取音频特征...");
    get_mfcc();
    create_scriptFile(mfcc_name, cnt);          //创建script文件

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

int segmentAudio(QString audio_path, QString file_name, qint64 duration, QString *audio_name){
    duration = duration/1000;
    int start=0,cnt=1;
    while(start <= duration){       //按300s时长进行分割音频文件
        qDebug() << start << " " << duration;
        audio_name[cnt-1] = "./record/";
        audio_name[cnt-1].append(file_name).append(QString::number(cnt,10)).append(".wav");
        QString ffmpeg_cmd = "ffmpeg";
        ffmpeg_cmd.append(" -ss ").append(QString::number(start,10)).append(" -t ");
        if(start+300 < duration)            //若开始时间+300s小于总时长
            ffmpeg_cmd.append("300 -i ");   //从开始时间开始截取300s
        else if(cnt != 1 && start+300 >= duration)      //若已经被截取过并且当前不够截取300s
            ffmpeg_cmd.append(QString::number(duration-start,10)).append(" -i ");   //截取剩余时间
        else if(cnt == 1 && start+300 >= duration)      //若未被截取过并且当前不够截取300s
            break;      //直接跳出，免去复制过程
        ffmpeg_cmd.append(audio_path).append(" ").append(audio_name[cnt-1]).append(" -y");
        QProcess *p = new QProcess();
        p->execute(ffmpeg_cmd);
        cnt++;
        start += 300;
    }
    if(cnt == 1){
        audio_name[cnt-1] = audio_path;
        cnt++;
    }
    return cnt;
}

void create_targetFile(QString *audio_name, QString file_name, QString *mfcc_name, int cnt){
    QString fileName = "./config/target.src";
    QFile file( fileName );      //新建文件target.src
    file.open( QIODevice::Text | QIODevice::WriteOnly );
    QTextStream out(&file);
    for(int i=0; i<cnt-1; i++){
        mfcc_name[i] = "./mfcc/";
        mfcc_name[i].append(file_name).append(QString::number(i+1,10)).append(".mfcc");
        out << audio_name[i] << " " << mfcc_name[i] << endl;
    }
    file.close();
}

void get_mfcc(){
    QProcess *p = new QProcess();
    QString HCopy_cmd = "./htk/HCopy -A -D -C ./config/analysis.conf -S ./config/target.src";
    p->execute(HCopy_cmd);
}

void create_scriptFile(QString *mfcc_name, int cnt){
    QString fileName = "./config/script.scp";
    QFile file(fileName);
    file.open( QIODevice::Text | QIODevice::WriteOnly );
    QTextStream out(&file);
    for(int i=0; i<cnt-1; i++){
        out << mfcc_name[i] << endl;
    }
    file.close();
}

void makeGram(QString input){
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
}

QString get_phone(QString file_name){
    QString desc_name = "./result/";
    desc_name.append(file_name).append(".mlf");     //构造音频mlf文件路径名
    QString HVite = "./htk/HVite -H ./config/hmm/macros -H ./config/hmm/hmmdefs -S ./config/script.scp -l * -i ";
    HVite.append(desc_name).append(" -w ./config/wdnet -p 0.0 -s 5.0 ./config/dict/dict ./config/list/triphones");
    QProcess *pHVite;
    pHVite->execute(HVite);
    return desc_name;
}


