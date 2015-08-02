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
#include "camerasettings2d.h"
#include "ui_camerasettings2d.h"

cameraSettings2d::cameraSettings2d(cameras *c,int numC,camconfig* cC,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cameraSettings2d)
{
    ui->setupUi(this);
    cam=c;
    for(int i=0; i<cam->numContextosIniciados; i++)
    {
        Mat im,dep,msk,mix,depC(480,640,CV_8UC3),depT,pC,depS;
        imgs.push_back(im);
        depths.push_back(dep);
        masks.push_back(msk);
        mixs.push_back(mix);
        depthsC.push_back(depC);
        depthsT.push_back(depT);
        pCs.push_back(pC);
        depthsS.push_back(depS);
    }
    nC=numC;
    alpha=0.0;
    isUpdated=cam->updateCam(false,nC);
    cConfig=cC;
    ui->planeSettings->setGeometry(ui->backSettings->geometry());
    ui->planeSettings->hide();
    ui->backSettings->hide();
    updateUI();
    if(ui->methodToUse->currentText()=="Background"){
        ui->backSettings->show();
    }
    else if(ui->methodToUse->currentText()=="Plane"){
        ui->planeSettings->show();
    }
    else{

    }
    //useColorImg=ui->checkBox->isChecked();
    zoneSelection=false;
    numSelectedPoints=0;
    BackCalibration=false;
    doPlaneCalculation=false;
    sktP=new sktprocessor();
    sktP->cConfig=cConfig;
    refiningMask=false;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    timer=new QTimer(this);
    timer->start(1);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

cameraSettings2d::~cameraSettings2d()
{
    timer->stop();
    delete timer;
    delete ui;
    delete sktP;
}

void cameraSettings2d::on_horizontalSlider_valueChanged(int value)
{
    alpha=(float)value/100;
}
bool cameraSettings2d::mixImagesToColor(){
    if(isUpdated)
    {
        cam->retriveDepthAndMask(nC,depths[nC],masks[nC]);
        cam->retriveImage(nC,imgs[nC]);
        depths[nC].convertTo(depthsT[nC],CV_8UC1,255.0/6000);
        depths[nC].convertTo(depthsS[nC],CV_32FC1,1.0/6000);
        cvtColor(depthsT[nC],depthsC[nC],CV_GRAY2RGB);
        addWeighted( depthsC[nC], alpha, imgs[nC], 1.0-alpha, 0.0, mixs[nC]);
        cvtColor(mixs[nC],mixs[nC],CV_BGR2RGB);
        return true;
    }
    return false;
}
void cameraSettings2d::camUpdate(){
    isUpdated=cam->updateCam(false,nC);
}
void cameraSettings2d::hacerUpdate(){
    ///ACA UPDATE DE TODO LO QUE SE NECESITA
    camUpdate();
    if(isUpdated){
        if(zoneSelection){
            if(numSelectedPoints<4){
                if(ui->glImage->isNewClick){
                cConfig->zone[numSelectedPoints].x=ui->glImage->lastPos.x()*640/ui->glImage->width();
                cConfig->zone[numSelectedPoints].y=ui->glImage->lastPos.y()*480/ui->glImage->height();
                //cout << cConfig->zone[numSelectedPoints].x << "," << cConfig->zone[numSelectedPoints].y << endl;
                numSelectedPoints++;
                ui->glImage->isNewClick=false;
                ui->progressBar->setValue(numSelectedPoints*25);
                }
            }
            else{
                numSelectedPoints=0;
                zoneSelection=false;
                cConfig->zoneSelected=true;
                ui->processLabel->setText("Idle process");
                ui->progressBar->setValue(0);
                ui->Instructions->setText("Nothing to do...");
                cConfig->maskZona.setTo(cv::Scalar::all(0));
                fillConvexPoly(cConfig->maskZona,cConfig->zone,4,cvScalar(255));
                Mat temp=cConfig->maskBack & cConfig->maskZona;
                ui->imgMaskBack->updateImage(false,temp);
                ui->imgMaskPlane->updateImage(false,cConfig->maskZona);
            }
        }
        if(mixImagesToColor()){
            if(BackCalibration){
                if(!cConfig->BM->proc(cConfig->variance,depthsS[nC],masks[nC],cConfig->Background,cConfig->maskBack)){
                    ui->progressBar->setValue(((float)cConfig->BM->nback/(cConfig->BM->total+1))*100);
                }
                else{
                    BackCalibration=false;
                    ui->processLabel->setText("Idle process");
                    ui->progressBar->setValue(0);
                    ui->Instructions->setText("Nothing to do...");
                    Mat temp0,temp1;
                    cConfig->Background.convertTo(temp0,CV_8UC1,255);
                    if(cConfig->zoneSelected)
                        temp1=cConfig->maskBack & cConfig->maskZona;
                    else
                        temp1=cConfig->maskBack;
                    ui->imgBack->updateImage(false,temp0);
                    ui->imgMaskBack->updateImage(false,temp1);
                }
            }
            if(zoneSelection){
                for(int cir=0;cir<numSelectedPoints;cir++)
                    circle(mixs[nC],cConfig->zone[cir],10,Scalar(0,0,255),-1);
            }
            else if(cConfig->zoneSelected){
                int *num=new int[1];
                num[0]=4;
                const Point* ptt[1] = { cConfig->zone };
                polylines(mixs[nC],ptt,num,1,1,cvScalar(255),2);
                delete num;
            }
            if(doPlaneCalculation){
                cam->retrivePointCloud(nC,depths[nC],pCs[nC]);
                cConfig->VP->calcPlanoPC(pCs[nC],cConfig->maskZona,masks[nC],ui->progressBar);
                doPlaneCalculation=false;
                ui->processLabel->setText("Idle process");
                ui->progressBar->setValue(0);
                ui->Instructions->setText("Nothing to do...");
            }
            ui->glImage->updateImage(true,mixs[nC]);
            if(cConfig->method=="Background"){
                sktP->processImage(depthsS[nC],toBlobs,masks[nC]);
            }
            else{
                cam->retrivePointCloud(nC,depths[nC],pCs[nC]);
                sktP->processImage(pCs[nC],toBlobs,masks[nC]);
            }
            ui->glBlobs->updateImage(false,toBlobs);
        }
        if(refiningMask){
            if(cConfig->method=="Background"){
                cConfig->maskBack=(cConfig->maskBack==255) & (toBlobs==0);
                cConfig->maskZona=(cConfig->maskZona==255) & (toBlobs==0);
                ui->imgMaskBack->updateImage(false,cConfig->maskBack);
            }
            else{
                cConfig->maskZona=(cConfig->maskZona==255) & (toBlobs==0);
                ui->imgMaskPlane->updateImage(false,cConfig->maskZona);
            }
        }
    }
    ////////////////////Procesamiento
    isUpdated=false;
}

void cameraSettings2d::on_methodToUse_currentIndexChanged(const QString &arg1)
{
    cConfig->method=arg1;
    if(arg1=="Background"){
        ui->backSettings->show();
        ui->planeSettings->hide();
    }
    if(arg1=="Plane"){
        ui->backSettings->hide();
        ui->planeSettings->show();
    }
}

void cameraSettings2d::on_minDistance_valueChanged(int arg1)
{
    cConfig->minDistance=arg1;
}
void cameraSettings2d::updateUI(){
    ui->dilate0->setValue(cConfig->dilIter0);
    ui->erode->setValue(cConfig->erIter);
    ui->dilate1->setValue(cConfig->dilIter1);
    ui->minDistance->setValue(cConfig->minDistance);
    ui->maxDistance->setValue(cConfig->maxDistance);
    ui->minBlobSize->setValue(cConfig->minBlobSize);
    //ui->maskMultiplier->setValue(cConfig->maskSize);
    //ui->planeMove->setValue(cConfig->planeDisplacement);
    int index=ui->methodToUse->findText(cConfig->method);
    ui->methodToUse->setCurrentIndex(index);
    ui->variance->setValue(cConfig->variance);
    if(cConfig->zoneSelected){
        Mat temp=cConfig->maskBack & cConfig->maskZona;
        Mat temp0;
        cConfig->Background.convertTo(temp0,CV_8UC1,255);
        ui->imgMaskBack->updateImage(false,temp);
        ui->imgMaskPlane->updateImage(false,cConfig->maskZona);
        ui->imgBack->updateImage(false,temp0);
        ui->imgPlane->updateImage(false,cConfig->plane);
    }
    ui->matchIandD->setChecked(cam->depthG[nC].GetAlternativeViewPointCap().IsViewPointAs(cam->image[nC]));
}

void cameraSettings2d::on_dilate0_valueChanged(int arg1)
{
    cConfig->dilIter0=arg1;
}

void cameraSettings2d::on_erode_valueChanged(int arg1)
{
    cConfig->erIter=arg1;
}

void cameraSettings2d::on_dilate1_valueChanged(int arg1)
{
    cConfig->dilIter1=arg1;
}

/*void cameraSettings2d::on_maskMultiplier_valueChanged(double arg1)
{
    cConfig->maskSize=arg1;
}*/

void cameraSettings2d::on_maxDistance_valueChanged(int arg1)
{
    cConfig->maxDistance=arg1;
}

void cameraSettings2d::on_minBlobSize_valueChanged(int arg1)
{
    cConfig->minBlobSize=arg1;
}

/*void cameraSettings2d::on_planeMove_valueChanged(double arg1)
{
    cConfig->planeDisplacement=arg1;
}
*/
void cameraSettings2d::on_zoneButton_released()
{
    zoneSelection=true;
    ui->glImage->isNewClick=false;
    ui->processLabel->setText("Waiting for you to select points");
    ui->Instructions->setText("Zone selection started.\nSelect the zone corners in the following order: \n\nUpperLeft,UpperRight,LowerRigth,LowerLeft.");
}

void cameraSettings2d::on_calBack_released()
{
    BackCalibration=true;
    cConfig->BM->start();
    ui->processLabel->setText("Background calibration...");
    ui->Instructions->setText("Wait for background calibration process.");
    cConfig->maskZona.setTo(cv::Scalar::all(0));
    fillConvexPoly(cConfig->maskZona,cConfig->zone,4,cvScalar(255));
}

void cameraSettings2d::on_variance_valueChanged(double arg1)
{
    cConfig->variance=arg1;
}

void cameraSettings2d::on_calcPlane_released()
{
    if(cConfig->zoneSelected){
        doPlaneCalculation=true;
        ui->processLabel->setText("RANSAC Plane Fitting...");
        ui->Instructions->setText("Wait for the process to finish.");
        cConfig->maskZona.setTo(cv::Scalar::all(0));
        fillConvexPoly(cConfig->maskZona,cConfig->zone,4,cvScalar(255));
    }
    else{
        ui->Instructions->setText("In order to do plane fit calculation \nyou must first select the valid zone.");
    }
}

void cameraSettings2d::on_refineMask_released()
{
    if(!refiningMask){
        ui->refineMask->setText("Stop Refining Mask");
    }
    else{
        ui->refineMask->setText("Start Refining Mask");
    }
    refiningMask=!refiningMask;
}

void cameraSettings2d::on_matchIandD_toggled(bool checked)
{
    if(checked){
    cam->userG[nC].GetAlternativeViewPointCap().SetViewPoint(cam->image[nC]);}
    else{
    cam->userG[nC].GetAlternativeViewPointCap().ResetViewPoint();}
}
