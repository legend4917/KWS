#include "init.h"
#include "stdio.h"
#include <QDialog>
#include <QMovie>

void init(Ui::KWS *ui, QDialog *qDialog){
    init_header(ui);
    init_player(ui);
    init_tableWidget(ui);
    init_comboBox(ui);
    init_qDialog(qDialog);
    init_lineEdit_show(ui);
}

void init_header(Ui::KWS *ui){
    QIcon icon;
    QPixmap pixmap0(":/image/Project.ico");
    icon.addPixmap(pixmap0);
    ui->logobtn->setIcon(icon);
    ui->logobtn->setIconSize(QSize(15,15));
}

void init_player(Ui::KWS *ui){
    QIcon icon;
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_2->setEnabled(false);
    ui->horizontalSlider_2->setValue(50);
    QPixmap voice(":/image/voice2.png");
    QPixmap scaledPixmap = voice.scaled(QSize(35,35),Qt::KeepAspectRatio);
    ui->label_6->setPixmap(scaledPixmap);
    QPixmap pixmap1(":/image/play.png");
    icon.addPixmap(pixmap1);
    ui->pushButton_2->setIcon(icon);
    ui->pushButton_2->setIconSize(QSize(40,40));
    ui->pushButton_2->setFixedSize(40,40);
    ui->pushButton_2->setCursor(Qt::PointingHandCursor);
}

void init_comboBox(Ui::KWS *ui){
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
}

void init_tableWidget(Ui::KWS *ui){
    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setColumnWidth(0,49);
    ui->tableWidget_2->setColumnWidth(1,140);
    ui->tableWidget_2->setColumnWidth(2,130);
    ui->tableWidget_2->setColumnWidth(3,130);
    QStringList list;
    list << "序号" <<"时间段(ms)" << "置信度" << "操作";
    ui->tableWidget_2->setHorizontalHeaderLabels(list);
    ui->tableWidget_2->verticalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget_2->setFont(QFont("微软雅黑"));
    ui->tableWidget_2->setMouseTracking(true);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setEditTriggers(QTableWidget::NoEditTriggers);
}

void init_qDialog(QDialog *qDialog){
    qDialog->setStyleSheet("background-color: white");
    qDialog->setFixedSize(250,100);
    qDialog->setWindowTitle("提示");
    QLabel *label = new QLabel(qDialog);
    label->setText("正在检索，请稍候...");
    label->setGeometry(QRect(30,20,180,20));
    label->setFont(QFont("'Times New Roman', Times, serif"));
    QLabel *label2 = new QLabel(qDialog);
    QMovie* movie = new QMovie(":/image/dialog.gif");
    movie->setScaledSize(QSize(188,25));
    movie->start();
    label2->setMovie(movie);
    label2->setGeometry(QRect(30,50,188,20));
}

void init_lineEdit_show(Ui::KWS *ui){
    ui->toolButton_prev->setEnabled(false);
    ui->toolButton_next->setEnabled(false);
    ui->toolButton_goto->setEnabled(false);
    ui->lineEdit_page->setEnabled(false);
}
