#ifndef KWS_H
#define KWS_H

#include <QWidget>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QAudio>
#include <QTextStream>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QProgressDialog>
#include <QGridLayout>
#include <iostream>
#include <QMap>
namespace Ui {
class KWS;
}

class KWS : public QWidget
{
    Q_OBJECT

public:    
    explicit KWS(QWidget *parent = 0);
    ~KWS();
    void start_record(QString);
    void stop_record(QString);
    void play_record(int);
    void show_result(int);
    void start();
    void finish();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void positionChanged(qint64);
    void durationChanged(qint64);
    void handlePlayerStateChanged(QMediaPlayer::State state);
    void setPosition(int position);
    void setVoice(int);
    void handleRecordStateChanged(QAudio::State);
    void displayTime();
    void finish_play(QAudio::State state);
    void slotStarted();
    void slotFinished(int, QProcess::ExitStatus);
    void on_pushButton_6_clicked();
    void handleButtonClickEvent();
    void handleButton1ClickEvent();
    void handleButton2ClickEvent();
    void handleButton3ClickEvent();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_pushButton_5_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_listWidget_clicked(const QModelIndex &index);

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::KWS *ui;
    QMediaPlayer *player;
    QTimer *timer;
    int i,j,k,l,rec_number;
    QString file_name,file_mfcc_name;   //保存源音频文件mfcc
    QFile outputFile;   //录音文件
    QAudioInput* audio_record;
    QFile inputFile;    //读取录音文件
    QAudioOutput* audio_play;
    QDialog pDialog,qDialog;
    QProcess *pro;
    QString rec_name[100];  //录音名
    bool flag[100];     //记录录音文件是否被删除
    QGridLayout *layout_1;
    QMap<int,QPushButton*> map1,map2,map3;

    QString rec_mlf_name[100];  //保存录音的mlf文件名
    QString spea_mlf_name;      //保存音频的mlf文件名

    typedef struct {
        qint64 start;
        qint64 end;
        QString phone;
        double poss;
    }result;

    result spea_mlf[10000];     //保存音频mlf文件的内容
    result recd_mlf[10][2000];  //保存录音mlf文件的内容

    QString recd_mlf_str[10];   //保存每个录音文件内容拼接的字符串
    int recd_length[10];     //保存每个录音文件内容的长度

    int dist[10][1000];     //保存距离编辑算法得到的距离
    int dist_length[10];    //保存每个录音文件的dist数组的长度
    int record_th;   //当前选中的录音文件序号

    QString pin[100];
    int pin_length[100];
};

#endif // KWS_H
