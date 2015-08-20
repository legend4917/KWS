#ifndef MYDIALOG_H
#define MYDIALOG_H
#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
#include <QDebug>
#include <QLabel>
#include <QMovie>
#include <QVector>


class mydialog : public QDialog
{
    Q_OBJECT
public:
    mydialog();
    ~mydialog();
    void closeEvent(QCloseEvent *);

    QThread *thread;
    bool flag;

};

#endif // MYDIALOG_H
