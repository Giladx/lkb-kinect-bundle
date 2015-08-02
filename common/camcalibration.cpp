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
#include "camcalibration.h"
#include "ui_camcalibration.h"

camcalibration::camcalibration(cameras* c,int cam1,int cam2,Mat* calMat,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::camcalibration)
{
    ui->setupUi(this);
    calM=calMat;
    cam=c;
    for(int i=0; i<cam->numContextosIniciados; i++)
        {
            Mat im,dep,msk,mix,depC(480,640,CV_8UC3),depT,pC,grayImg;
            vector<Point2f> pB;
            pointbuf.push_back(pB);
            imgs.push_back(im);
            depths.push_back(dep);
            masks.push_back(msk);
            mixs.push_back(mix);
            depthsC.push_back(depC);
            depthsT.push_back(depT);
            pCs.push_back(pC);
            grayImgs.push_back(grayImg);
            vector<Vec3d> pp;
            pCam.push_back(pp);
        }
    srand((unsigned)time(0));
    timer=new QTimer(this);
    c1=cam1;c2=cam2;
    reps=0;
    pause=false;
    doneCalib=false;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    this->setAttribute( Qt::WA_DeleteOnClose, true );
    timer->start(20);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

camcalibration::~camcalibration()
{
    if(doneCalib){

    }
    delete ui;
    timer->stop();
    delete timer;
}
void camcalibration::hacerUpdate(){

    if(cam->numContextosIniciados>1 && !pause){
        bool* update=new bool[cam->numContextosIniciados];
        int tid;
        omp_set_num_threads(2);
        int checkOptions=0;
        if(ui->useAdaptive)
            checkOptions+=CV_CALIB_CB_ADAPTIVE_THRESH;
        if(ui->useFast)
            checkOptions+=CV_CALIB_CB_FAST_CHECK;
        if(ui->useEqualization)
            checkOptions+=CV_CALIB_CB_NORMALIZE_IMAGE;
#pragma omp parallel private(tid) shared(update)
        {
            /// TODO : FIX FOR LESS AVAILABLE THREADS THAN CONTEXTS
            tid = omp_get_thread_num();
            //if(tid!=0){
            //  tid--;
            cameras* cc=cam;
            if(tid<cc->numContextosIniciados){
                if(cc->updateCam(false,tid,true,true)){
                    cc->retriveImage(tid,imgs[tid]);
                    cvtColor(imgs[tid], grayImgs[tid], CV_BGR2GRAY);
                    cvtColor(imgs[tid],imgs[tid],CV_BGR2RGB);
                    Size boardSize(ui->spinBoxNumWidth->value(),ui->spinBoxNumHeight->value());
                    bool found = findChessboardCorners( grayImgs[tid], boardSize, pointbuf[tid],checkOptions);
                    if(found){
                        update[tid]=true;
                        drawChessboardCorners( imgs[tid], boardSize, Mat(pointbuf[tid]), found );
                    }
                    else{update[tid]=false;}
                }
                else{update[tid]=false;}
            }
            //}
        }
        if(update[0] && update[1]){
            /// BUSCAR LOS PUNTOS CORRESPONDIENTES
            reps++;
            if(reps>ui->framesToPause->value()){
                ui->getNew->setEnabled(true);
                ui->savePoints->setEnabled(true);
            reps=0;
            pause=true;
            }
        }
        else{
            reps=0;
        }
        ui->glwidget1->updateImage(true,imgs[c1]);
        ui->glwidget2->updateImage(true,imgs[c2]);
        delete update;
    }
}

void camcalibration::on_clearPoints_released()
{
    pCam[0].clear();pCam[1].clear();
    ui->labelNumPoints->setText(QString::number(0));
    ui->pushButton_2->setEnabled(false);
}

void camcalibration::on_getNew_released()
{
    ui->getNew->setEnabled(false);
    ui->savePoints->setEnabled(false);
    pause=false;
}

void camcalibration::on_savePoints_released()
{
    cam->retriveDepthAndMask(c1,depths[c1],masks[c1]);
    cam->retrivePointCloudFast(c1,&(depths[c1]),&(pCs[c1]));
    cam->retriveDepthAndMask(c2,depths[c2],masks[c2]);
    cam->retrivePointCloudFast(c2,&(depths[c2]),&(pCs[c2]));
    for(int i=0;i<pointbuf[c1].size();i++){
                    Vec3f vec1=pCs[c1].at<Vec3f>((int)(pointbuf[c1][i].y),(int)(pointbuf[c1][i].x));
                    Vec3f vec2=pCs[c2].at<Vec3f>((int)(pointbuf[c2][i].y),(int)(pointbuf[c2][i].x));
                    if((vec1[0]!=0.0 || vec1[1]!=0.0 || vec1[2]!=0.0) && (vec2[0]!=0.0 || vec2[1]!=0.0 || vec2[2]!=0.0)){
                        Vec3d punto1(vec1[0],vec1[1],vec1[2]);
                        pCam[0].push_back(punto1);
                        Vec3d punto2(vec2[0],vec2[1],vec2[2]);
                        pCam[1].push_back(punto2);
                    }
                }
    ui->labelNumPoints->setText(QString::number(pCam[0].size()));
    ui->savePoints->setEnabled(false);
    ui->getNew->setEnabled(false);
    if(pCam[0].size()>10){
        ui->pushButton_2->setEnabled(true);
    }
    pause=false;
}

void camcalibration::on_pushButton_2_released()
{
    pause=true;
    if(pCam[0].size()>10)
    {
        bool continuar=true;
        ransacfitcam* RAN=new ransacfitcam();
        RAN->setData(pCam[0],pCam[1]);
        double gotThre=RAN->doRansac(0.1,0.15,20000,0.0005);
        double thrI=0.0005,por1=0.1,por2=0.15;
        int ite=20000;
        while(continuar)
        {
            ransacDialog* rD=new ransacDialog();
            rD->setThre(thrI);
            rD->setError(gotThre);
            rD->setIter(ite);
            //rD->setMaxNum(maxNum);
            rD->setPerFin(por2);
            rD->setPerRan(por1);
            int result=rD->exec();
            if(result==1)
               {
                   thrI=rD->getThre();
                   por1=rD->getPerRan();
                   por2=rD->getPerFin();
                   ite=rD->getIter();
                   //maxNum=rD->getMaxNum();
                   RAN->restart();
                   gotThre=RAN->doRansac(por1,por2,ite,thrI);
               }
            else{
                continuar=false;
            }
            delete rD;
        }
        if(gotThre<1000){
        RAN->currentGoodModel.copyTo((*calM));
        doneCalib=true;
        }
        delete RAN;
    }
   pause=false;
}
