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
#ifndef RANSACDIALOG_H
#define RANSACDIALOG_H

#include <QDialog>

namespace Ui {
    class ransacDialog;
}

class ransacDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ransacDialog(QWidget *parent = 0);
    ~ransacDialog();
    double getThre();
    double getPerFin();
    double getPerRan();
    int getIter();
    int getMaxNum();
    void setThre(double val);
    void setPerFin(double val);
    void setPerRan(double val);
    void setIter(int val);
    void setMaxNum(int val);
    void setError(double val);

private:
    Ui::ransacDialog *ui;
};

#endif // RANSACDIALOG_H
