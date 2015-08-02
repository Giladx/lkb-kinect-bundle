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
#ifndef CAMCALIBRATION_H
#define CAMCALIBRATION_H

#include <QDialog>
#include <cameras.h>
#include <ransacfitcam.h>
#include <ransacdialog.h>
#include <glwidget2d.h>
#include <QGridLayout>
#include <QTimer>
#include <utilidades.h>
#include <QString>
#include <ransacfitcam.h>
#include <ransacdialog.h>

namespace Ui {
    class camcalibration;
}

class camcalibration : public QDialog
{
    Q_OBJECT

public:
    explicit camcalibration(cameras* c,int cam1,int cam2,Mat* calMat,QWidget *parent = 0);
    ~camcalibration();
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,grayImgs;
    QTimer *timer;
    int c1,c2;
    QGridLayout *layout;
    glwidget2d* gl1;
    void drawAndGetPoints(int num1, int num2,int numMin, vector<Vec3d> *sal1, vector<Vec3d> *sal2);
    //void hacerUpdate();
    vector<vector<Point2f> > pointbuf;
    int reps;
    bool pause;
    vector<vector<Vec3d> > pCam;
    bool doneCalib;
private:
    Ui::camcalibration *ui;
    cameras* cam;
    Mat* calM;
private slots:
    void hacerUpdate();
    void on_clearPoints_released();
    void on_getNew_released();
    void on_savePoints_released();
    void on_pushButton_2_released();
};

#endif // CAMCALIBRATION_H
