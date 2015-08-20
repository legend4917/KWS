#ifndef KWS_H
#define KWS_H

#include "mydialog.h"
#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QAudio>
#include <QTextStream>
#include <QFileDialog>
#include <QFileInfo>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QProgressDialog>
#include <QGridLayout>
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <QToolButton>
#include <QPalette>
#include <QButtonGroup>
#include <QThread>
#include <QRadioButton>
#include <QTime>


namespace Ui {
class KWS;
}

class KWS : public QWidget
{
    Q_OBJECT

public:    
    explicit KWS(QWidget *parent = 0);
    ~KWS();
    void init_widgetButton();    //初始化窗口关闭、最小化按钮
    void init_dict();
    bool isInDict(QString);
    void getKeyWord();
    void readMLF();
    void select_result();
    void readList();
    void init_dialog();

private slots:
    //播放器模块
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void positionChanged(qint64);
    void durationChanged(qint64);
    void handlePlayerStateChanged(QMediaPlayer::State state);
    void setPosition(int position);
    void setVoice(int);
    void handleSliderPressed();

    //写入待检索拼音模块
    void on_pushButton_5_clicked();
    void handlepDeleteBtnClick();
    void on_listWidget_clicked(const QModelIndex &index);

    //检索模块
    void on_pushButton_6_clicked();
    void showDialog();
    void closeDialog();
    void readySearch();
    void startInitFile(int);
    void startSearch(int);
    void countNum();

    //置信度调节模块
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_comboBox_2_currentTextChanged(const QString &arg1);

    //显示检索结果模块
    void show_result();
    void handleButtonClickEvent();
    void on_toolButton_next_clicked();
    void on_toolButton_prev_clicked();
    void on_toolButton_goto_clicked();

    //初始化窗口组件模块
    void windowclose();
    void windowmin();
    void on_lineEdit_textChanged(const QString &arg1);

    void addPatterm();
    void handleSubmit();
    void handleSelect();
    void handleRadioClick();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);

public:
    Ui::KWS *ui;
    QPoint last;                //保存鼠标的位置
    QMediaPlayer *player;
    qint64 durat;
    int cnt,threadNum;                    //音频数量
    QThread *mainThread;
    QString audio_path;         //保存音频文件路径
    QString file_name;          //保存音频文件名
    QVector<QString> audio_name;

    QTime start_time,end_time;
    mydialog qDialog;
    QDialog *dialog;
    bool flag;
    bool inArea, ok;
    QVector<QString> dict;

    QButtonGroup *getModeSelect;
    QString name;
    typedef struct {
        qint64 start;
        qint64 end;
        QString phone;
        double poss;
    }result;

    QVector<result> spea_mlf;
    QVector<int> dist[10];      //保存当前每个关键词的距离
    int m;                      //当前选中的拼音序号

    QString pin[100];
    int pin_length[100];

    QVector<int> sele;
    int total_page;             //当前的总页数
    int current_page;           //当前显示的页数
};

#endif // KWS_H
