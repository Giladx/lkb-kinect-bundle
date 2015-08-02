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
#ifndef SURFACECAMCALIBRATION_H
#define SURFACECAMCALIBRATION_H

#include <QWidget>
#include <cameras.h>
#include <ransacfitcam.h>
#include <ransacdialog.h>
#include <glwidget2d.h>
#include <QGridLayout>
#include <QTimer>
#include <utilidades.h>
namespace Ui {
    class surfacecamcalibration;
}

class surfacecamcalibration : public QWidget
{
    Q_OBJECT

public:
    explicit surfacecamcalibration(cameras* c,int cam1,int cam2,Mat* calMat,QWidget *parent = 0);
    ~surfacecamcalibration();
    void printSizes();
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,grayImgs;
    QTimer *timer;
    int c1,c2;
    QGridLayout *layout;
    glwidget2d* gl1;
    void drawAndGetPoints(int num1, int num2,int numMin, vector<Vec3d> *sal1, vector<Vec3d> *sal2);
    //void hacerUpdate();
    int numPMin;
private:
    Ui::surfacecamcalibration *ui;
    cameras* cam;
    Mat* calM;
private slots:
    void hacerUpdate();
};

#endif // SURFACECAMCALIBRATION_H
