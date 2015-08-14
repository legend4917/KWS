#ifndef THREAD_H
#define THREAD_H
#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread(int);
    ~Thread();

signals:
    void showDialog();
    void startedit(int);

protected:
    void run();

public:
   int order;

};

#endif // THREAD_H
