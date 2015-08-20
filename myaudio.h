#ifndef MYAUDIO
#define MYAUDIO
#include <QMediaPlayer>
#include <QFile>
#include <QAudioInput>
#include <QAudioOutput>
#include <QProcess>
#include <QVector>
#include "ui_kws.h"


QString handleAudio(QString audio_path, qint64 time, QVector<QString> &audio_name);         //音频预处理
void segmentAudio(QString, QString, qint64, QVector<QString> &audio_name);
QString create_targetFile(int audio, QString file_name, QVector<QString> audio_name);
QString audio_toWav(QString, QString);
void get_mfcc(QString target_path);
QString create_scriptFile(int audio, QString file_name);
void makeGram(QString name);
void get_phone(QString script_path, QString file_name, int audio, QString name);


#endif // MYAUDIO
