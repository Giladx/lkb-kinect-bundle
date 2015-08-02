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
#ifndef GLWIDGET2D_H
#define GLWIDGET2D_H

#include <QGLWidget>
#include <cameras.h>

class glwidget2d : public QGLWidget
{
    Q_OBJECT

public:
    glwidget2d(QWidget *parent = 0);
    ~glwidget2d();
    //QSize minimumSizeHint() const;
    //QSize maximumSizeHint() const;
    //QSize sizeHint() const;
    void updateImage(bool color,Mat& img, double scale=1.0);
    QPoint lastPos;
    QPoint globalLastPos;
    bool isNewClick;
    bool isResized;
    void resizeGL(int width, int height);
    void (*onNewClick)(QPoint pos);
    bool useglfix;
    float widthI,heightI;
protected:
    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
private:
    bool color;
    Mat image;
};

#endif
