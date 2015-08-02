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

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

glwidget::glwidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
}
glwidget::~glwidget()
{
    //delete cam;
    timer->stop();
    delete timer;
}
void glwidget::init(cameras* c,int numC){
    cam=c;
    for(int i=0; i<cam->numContextosIniciados; i++)
    {
        Mat im,dep,msk,mix,depC(480,640,CV_8UC3),depT,pC;
        imgs.push_back(im);
        depths.push_back(dep);
        masks.push_back(msk);
        mixs.push_back(mix);
        depthsC.push_back(depC);
        depthsT.push_back(depT);
        pCs.push_back(pC);
    }
    timer=new QTimer(this);
    timer->start(1);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
    nC=numC;
}
QSize glwidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
QSize glwidget::sizeHint() const
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

void glwidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
void glwidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void glwidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}
void glwidget::hacerUpdate()
{
    updateGL();
}
void glwidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
}
void glwidget::paintGL()
{

    if(cam->updateCam(true))
    {
        cam->retriveDepthAndMask(nC,depths[nC],masks[nC]);
        cam->retrivePointCloud(nC,depths[nC],pCs[nC]);
        cam->retriveImage(nC,imgs[nC]);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear the gl buffers

        glLoadIdentity();
        //glOrtho (-WIDTH,WIDTH,HEIGHT,-HEIGHT,DEPTH,-DEPTH);
        glTranslatef(0.0, 0.0, -10.0);
        glRotatef(xRot / 16.0,1.0f,0,0);
        glRotatef(yRot / 16.0,0,1.0f,0);
        glRotatef(zRot / 16.0,0,0,1.0f);
        //glScalef(2*zoom,2*zoom,zoom);
        ///-----------------------

        glColor3ub(0,255,0);
        glBegin(GL_POINTS);
        for(int i = 0; i<depths[nC].rows; i++)
        {
            Vec3b *vecCol=(Vec3b*)(imgs[nC].data + imgs[nC].step*i);
            Vec3f *vec=(Vec3f*)(pCs[nC].data + pCs[nC].step*i);
            for(int j=0; j<depths[nC].cols; j++)
            {
                if((*vec)[0]!=0 || (*vec)[1]!=0 || (*vec)[2]!=0)
                {
                    glColor3ub((*vecCol)[2],(*vecCol)[1],(*vecCol)[0]);
                    glVertex3f((*vec)[0],(*vec)[1],(*vec)[2]);
                }			//draw the pixel
                vecCol++;vec++;
            }
        }
        glEnd();
    }
}
void glwidget::resizeGL(int width, int height)
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
void glwidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
void glwidget::mouseMoveEvent(QMouseEvent *event)
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
