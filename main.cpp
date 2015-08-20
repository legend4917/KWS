#include "kws.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("Project.ico"));
    KWS w;
    w.setFixedSize(1000,675);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setGeometry(180,60,1000,675);
    w.show();
    QFile styleFile(":/style/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    qApp->setStyleSheet(styleSheet);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    return a.exec();
}
