#include "selectsearchsource.h"
#include "ui_selectsearchsource.h"

SelectSearchSource::SelectSearchSource(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectSearchSource)
{
    ui->setupUi(this);
}

SelectSearchSource::~SelectSearchSource()
{
    delete ui;
}
