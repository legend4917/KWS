#ifndef KWS_H
#define KWS_H

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
    QString getKeyWord();
    void readMLF(QString);
    void select_result();


private slots:
    //播放器模块
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void positionChanged(qint64);
    void durationChanged(qint64);
    void handlePlayerStateChanged(QMediaPlayer::State state);
    void setPosition(int position);
    void setVoice(int);

    //写入待检索拼音模块
    void on_pushButton_5_clicked();
    void handlepDeleteBtnClick();
    void on_listWidget_clicked(const QModelIndex &index);

    //检索模块
    void on_pushButton_6_clicked();
    void showDialog();
    void closeDialog();
    void readySearch();
    void startSearch(int);

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

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);

public:
    Ui::KWS *ui;
    QPoint last;                //保存鼠标的位置
    QMediaPlayer *player;
    qint64 durat;
    int cnt;                    //音频数量
    QString audio_path;         //保存音频文件路径
    QDialog qDialog;
    bool flag;
    bool flag1, ok;
    QString dict[1000];
    int dict_length;

    int spea_length;            //保存音频文件内容的长度

    typedef struct {
        qint64 start;
        qint64 end;
        QString phone;
        double poss;
    }result;

    result spea_mlf[10000];     //保存音频mlf文件的内容

    int dist[10][1000];         //保存距离编辑算法得到的距离
    int dist_length[10];        //保存每个拼音的dist数组的长度
    int m;                      //当前选中的拼音序号

    QString pin[100];
    int pin_length[100];

    int sele[10000];            //根据置信区间筛选的编号
    int total_page;             //当前的总页数
    int current_page;           //当前显示的页数
    int count_line;             //筛选得到的总数据数
};

#endif // KWS_H
