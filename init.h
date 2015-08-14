#ifndef INIT
#define INIT
#include "ui_kws.h"
#include <QStyle>

void init(Ui::KWS *ui, QDialog *qDialog);
void init_header(Ui::KWS *ui);
void init_player(Ui::KWS *ui);
void init_comboBox(Ui::KWS *ui);
void init_tableWidget(Ui::KWS *ui);
void init_tableout(Ui::KWS *ui);
void init_pDialog(QDialog *pDialog);
void init_qDialog(QDialog *qDialog);
void init_tableout(Ui::KWS *ui, QLabel *tableout);
void init_lineEdit_show(Ui::KWS *ui);

#endif // INIT

