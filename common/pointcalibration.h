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
#ifndef POINTCALIBRATION_H
#define POINTCALIBRATION_H

#include <QWidget>
#include <cameras.h>
#include <glwidget2d.h>
#include <QGridLayout>
#include "QDesktopWidget"
#include "QRect"
#include <utilidades.h>
#include <QPlainTextEdit>
#include <instructions.h>
#include <utilidades.h>
#include <calibrationskt.h>
#include <datacollector.h>
#include <QMessageBox>
#include <qtimagewidget.h>

namespace Ui {
    class pointcalibration;
}

class pointcalibration : public QWidget
{
    Q_OBJECT

public:
    explicit pointcalibration(calibrationskt* cal, datacollector* datc,int numP,double distBor,int numFrames,bool* working=NULL,QWidget *parent = 0);
    ~pointcalibration();

    Point2f zone[4];
    bool doingZoneSelection;
    bool zoneSelected;
    bool fullscreen;
    int numPoints;
    int numSelectedPoints;
    int numCalibrating;
    int numFramesToUsable;
    int numFramesGone;
    bool received0;
    double xAvrg,yAvrg;
    bool selectingRectangle;
    double distanceFromBorder;
    bool* work;
    Mat back;
    QRect screen;
    vector<Point2f> pointGrid;
    calibrationskt* calib;
    datacollector* dc;
    ///UI
    //QGridLayout *layo;

    qtimagewidget* gl1;
    //glwidget2d* gl1;
    instructions* in;
    /////
    bool doingCalibration;

    ////FOR DATA INPUT
    static pointcalibration* instance;
    static void onup(int id, float x, float y){
        //cout << "Update : " << id << "," << x << "," << y << endl;
        instance->onUpdate(id,x,y);
    }
    static void onde(int id){
        //cout << "Remove : " << id << endl;
        instance->onDelete(id);
    }
    static void onad(int id, float x, float y){
        //cout << "Add : " << id << "," << x << "," << y << endl;
        instance->onAdd(id,x,y);
    }
    static void newClick(QPoint pos){
        instance->nClick(pos);
    }
    void nClick(QPoint pos);
    void onUpdate(int id, float x, float y);
    void onDelete(int id);
    void onAdd(int id, float x, float y);
    vector<Point2f> createPointArray(int nP, Point2f *zo,double sep);
    void updateCalibrationImage(Mat& img,int numPC,double color);
    /////
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::pointcalibration *ui;
};

#endif // POINTCALIBRATION_H
