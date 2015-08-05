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
    void show_result(int);
    void start();
    void finish();
    void init_widgetButton();    //初始化窗口关闭、最小化按钮

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void positionChanged(qint64);
    void durationChanged(qint64);
    void handlePlayerStateChanged(QMediaPlayer::State state);
    void setPosition(int position);
    void setVoice(int);
    void slotStarted();
    void slotFinished(int, QProcess::ExitStatus);
    void on_pushButton_6_clicked();
    void handleButtonClickEvent();
    void handlepDeleteBtnClick();
    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_pushButton_5_clicked();

    void on_listWidget_clicked(const QModelIndex &index);

    void on_lineEdit_textChanged(const QString &arg1);

    void windowclose();
    void windowmin();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    Ui::KWS *ui;
    QPoint last;    //保存鼠标的位置
    QMediaPlayer *player;
    QTimer *timer;
    int i,j,k,l,rec_number;
    QString file_name,file_mfcc_name;   //保存源音频文件mfcc
    QAudioOutput* audio_play;
    QDialog pDialog,qDialog;
    QProcess *pro;
    QGridLayout *layout_1;
    QMap<int,QPushButton*> map1,map2,map3;
    bool flag, flag1;
    QString spea_mlf_name;      //保存音频的mlf文件名

    typedef struct {
        qint64 start;
        qint64 end;
        QString phone;
        double poss;
    }result;

    result spea_mlf[10000];     //保存音频mlf文件的内容

    int dist[10][1000];     //保存距离编辑算法得到的距离
    int dist_length[10];    //保存每个拼音的dist数组的长度
    int record_th;   //当前选中的拼音序号

    QString pin[100];
    int pin_length[100];
};

#endif // KWS_H
