#ifndef MYAUDIO
#define MYAUDIO
#include <QMediaPlayer>
#include <QFile>
#include <QAudioInput>
#include <QAudioOutput>
#include <QProcess>
#include "ui_kws.h"
#include "kws.h"

const QString HCopy_cmd = "./htk/HCopy -A -D -C ./config/analysis.conf -S ./config/target.src";


void init_widget(Ui::KWS *ui);
void create_file(QString audio_path,QString file_name);
QString audio_toWav(QString, QProcess *pro);
void get_mfcc(QString file_name,QProcess *pro);
void init_player(QMediaPlayer *player,QString file_path);
void playorPause(QMediaPlayer *player,Ui::KWS *ui);
void slotStarted();
void slotFinished(int, QProcess::ExitStatus);
void rawTOwav(int);
void get_record_mfcc(int);
void get_phone(QString path_name,QString desc_name);


#endif // MYAUDIO
