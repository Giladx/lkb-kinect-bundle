#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutdialog)
{
    ui->setupUi(this);
    ui->label_7->setOpenExternalLinks(true);
}

aboutdialog::~aboutdialog()
{
    delete ui;
}

