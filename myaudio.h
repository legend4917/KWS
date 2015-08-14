#ifndef MYAUDIO
#define MYAUDIO
#include <QMediaPlayer>
#include <QFile>
#include <QAudioInput>
#include <QAudioOutput>
#include <QProcess>
#include "ui_kws.h"


QString handleAudio(QString audio_path, qint64 time, int *cnt_m, QLabel *label);         //音频预处理
int segmentAudio(QString, QString, qint64, QString *);
void create_targetFile(QString *audio_name, QString file_name, QString *mfcc_name, int cnt);
QString audio_toWav(QString, QString);
void get_mfcc();
void create_scriptFile(QString *mfcc_name, int cnt);
void makeGram(QString input);
QString get_phone(QString file_name);


#endif // MYAUDIO
