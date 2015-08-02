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
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <cameras.h>

#define WIDTH   1300
#define HEIGHT  1000
#define DEPTH	6000

class glwidget : public QGLWidget
{
    Q_OBJECT

public:
    glwidget(QWidget *parent = 0);
    ~glwidget();
    void init(cameras* cam,int numC);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    cameras *cam;
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs;
    QTimer *timer;
    int nC;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void hacerUpdate();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor qtGreen;
    QColor qtPurple;
};

#endif
