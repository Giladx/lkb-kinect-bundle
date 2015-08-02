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
#ifndef GLWIDGETCLOUD_H
#define GLWIDGETCLOUD_H

#include <QGLWidget>
#include <cameras.h>

#define WIDTH   1300																			//width and height of the window
#define HEIGHT  1000
#define DEPTH	6000

class glwidgetcloud : public QGLWidget
{
    Q_OBJECT

public:
    glwidgetcloud(QWidget *parent = 0);
    ~glwidgetcloud();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updateCloud(vector<Vec3f> pts,vector<Vec3b> colors);
    void updateBackColor(float R, float G, float B);
public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

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
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    int xRot;
    int yRot;
    int zRot;
    int xTr,yTr,zTr;
    float cP[3],cPP[3];
    float zoom;
    QPoint lastPos;
    QColor qtGreen;
    QColor qtPurple;
    vector<Vec3f> pC;
    vector<Vec3b> img;
    float RGB[3];
};

#endif
