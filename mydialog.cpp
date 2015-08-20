#include "mydialog.h"

mydialog::mydialog(){
    this->setStyleSheet("background-color: white");
    this->setFixedSize(250,100);
    this->setWindowTitle("提示");
    QLabel *label = new QLabel(this);
    label->setText("正在检索，请稍候...");
    label->setGeometry(QRect(30,20,180,20));
    label->setFont(QFont("'Times New Roman', Times, serif"));
    QLabel *label2 = new QLabel(this);
    QMovie* movie = new QMovie(":/image/dialog.gif");
    movie->setScaledSize(QSize(188,25));
    movie->start();
    label2->setMovie(movie);
    label2->setGeometry(QRect(30,50,188,20));
    flag = true;
}

mydialog::~mydialog(){

}


void mydialog::closeEvent(QCloseEvent *event){
    if(thread->isRunning()){
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("退出程序"),
            QString(tr("警告：程序正在运行，是否确定终止？")),
            QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::No) {
            if(thread->isFinished()){
                qDebug() << "isFinished";
                event->accept();
            }else{
                qDebug() << "isRunning";
                event->ignore();  //忽略退出信号，程序继续运行
            }
        }
        else {
            flag = false;
            if(thread->isRunning())
                thread->terminate();
            event->accept();  //接受退出信号，程序退出
        }
    }
}
