/*
LKB (Ludique's Kinect Bundle)

Version 0.1 Copyright (C) 2012 Ludique

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications,
and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software in
       a product, an acknowledgment in the product documentation would be appreciated
       but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.*/
#include "ransacdialog.h"
#include "ui_ransacdialog.h"

ransacDialog::ransacDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ransacDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
}

ransacDialog::~ransacDialog()
{
    delete ui;
}
void ransacDialog::setError(double val){
    QString PP;
    PP.setNum(val);
    ui->error->setText(PP);
}
void ransacDialog::setThre(double val){
    ui->thresholdId->setValue(val);
}
void ransacDialog::setMaxNum(int val){
    ui->maxNum->setValue(val);
}
void ransacDialog::setPerFin(double val){
    ui->finalPercent->setValue(val*100);
}
void ransacDialog::setPerRan(double val){
    ui->randomPercent->setValue(val*100);
}
void ransacDialog::setIter(int val){
    ui->Iterations->setValue(val);
}
int ransacDialog::getIter(){
    return ui->Iterations->value();
}
int ransacDialog::getMaxNum(){
    return ui->maxNum->value();
}
double ransacDialog::getThre(){
    return ui->thresholdId->value();
}
double ransacDialog::getPerRan(){
    return ui->randomPercent->value()/100.0;
}
double ransacDialog::getPerFin(){
    return ui->finalPercent->value()/100.0;
}
