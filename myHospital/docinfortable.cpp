#include "docinfortable.h"
#include "ui_docinfortable.h"

DocInforTable::DocInforTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DocInforTable)
{
    ui->setupUi(this);
}

DocInforTable::~DocInforTable()
{
    delete ui;
}
