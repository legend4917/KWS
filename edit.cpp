#include "edit.h"
#include <stdio.h>
#include <QStyle>

int edit(QString str1,QString str2){
    int max1 = str1.size();
    int max2 = str2.size();
    int **ptr = new int*[max1 + 1];
    for(int i = 0; i < max1 + 1 ;i++)
        ptr[i] = new int[max2 + 1];
    for(int i = 0 ;i < max1 + 1 ;i++)
        ptr[i][0] = i;
    for(int i = 0 ;i < max2 + 1;i++)
        ptr[0][i] = i;
    for(int i = 1 ;i < max1 + 1 ;i++){
        for(int j = 1 ;j< max2 + 1; j++){
            int d;
            int temp = min(ptr[i-1][j] + 1, ptr[i][j-1] + 1);
            if(str1[i-1] == str2[j-1])
                d = 0 ;
            else
                d = 1 ;
            ptr[i][j] = min(temp, ptr[i-1][j-1] + d);
        }
    }
    int dis = ptr[max1][max2];
    for(int i = 0; i < max1 + 1; i++){
        delete[] ptr[i];
        ptr[i] = NULL;
    }
    delete[] ptr;
    ptr = NULL;
    return dis;
}

int min(int a, int b){
    return a < b ? a : b;
}

void init_widgetIn(Ui::KWS *ui){
    ui->pushButton_2->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->horizontalSlider_2->setEnabled(false);
    ui->horizontalSlider_2->setValue(50);
    ui->label_6->setPixmap(QPixmap(":/image/speaker.png"));

    ui->tableWidget_2->setColumnCount(4);
    ui->tableWidget_2->setColumnWidth(0,40);
    ui->tableWidget_2->setColumnWidth(1,130);
    ui->tableWidget_2->setColumnWidth(2,130);
    ui->tableWidget_2->setColumnWidth(3,130);
    QStringList list2;
    list2 << "序号" <<"时间段" << "置信度" << "操作";
    ui->tableWidget_2->setHorizontalHeaderLabels(list2);
    ui->tableWidget_2->verticalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget_2->setFont(QFont("微软雅黑"));
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
}


void init_widgetOut(Ui::KWS *ui){
}
