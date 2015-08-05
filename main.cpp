#include "kws.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KWS w;
    a.setWindowIcon(QIcon("Project.ico"));
    w.setWindowTitle("音频检索系统");
    w.setFixedSize(1000,650);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setGeometry(180,60,1000,650);
    w.show();
    return a.exec();
}
