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
#ifndef CAMERASETTINGS2d_H
#define CAMERASETTINGS2d_H

#include <QDialog>
#include <QTimer>
#include <cameras.h>
#include <camconfig.h>
#include <sktprocessor.h>

namespace Ui {
    class cameraSettings2d;
}

class cameraSettings2d : public QDialog
{
    Q_OBJECT

public:
    explicit cameraSettings2d(cameras *c,int numC,camconfig* cC,QWidget *parent = 0);
    ~cameraSettings2d();
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,depthsS;
    QTimer *timer;
    int nC;
    float alpha;
    bool isUpdated;
    void camUpdate();
    camconfig* cConfig;
    Ui::cameraSettings2d *ui;
    sktprocessor* sktP;
    Mat toBlobs;
private slots:
    void on_horizontalSlider_valueChanged(int value);
    void hacerUpdate();
    void on_methodToUse_currentIndexChanged(const QString &arg1);
    void on_minDistance_valueChanged(int arg1);
    void on_dilate0_valueChanged(int arg1);
    void on_erode_valueChanged(int arg1);
    void on_dilate1_valueChanged(int arg1);
   // void on_maskMultiplier_valueChanged(double arg1);
    void on_maxDistance_valueChanged(int arg1);
    void on_minBlobSize_valueChanged(int arg1);
   // void on_planeMove_valueChanged(double arg1);
    void on_zoneButton_released();

    void on_calBack_released();

    void on_variance_valueChanged(double arg1);

    void on_calcPlane_released();

    void on_refineMask_released();

    void on_matchIandD_toggled(bool checked);

private:

    cameras* cam;
    bool mixImagesToColor();
    bool useColorImg;
    void updateUI();
    bool zoneSelection;
    bool BackCalibration;
    int numSelectedPoints;
    bool doPlaneCalculation;
    bool refiningMask;
};

#endif // CAMERASETTINGS_H
