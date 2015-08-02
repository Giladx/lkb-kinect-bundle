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
#ifndef SKTINFOEXTRACTOR_H
#define SKTINFOEXTRACTOR_H

#include <QDialog>
#include <QTimer>
#include <cameras.h>
#include <camconfig.h>
#include <sktprocessor.h>
#include <vector>
#include <tuiodatasender.h>
#include <QMessageBox>
#include <datacollector.h>
#include <calibrationskt.h>
#include <pointcalibration.h>
#ifdef _WIN32 || _WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <omp.h>
#include <tuiomousewrapper.h>

//#define OMP_NUM_THREADS 2

using namespace std;
using namespace cv;
using namespace xn;
namespace Ui {
    class sktinfoextractor;
}

class sktinfoextractor : public QDialog
{
    Q_OBJECT

public:
    explicit sktinfoextractor(cameras* c,vector<Mat> mix, vector<camconfig*> configs,calibrationskt* cal,tuiomousewrapper* tm,string hst="localhost",int prt=3333,bool autostart=false, QWidget *parent = 0);
    ~sktinfoextractor();
    vector<Mat> imgs,depths,masks,mixs,depthsC,depthsT,pCs,depthsS;
    QTimer *timer;
    vector<camconfig*> cConfig;
    vector<sktprocessor*> sktP;
    vector<Mat> toBlobs;
    cameras* cam;
    Mat calMat;
    XnPoint3D* real;
    XnPoint3D* proj;
    double matC[3][4];
    vector<vector < Point2f > > finBlobs;
    vector<vector < int > > blobSizes;
    bool hasCloseBlob(XnPoint3D* rP,int count,Point2f p,double minDist);
    bool hasCloseBlob2(XnPoint3D *r, vector<Point2f> vec,double minDist);
    double mD;
    bool sendTuio;
    tuiodatasender* tds;
    bool isServerCreated;
    datacollector* dc;
    bool isDCCreated,useDC;
    calibrationskt* calSKT;
    bool is2CamCalib;
#ifdef _WIN32 || _WIN64
    SYSTEMTIME inicio;
#else
    struct timeval inicio;
#endif
    int numFrames;
    int nthreads;
    tuiomousewrapper* tmw;
    double minDataUpdate;
    static sktinfoextractor* instance;
    static void onup(int id, float x, float y){
        cout << "Desde sktinfo : " << id << "," << x << "," << y << " ---- " << instance->sendTuio << endl;
    }
    bool isProcessing;
    //void doProcess();
    vector<vector <Point> > blobs[10];

private:
    Ui::sktinfoextractor *ui;
private slots:
    void hacerUpdate();
    void on_pushButton_released();
    void on_doubleSpinBox_valueChanged(double arg1);
    void on_sendTuioCheck_toggled(bool checked);
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);

    void on_pointCalStart_released();
    void on_tuioMouse_toggled(bool checked);
    void on_checkBox_3_toggled(bool checked);
    void on_minDistanceToUpdateBox_valueChanged(double arg1);
    void on_framesPointBox_valueChanged(int arg1);
};

#endif // SKTINFOEXTRACTOR_H
