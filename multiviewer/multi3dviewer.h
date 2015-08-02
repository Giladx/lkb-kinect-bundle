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
#ifndef MULTI3DVIEWER_H
#define MULTI3DVIEWER_H

#include <QDialog>
#include <QTimer>
#include <QString>
#include <cameras.h>
#include <utilidades.h>

namespace Ui {
    class multi3dviewer;
}

class multi3dviewer : public QDialog
{
    Q_OBJECT

public:
    explicit multi3dviewer(cameras* c,Mat& calMat,QWidget *parent = 0);
    ~multi3dviewer();
    cameras* cam;
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,depthsS;
    QTimer *timer;
    double matC[3][4];
    bool videoPlay;
    bool saveVideo;
    bool savingVideo;
    bool pause;
    bool hasCal;
    bool pauseVideo;
    vector<Vec3f> puntos;
    vector<Vec3b> colores;
    vector< vector<Vec3f> > puntosFilmacion;
    vector< vector<Vec3b> > coloresFilmacion;
    int frames;
    double bytesUs;
private:
    Ui::multi3dviewer *ui;
private slots:
    void hacerUpdate();
    void on_keppCloud_released();
    void on_videoRecBut_released();
    void on_videoPlay_released();
    void on_videoSlider_valueChanged(int value);
    void on_videoControl_released();
    void on_colR_valueChanged(double arg1);
    void on_colG_valueChanged(double arg1);
    void on_colB_valueChanged(double arg1);
};

#endif // MULTI3DVIEWER_H
