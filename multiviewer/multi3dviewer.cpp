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
#include "multi3dviewer.h"
#include "ui_multi3dviewer.h"

multi3dviewer::multi3dviewer(cameras* c,Mat& calMat,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::multi3dviewer)
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
    //matCal=calMat;
    pause=false;
    saveVideo=false;
    videoPlay=false;
    hasCal=false;
    savingVideo=false;
    pauseVideo=false;
    if(calMat.cols==4 && calMat.rows==3){
        matC[0][0]=calMat.at<double>(0,0);
        matC[0][1]=calMat.at<double>(0,1);
        matC[0][2]=calMat.at<double>(0,2);
        matC[0][3]=calMat.at<double>(0,3);
        matC[1][0]=calMat.at<double>(1,0);
        matC[1][1]=calMat.at<double>(1,1);
        matC[1][2]=calMat.at<double>(1,2);
        matC[1][3]=calMat.at<double>(1,3);
        matC[2][0]=calMat.at<double>(2,0);
        matC[2][1]=calMat.at<double>(2,1);
        matC[2][2]=calMat.at<double>(2,2);
        matC[2][3]=calMat.at<double>(2,3);
        hasCal=true;
    }
    frames=0;
    ui->viewer->grabKeyboard();
    bytesUs=0;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    timer=new QTimer(this);
    timer->start(10);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

multi3dviewer::~multi3dviewer()
{
    timer->stop();
    ui->viewer->releaseKeyboard();
    delete ui;
    delete timer;
}
void multi3dviewer::hacerUpdate(){
    bool update[5];
    if(cam->numContextosIniciados>0 && !pause){
        update[0]=false;
        update[1]=false;
        puntos.clear();
        colores.clear();
        int tid;
        omp_set_num_threads(2);
#pragma omp parallel private(tid) shared(update)
        {
            /// TODO : FIX FOR LESS AVAILABLE THREADS THAN CONTEXTS
            tid = omp_get_thread_num();
            cameras* cc=cam;
            if(tid<cc->numContextosIniciados){
                if(cc->updateCam(false,tid,true,true)){
                    cc->retriveDepthAndMask(tid,depths[tid],masks[tid]);
                    cc->retrivePointCloudFast(tid,&depths[tid],&pCs[tid]);
                    cc->retriveImage(tid,imgs[tid]);
                    update[tid]=true;
                }
                else{
                    update[tid]=false;
                }
            }
        }
        int count=0;bool keep=true;
        if(ui->viewCam1->isChecked() && update[0]){
            Vec3b* cols=(Vec3b*)(imgs[0].data);
            Vec3f* pts=(Vec3f*)(pCs[0].data);
            double dist=ui->maxDist1->value();
            for(int i=0;i<640*480;i++){
                if(((*pts)[0]!=0.0 || (*pts)[1]!=0.0 || (*pts)[2]!=0.0) && utilidades::vectorModule(pts)<dist){
                    if(keep){
                        colores.push_back((*cols));
                        puntos.push_back((*pts));
                        if(ui->pSkip->value()>0){
                            keep=false;
                            count++;
                        }
                    }
                    else{
                        if(count>=ui->pSkip->value()){
                            keep=true;
                            count=0;
                        }
                        else
                            count++;
                    }
                }
                cols++;pts++;
            }
        }
        keep=true;count=0;
        if(ui->viewCam2->isChecked() && update[1]){
            Vec3b* cols=(Vec3b*)(imgs[1].data);
            Vec3f* pts=(Vec3f*)(pCs[1].data);
            double dist=ui->maxDist2->value();
            for(int i=0;i<640*480;i++){
                if(((*pts)[0]!=0.0 || (*pts)[1]!=0.0 || (*pts)[2]!=0.0) && utilidades::vectorModule(pts)<dist){
                    if(ui->doRemap->isChecked() && hasCal){
                        double x=(*pts)[0],y=(*pts)[1],z=(*pts)[2];
                        (*pts)[0]=x*matC[0][0]+y*matC[0][1]+z*matC[0][2] +matC[0][3];
                        (*pts)[1]=x*matC[1][0]+y*matC[1][1]+z*matC[1][2] +matC[1][3];
                        (*pts)[2]=x*matC[2][0]+y*matC[2][1]+z*matC[2][2] +matC[2][3];
                    }
                    if(keep){
                        colores.push_back((*cols));
                        puntos.push_back((*pts));
                        if(ui->pSkip->value()>0){
                            keep=false;
                            count++;
                        }
                    }
                    else{
                        if(count>=ui->pSkip->value()){
                            keep=true;
                            count=0;
                        }
                        else
                            count++;
                    }
                }
                cols++;pts++;
            }
        }
    }
    if(saveVideo){
        saveVideo=false;
        puntosFilmacion.clear();
        coloresFilmacion.clear();
        bytesUs=0;
    }
    if(savingVideo && (update[0] || update[1])){
        puntosFilmacion.push_back(puntos);
        coloresFilmacion.push_back(colores);
        bytesUs+=sizeof(puntos)*puntos.size()+sizeof(colores)*colores.size();
        if(bytesUs>ui->maxSizeMB->value()*1000000){
        saveVideo=false;
        savingVideo=false;
        ui->videoRecBut->setText("Start Video Save");
        ui->videoPlay->setEnabled(true);
        frames=puntosFilmacion.size()-1;
        ui->videoSlider->setMaximum(frames);
        ui->videoSlider->setValue(0);
        }
    }
    if(videoPlay){
        if(!pauseVideo){
            int f=ui->videoSlider->value();
            ui->viewer->updateCloud(puntosFilmacion[f],coloresFilmacion[f]);
            ui->videoSlider->setValue((f+1)%frames);
        }
    }
    else{
        ui->viewer->updateCloud(puntos,colores);
    }
    ui->fiSize->setText(QString::number(bytesUs/1000000.0));
}

void multi3dviewer::on_keppCloud_released()
{
    if(!pause){
        pause=true;
        ui->keppCloud->setText("Continue");
    }
    else{
        pause=false;
        ui->keppCloud->setText("Keep Cloud");
    }
}

void multi3dviewer::on_videoRecBut_released()
{
    if(!savingVideo){
        saveVideo=true;
        savingVideo=true;
        ui->videoRecBut->setText("Stop");
        ui->videoPlay->setEnabled(false);
    }
    else{
        saveVideo=false;
        savingVideo=false;
        ui->videoRecBut->setText("Start Video Save");
        ui->videoPlay->setEnabled(true);
        frames=puntosFilmacion.size()-1;
        ui->videoSlider->setMaximum(frames);
        ui->videoSlider->setValue(0);
    }
}

void multi3dviewer::on_videoPlay_released()
{
    if(!videoPlay && puntosFilmacion.size()>0){
        videoPlay=true;
        pause=true;
        ui->videoPlay->setText("Live Feed");
    }
    else{
        videoPlay=false;
        pause=false;
        ui->videoPlay->setText("Play Video");
    }
}

void multi3dviewer::on_videoSlider_valueChanged(int value)
{
    if(videoPlay){
        ui->viewer->updateCloud(puntosFilmacion[value],coloresFilmacion[value]);
    }
}

void multi3dviewer::on_videoControl_released()
{
    if(!pauseVideo){
        pauseVideo=true;
        ui->videoControl->setText("Play");
    }
    else{
        pauseVideo=false;
        ui->videoControl->setText("Pause");
    }
}

void multi3dviewer::on_colR_valueChanged(double arg1)
{
    ui->viewer->updateBackColor((float)(ui->colR->value()),(float)(ui->colG->value()),(float)(ui->colB->value()));
}

void multi3dviewer::on_colG_valueChanged(double arg1)
{
    ui->viewer->updateBackColor((float)(ui->colR->value()),(float)(ui->colG->value()),(float)(ui->colB->value()));
}

void multi3dviewer::on_colB_valueChanged(double arg1)
{
    ui->viewer->updateBackColor((float)(ui->colR->value()),(float)(ui->colG->value()),(float)(ui->colB->value()));
}
