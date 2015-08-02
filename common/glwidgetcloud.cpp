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
#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidgetcloud.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

glwidgetcloud::glwidgetcloud(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom=0.4;
    xTr=0;
    yTr=0;
    zTr=0;
    cP[0]=0.0;
    cP[1]=0.0;
    cP[2]=DEPTH*zoom;
    cPP[0]=cP[0];
    cPP[1]=cP[1];
    cPP[2]=cP[2];
}
glwidgetcloud::~glwidgetcloud()
{
}
QSize glwidgetcloud::minimumSizeHint() const
{
    return QSize(50, 50);
}
QSize glwidgetcloud::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}
void glwidgetcloud::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
void glwidgetcloud::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void glwidgetcloud::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}
void glwidgetcloud::updateCloud(vector<Vec3f> pts, vector<Vec3b> colors){
    pC=pts;
    img=colors;
    updateGL();
}
void glwidgetcloud::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void glwidgetcloud::paintGL()
{
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear the gl buffers

        glLoadIdentity();
        //glOrtho (-WIDTH,WIDTH,HEIGHT,-HEIGHT,DEPTH,-DEPTH);
        glTranslatef(0.0, 0.0, cPP[2]);
        glScalef(zoom,zoom,zoom);
        glRotatef(xRot / 16.0,1.0f,0,0);
        glRotatef(yRot / 16.0,0,1.0f,0);
        glRotatef(zRot / 16.0+180.0,0,0,1.0f);
        glTranslatef(0.0, 0.0, -cPP[2]);
        //glTranslatef(0.0, 0.0, -6000*zoom);
        ///-------------------------------------------------
        glTranslatef(xTr, yTr, zTr);
        glColor3ub(0,255,0);
        glBegin(GL_POINTS);
        if(img.size()>0 && pC.size()>0 && pC.size()==img.size()){
            cP[2]=0.0;cP[0]=0.0;cP[1]=0.0;
            vector<Vec3b>::iterator itC=img.begin();
            for(vector<Vec3f>::iterator it=pC.begin();it!=pC.end();it++){
                glColor3ub((*itC)[2],(*itC)[1],(*itC)[0]);
                glVertex3f((*it)[0],(*it)[1],(*it)[2]);
                if(itC!=img.end()){
                    itC++;
                }
                cP[0]+=(*it)[0];
                cP[1]+=(*it)[1];
                cP[2]+=(*it)[2];
            }
            cP[0]/=pC.size();
            cP[1]/=pC.size();
            cP[2]/=pC.size();
            int dii=100;
            if(abs(cP[0]-cPP[0])>dii || abs(cP[1]-cPP[1])>dii || abs(cP[1]-cPP[1])>dii){
                cPP[0]=cP[0];
                cPP[1]=cP[1];
                cPP[2]=cP[2];
            }
        }
        glEnd();
}
void glwidgetcloud::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof (-WIDTH,WIDTH,HEIGHT,-HEIGHT,DEPTH,-DEPTH);
#else
    glOrtho (-WIDTH,WIDTH,HEIGHT,-HEIGHT,DEPTH,-DEPTH);
#endif
    glMatrixMode(GL_MODELVIEW);
}
void glwidgetcloud::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
void glwidgetcloud::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}
void glwidgetcloud::wheelEvent(QWheelEvent *event){
    if(event->orientation() == Qt::Vertical)
       {
           //cout << "WHEEL : " << event->delta() << endl;
           zoom+=double(event->delta())/10000;
           if(zoom<0.001){
               zoom=0.001;
           }
           event->Quit;
           //cout << "ZOOM : " << zoom<< endl;
           updateGL();
       }
}
void glwidgetcloud::keyPressEvent(QKeyEvent *event){
    cout << "KEY : " << event->key() << endl;
    switch(event->key())
    {
        case Qt::Key_Left:
        xTr+=50;
        break;
        case Qt::Key_Right:
        xTr-=50;
        break;
        case Qt::Key_Up:
        yTr+=50;
        break;
        case Qt::Key_Down:
        yTr-=50;
        break;
    }
    updateGL();
    //cout << xTr << "," << yTr << "," << zTr << endl;
}
void glwidgetcloud::updateBackColor(float R, float G, float B){
    glClearColor(R, G, B, 1.0f);
    updateGL();
}
