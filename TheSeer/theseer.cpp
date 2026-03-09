#include "theseer.h"
#include "ui_theseer.h"

TheSeer::TheSeer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TheSeer)
{
    ui->setupUi(this);
}

TheSeer::~TheSeer()
{
    delete ui;
}

