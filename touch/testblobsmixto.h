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
#ifndef TESTBLOBSMIXTO_H
#define TESTBLOBSMIXTO_H

#include <QDialog>
#include <QTimer>
#include <cameras.h>
#include <camconfig.h>
#include <sktprocessor.h>
#include <vector>

using namespace std;
using namespace cv;
using namespace xn;

namespace Ui {
    class testblobsmixto;
}

class testblobsmixto : public QDialog
{
    Q_OBJECT

public:
    explicit testblobsmixto(cameras* c,Mat& mix, vector<camconfig*> configs,QWidget *parent = 0);
    ~testblobsmixto();
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,depthsS;
    QTimer *timer;
    vector<camconfig*> cConfig;
    sktprocessor* sktP[2];
    Mat toBlobs[2];
    cameras* cam;
    Mat calMat,final;
    XnPoint3D* real;
    XnPoint3D* proj;
    double matC[3][4];
    int maxX0,maxY0,minX0,minY0;
    bool isZoneSet;
    vector<vector < Point2f > > finBlobs;
    vector<vector < int > > blobSizes;
private:
    Ui::testblobsmixto *ui;
private slots:
        void hacerUpdate();

};

#endif // TESTBLOBSMIXTO_H
