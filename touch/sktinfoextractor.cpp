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
#include "sktinfoextractor.h"
#include "ui_sktinfoextractor.h"
sktinfoextractor *sktinfoextractor::instance = 0;
sktinfoextractor::sktinfoextractor(cameras* c,vector<Mat> mix, vector<camconfig*> configs,calibrationskt* cal,tuiomousewrapper* tm,string hst,int prt,bool autostart,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sktinfoextractor)
{
    ui->setupUi(this);
    cam=c;
    cConfig=configs;
    calMat=mix[0];
    mD=ui->doubleSpinBox->value();
    isDCCreated=false;
    useDC=false;
    instance=this;
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
        sktprocessor* sP=new sktprocessor();
        sP->cConfig=cConfig[i];
        sktP.push_back(sP);
        toBlobs.push_back(Mat(480,640,CV_8UC1));
        vector<Point2f> fB;
        vector<int> bS;
        finBlobs.push_back(fB);
        blobSizes.push_back(bS);
    }
    real=new XnPoint3D[1000];
    proj=new XnPoint3D[1000];
    is2CamCalib=false;
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
        is2CamCalib=true;
    }
    sendTuio=false;
    isServerCreated=false;
    calSKT=cal;
    if(cam->numContextosIniciados<2){
        ui->twoCamsMenu->setDisabled(true);
    }
    ui->mixActive->setChecked(is2CamCalib);
    ui->mixActive->setDisabled(!is2CamCalib);
    //inicio=clock();
    numFrames=0;
    tmw=tm;
    minDataUpdate=0.0;
    ui->numThreadsText->setText(QString::number(omp_get_max_threads()));
    isProcessing=true;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    if(autostart){
        ui->tuioHost->setText(QString::fromStdString(hst));
        ui->tuioPort->setValue(prt);
        ui->sendTuioCheck->toggle();
    }
    timer=new QTimer(this);
    timer->start(20);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

sktinfoextractor::~sktinfoextractor()
{
    timer->stop();
    delete timer;
    delete ui;
    sktP.clear();
    toBlobs.clear();
    delete real;
    delete proj;
    blobSizes.clear();
    finBlobs.clear();
    if(isServerCreated){
        delete tds;
    }
    if(useDC){
        delete dc;
    }
}
void sktinfoextractor::hacerUpdate(){
    int hhh=cam->numContextosIniciados;
    if(cam->numContextosIniciados>0){
        if(numFrames==0){
#ifdef _WIN32 || _WIN64
            GetSystemTime(&inicio);
#else
            gettimeofday(&inicio,NULL);
#endif
        }
        bool* update=new bool[cam->numContextosIniciados];

        //if(cam->updateCam(true,0,true,false)){
        int tid;

        Moments moms;
        int b;
        Point2f pt;
        if(cam->numContextosIniciados>1){
            omp_set_num_threads(2);
#pragma omp parallel private(tid,moms,b,pt) shared(update)
            {
                /// TODO : FIX FOR LESS AVAILABLE THREADS THAN CONTEXTS
                tid = omp_get_thread_num();
                //if(tid!=0){
                //  tid--;
                cameras* cc=cam;
                if(tid<cc->numContextosIniciados){
                    if(cc->updateCam(false,tid,true,false)){
                        cc->retriveDepthAndMask(tid,depths[tid],masks[tid]);
                        cc->retrivePointCloudFast(tid,&(depths[tid]),&(pCs[tid]));
                        if(cConfig[tid]->method=="Background"){
                            depths[tid].convertTo(depthsS[tid],CV_32FC1,1.0/6000);
                            sktP[tid]->processImage(depthsS[tid],toBlobs[tid],masks[tid]);
                        }
                        else
                        {
                            sktP[tid]->processImage(pCs[tid],toBlobs[tid],masks[tid]);
                        }
                        findContours(toBlobs[tid], blobs[tid],CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
                        finBlobs[tid].clear();
                        blobSizes[tid].clear();
                        for(b=0; b<blobs[tid].size(); b++)
                        {
                            moms=moments(blobs[tid][b],true);
                            if(moms.m00>cConfig[tid]->minBlobSize)
                            {
                                //Point2f pt((int)(moms.m10 / moms.m00),(int)(moms.m01 / moms.m00));
                                //circle(toBlobs[k],pt,sqrt((double)moms.m00/3.14),cvScalar(255),-1);
                                //Save blob centers
                                pt.x=(int)(moms.m10 / moms.m00);
                                pt.y=(int)(moms.m01 / moms.m00);
                                finBlobs[tid].push_back(pt);
                                blobSizes[tid].push_back((int)moms.m00);
                            }
                        }
                        update[tid]=true;
                    }
                    else{update[tid]=false;}
                }
                //}
            }
        }
        else{
            if(cam->updateCam(false,0,true,false)){
                cam->retriveDepthAndMask(0,depths[0],masks[0]);
                cam->retrivePointCloudFast(0,&(depths[0]),&(pCs[0]));
                if(cConfig[0]->method=="Background"){
                    depths[0].convertTo(depthsS[0],CV_32FC1,1.0/6000);
                    sktP[0]->processImage(depthsS[0],toBlobs[0],masks[0]);
                }
                else
                {
                    sktP[0]->processImage(pCs[0],toBlobs[0],masks[0]);
                }
                findContours(toBlobs[0], blobs[0],CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
                finBlobs[0].clear();
                blobSizes[0].clear();
                for(b=0; b<blobs[0].size(); b++)
                {
                    moms=moments(blobs[0][b],true);
                    if(moms.m00>cConfig[0]->minBlobSize)
                    {
                        //Point2f pt((int)(moms.m10 / moms.m00),(int)(moms.m01 / moms.m00));
                        //circle(toBlobs[k],pt,sqrt((double)moms.m00/3.14),cvScalar(255),-1);
                        //Save blob centers
                        pt.x=(int)(moms.m10 / moms.m00);
                        pt.y=(int)(moms.m01 / moms.m00);
                        finBlobs[0].push_back(pt);
                        blobSizes[0].push_back((int)moms.m00);
                    }
                }
                update[0]=true;
            }
            else{update[0]=false;}
        }
        bool doUpdate=true;
        for(int i=0;i<cam->numContextosIniciados;i++){
            doUpdate=(doUpdate && update[i]);
        }
        if(doUpdate){
            vector<Point2f> ou=finBlobs[0];
            if(cam->numContextosIniciados>1 && update[1]){
                if(ui->mixActive->isChecked()){
                    //Run through blob centers and get 3d vector
                    //Convert 3d vectors to the first cam
                    int count=0;
                    XnPoint3D* rP=real;
                    Vec3f v;
                    //cout <<"SIZE : " << finBlobs[1].size() << endl;
                    for(int i=0;i<finBlobs[1].size();i++){
                        if(count<999 && calMat.cols>2){
                            /// POSIBLE SPEEDUP : USE POINTERS
                            v=pCs[1].at<Vec3f>(finBlobs[1][i].y,finBlobs[1][i].x);
                            ///
                            (*rP).X=v[0]*matC[0][0]+v[1]*matC[0][1]+v[2]*matC[0][2] +matC[0][3];
                            (*rP).Y=v[0]*matC[1][0]+v[1]*matC[1][1]+v[2]*matC[1][2] +matC[1][3];
                            (*rP).Z=v[0]*matC[2][0]+v[1]*matC[2][1]+v[2]*matC[2][2] +matC[2][3];
                            count++;rP++;}
                    }
                    //Convert results to projected in first cam
                    if(count >0)
                        cam->depthG[0].ConvertRealWorldToProjective(count,real,proj);

                    rP=proj;

                    for(int i=0;i<count;i++){
                        //circle(final,Point(proj[i].X,proj[i].Y),sqrt((double)blobSizes[1][i]/3.14),cvScalar(0,0,255),-1);
                        if(!hasCloseBlob2(rP,finBlobs[0],mD)){
                            //cout << "1 : "<< (int)(rP->X) << "," << (int)(rP->Y) << endl;
                            ou.push_back(Point2f(rP->X,rP->Y));
                        }
                        rP++;
                    }
                }
                else{
                    ou.insert(ou.end(), finBlobs[1].begin(), finBlobs[1].end() );
                }
            }
            if(sendTuio && isServerCreated){
                if(calSKT->isCalibrated){
                    calSKT->convertPointVector(&ou);
                }
                tds->sendCursors(ou,minDataUpdate);
            }
            if(useDC){
                if(calSKT->isCalibrated){
                    calSKT->convertPointVector(&ou);
                }
                dc->updateData(ou,minDataUpdate);
            }
            numFrames++;
        }
        if(numFrames==10){
            numFrames=0;

#ifdef _WIN32 || _WIN64
            SYSTEMTIME fin;
            GetSystemTime(&fin);
            double timeSecs=(double)(fin.wSecond+(double)fin.wMilliseconds/1000.0-((double)inicio.wMilliseconds/1000.0+inicio.wSecond))/10;
#else
            struct timeval fin;
            gettimeofday(&fin,NULL);
            double timeSecs=(double)(fin.tv_sec+(double)fin.tv_usec/1000000.0-((double)inicio.tv_usec/1000000.0+inicio.tv_sec))/10;
#endif
            //cout << timeSecs << endl;
            double dif=1.0/timeSecs;
            //cout << "FPS : " << dif <<endl;
            ui->framesPerSec->setText(QString::number(dif));
        }
        delete update;
    }
}

void sktinfoextractor::on_pushButton_released()
{
    close();
}
bool sktinfoextractor::hasCloseBlob(XnPoint3D *r, int count, Point2f p,double minDist){
    XnPoint3D *rP=r;
    for(int i=0;i<count;i++){
        if(sqrt(pow((rP->X-p.x),2)+pow((rP->Y-p.y),2))<minDist){
            return true;
        }
        rP++;
    }
    return false;
}
bool sktinfoextractor::hasCloseBlob2(XnPoint3D *r, vector<Point2f> vec,double minDist){
    for (vector<Point2f>::iterator it = vec.begin(); it!=vec.end(); ++it){
        if(sqrt(pow((r->X-(*it).x),2)+pow((r->Y-(*it).y),2))<minDist){
            return true;
        }
    }
    return false;
}
void sktinfoextractor::on_doubleSpinBox_valueChanged(double arg1)
{
    mD=arg1;
}

void sktinfoextractor::on_sendTuioCheck_toggled(bool checked)
{
    sendTuio=checked;
    if(checked){
        QByteArray   bytes  = ui->tuioHost->text().toAscii();
        const char * host    = bytes.data();
        try{
            tds=new tuiodatasender(host,ui->tuioPort->value());
            isServerCreated=true;
            tds->server->setVerbose(ui->checkBox->isChecked());
            cout << "Tuio Sender Created" << endl;
            ui->tuioHost->setDisabled(true);
            ui->tuioPort->setDisabled(true);
        }
        catch(std::exception &e){
            QMessageBox* msg=new QMessageBox(QMessageBox::Critical,"Error","Could not create the tuio server.\nCheck the parameters.");
            msg->exec();
            ui->sendTuioCheck->setChecked(false);
        }

    }
    else{
        if(isServerCreated){
            delete tds;
            tds=NULL;
            cout << "Tuio Sender Deleted" << endl;
            isServerCreated=false;
            ui->tuioHost->setDisabled(false);
            ui->tuioPort->setDisabled(false);
        }
    }
}

void sktinfoextractor::on_checkBox_toggled(bool checked)
{
    if(isServerCreated && sendTuio){
        tds->server->setVerbose(checked);
    }
}

void sktinfoextractor::on_checkBox_2_toggled(bool checked)
{
    useDC=checked;
    if(checked){
        dc=new datacollector();
        dc->tmw=tmw;
        dc->useMouse=false;
        //dc->onUpdate=&onup;
        cout << "DataCollector Created" << endl;
    }
    else
    {
        delete dc;
        dc=NULL;
        cout << "DataCollector Deleted" << endl;
    }
}


void sktinfoextractor::on_pointCalStart_released()
{
    if(ui->checkBox_2->isChecked()){
        pointcalibration* pp=new pointcalibration(calSKT,dc,ui->numPointsSpin->value(),ui->distanceFromBorder->value(),ui->framesPointBox->value());
    }
    else{
        QMessageBox* msg=new QMessageBox(QMessageBox::Critical,"Error","Check the data collector box first.");
        msg->exec();
        ui->sendTuioCheck->setChecked(false);
    }
}

void sktinfoextractor::on_tuioMouse_toggled(bool checked)
{
    if(useDC){
        dc->useMouse=checked;
    }
    else{
        QMessageBox* msg=new QMessageBox(QMessageBox::Critical,"Error","Check the data collector box first.");
        QMessageBox* msg2=new QMessageBox(QMessageBox::Warning,"Warning","If the normalized coordinates is checked you will send 0.0-1.0 coordinates to mouse.");
        ui->tuioMouse->setChecked(false);
    }
}

void sktinfoextractor::on_checkBox_3_toggled(bool checked)
{
    calSKT->useNormalizedCoords=checked;
}

void sktinfoextractor::on_minDistanceToUpdateBox_valueChanged(double arg1)
{
    minDataUpdate=arg1;
}

void sktinfoextractor::on_framesPointBox_valueChanged(int arg1)
{
    /// TODO : CHECK IF THIS FUNCTION IS BEING CALLED
}
