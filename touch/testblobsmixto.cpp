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
#include "testblobsmixto.h"
#include "ui_testblobsmixto.h"

testblobsmixto::testblobsmixto(cameras* c,Mat& mix, vector<camconfig*> configs,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testblobsmixto)
{
    ui->setupUi(this);
    cam=c;
    for(int i=0; i<cam->numContextosIniciados; i++)
    {
        Mat im,dep,msk,mix,depC(480,640,CV_8UC3),depT,pC,depS(480,640,CV_32FC1);
        imgs.push_back(im);
        depths.push_back(dep);
        masks.push_back(msk);
        mixs.push_back(mix);
        depthsC.push_back(depC);
        depthsT.push_back(depT);
        pCs.push_back(pC);
        depthsS.push_back(depS);
        vector<Point2f> fB;
        vector<int> bS;
        finBlobs.push_back(fB);
        blobSizes.push_back(bS);
    }
    cConfig=configs;
    calMat=mix;
    sktP[0]=new sktprocessor();
    sktP[1]=new sktprocessor();
    sktP[0]->cConfig=cConfig[0];
    sktP[1]->cConfig=cConfig[1];
    toBlobs[0].create(480,640,CV_8UC1);
    toBlobs[1].create(480,640,CV_8UC1);
    real=new XnPoint3D[1000];
    proj=new XnPoint3D[1000];
    if(calMat.cols>2){
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
    }
    final.create(600,1280,CV_8UC3);
    final.setTo(cv::Scalar::all(255));
    isZoneSet=false;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    timer=new QTimer(this);
    timer->start(10);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

testblobsmixto::~testblobsmixto()
{
    timer->stop();
    delete timer;
    delete ui;
    delete sktP[0];
    delete sktP[1];
    delete real;
    delete proj;
}
void testblobsmixto::hacerUpdate(){

    vector<vector <Point> > blobs;
    Moments moms;
    if(cam->updateCam(true)){
        for(int k=0;k<cam->numContextosIniciados;k++){
            blobSizes[k].clear();
            finBlobs[k].clear();
            cam->retriveDepthAndMask(k,depths[k],masks[k]);
            cam->retrivePointCloud(k,depths[k],pCs[k]);
            depths[k].convertTo(depthsS[k],CV_32FC1,1.0/6000);
            if(cConfig[k]->method=="Background"){
                sktP[k]->processImage(depthsS[k],toBlobs[k],masks[k]);
            }
            else{
                sktP[k]->processImage(pCs[k],toBlobs[k],masks[k]);
            }
            findContours(toBlobs[k], blobs,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
            toBlobs[k].setTo(cv::Scalar::all(0));
            for(int b=0; b<blobs.size(); b++)
            {
                moms=moments(blobs[b],true);
                if(moms.m00>cConfig[k]->minBlobSize)
                {
                    Point pt((int)(moms.m10 / moms.m00),(int)(moms.m01 / moms.m00));
                    circle(toBlobs[k],pt,sqrt((double)moms.m00/3.14),cvScalar(255),-1);
                    //Save blob centers
                    finBlobs[k].push_back(pt);
                    blobSizes[k].push_back((int)moms.m00);
                }
            }
        }
        //Run through blob centers and get 3d vector
        //Convert 3d vectors to the first cam
        int count=0;
        XnPoint3D* rP=real;
        Vec3f v;
        for(int i=0;i<finBlobs[1].size();i++){
            if(count<999 && calMat.cols>2){
                v=pCs[1].at<Vec3f>(finBlobs[1][i].y,finBlobs[1][i].x);
                (*rP).X=v[0]*matC[0][0]+v[1]*matC[0][1]+v[2]*matC[0][2] +matC[0][3];
                (*rP).Y=v[0]*matC[1][0]+v[1]*matC[1][1]+v[2]*matC[1][2] +matC[1][3];
                (*rP).Z=v[0]*matC[2][0]+v[1]*matC[2][1]+v[2]*matC[2][2] +matC[2][3];
                count++;rP++;}
        }
        //Convert results to projected in first cam
        if(count >0){
            cam->depthG[0].ConvertRealWorldToProjective(count,real,proj);}
        //Check max image size and modify accordingly
        /*int maxX=0,maxY=0,minX=10000,minY=10000;
    rP=proj;
    for(int i=0;i<count;i++){
        maxX=maxX>(*rP).X ? maxX:(*rP).X;
        maxY=maxY>(*rP).Y ? maxY:(*rP).Y;
        minX=minX<(*rP).X ? minX:(*rP).X;
        minY=minY<(*rP).Y ? minY:(*rP).Y;
        rP++;
    }
    //cout << "MAXS : " << maxX << "," << maxY << endl;
    //cout << "MINS : " << minX << "," << minY << endl;
    if(!isZoneSet){
        //final.create(maxY<480?480:maxY,maxX<640?640:maxX,CV_8UC1);
        final.create(600,1280,CV_8UC3);
        maxX0=maxX;maxY0=maxY;minX0=minX;minY0=minY;
        isZoneSet=true;
        cout << "MAXS : " << maxX << "," << maxY << endl;
        cout << "MINS : " << minX << "," << minY << endl;
    }
    if(isZoneSet && (maxX>maxX0 || maxY>maxY0)){
        //final.create(maxY<480?480:maxY,maxX<640?640:maxX,CV_8UC1);
        maxX0=maxX;maxY0=maxY;minX0=minX;minY0=minY;
        cout << "MAXS : " << maxX << "," << maxY << endl;
        cout << "MINS : " << minX << "," << minY << endl;
    }*/
        final.setTo(cv::Scalar::all(255));
        //draw 1rst image blobs in enlarged image
        for(int i=0;i<finBlobs[0].size();i++){
            circle(final,finBlobs[0][i],sqrt((double)blobSizes[0][i]/3.14),cvScalar(255,0,0),-1);
        }
        //draw 2cnd image blobs in enlarged image
        for(int i=0;i<count;i++){
            if(proj[i].X>0 && proj[i].Y>0){
                circle(final,Point(proj[i].X,proj[i].Y),sqrt((double)blobSizes[1][i]/3.14),cvScalar(0,0,255),-1);}
        }
        ui->gl1->updateImage(false,toBlobs[0]);
        ui->gl2->updateImage(false,toBlobs[1]);
        ui->glmixto->updateImage(true,final);
    }
}
