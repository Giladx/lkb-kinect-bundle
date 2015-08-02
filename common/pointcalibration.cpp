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
#include "pointcalibration.h"
#include "ui_pointcalibration.h"
pointcalibration *pointcalibration::instance = 0;
template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}
pointcalibration::pointcalibration(calibrationskt* cal,datacollector* datc,int numP,double distBor,int numFrames,bool* working,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pointcalibration)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    //layo = new QGridLayout();
    //gl1=new glwidget2d();
    gl1=ui->viewer;
    in=new instructions("Press 'q' to quit calibration\n"
                        "Press 'h' to close/open this help\n"
                        "Press 's' to begin the calibration\n"
                        "Press 'a' to abort current calibration\n\n"
                        "HOW TO USE : \n\n"
                        "NORMAL MODE : \n"
                        "a)Select the zone where that you want to use for calibration by clicking on the four corners.\n"
                        "b)After a zone is selected you can deselect it by clicking again.\n"
                        "NOTE : If no zone is selected the calibration will be done using all the screen but \n"
                        "       you must select a zone and click to deselect it first.\n\n"
                        "RECTANGLE EXCUSIVE MODE : \n"
                        "Pressing '1' toggles on and off the mode to rectangle exclusive mode.\n"
                        "In this mode only the selected rectangle's data will be used.\n"
                        "a)Select the zone by clicking on the upper-left and lower-right corners.\n"
                        "b)After a zone is selected you can deselect it by clicking again.\n\n"
                        "AFTER ZONE SELECTION FOR BOTH MODES : \n"
                        "a)Press 's' to begin calibration.\n"
                        "b)Perform the calibration.\n"
                        "c)When the calibration finishes the window will close automatically.");
    instance=this;
    gl1->onNewClick=&newClick;
    /*layo->addWidget(gl1, 0, 0,1,1);
    layo->setContentsMargins(0,0,0,0);
    layo->setSpacing(0);
    this->setLayout(layo);*/
    this->setAttribute( Qt::WA_DeleteOnClose, true );
    this->setWindowOpacity(0.5);
    QDesktopWidget* Qaux=new QDesktopWidget();
    screen= Qaux->screenGeometry();
    delete Qaux;
    cout << screen.width() << "," << screen.height() << endl;
    back.create(screen.height(),screen.width(),CV_8UC3);
    gl1->updateImage(true,back);
    this->grabKeyboard();
    calib=cal;
    doingCalibration=false;
    doingZoneSelection=false;
    zoneSelected=false;
    fullscreen=true;
    numPoints=numP;
    numSelectedPoints=0;
    numCalibrating=0;
    numFramesGone=0;
    numFramesToUsable=numFrames;
    received0=false;
    selectingRectangle=false;
    distanceFromBorder=distBor;
    if(datc!=NULL){
    dc=datc;
    dc->onUpdate=&(onup);
    dc->onAdd=&(onad);
    dc->onRemove=&(onde);
    }
    else{
        dc=NULL;
    }
    work=working;
    showFullScreen();
    in->show();
    /// TODO MAKE A SMALL APPEARING GUI TO CHANGE NUMFRAMES USABLE AND NUMPOINTS
}

pointcalibration::~pointcalibration()
{
    gl1->onNewClick=NULL;
    //delete gl1;
    delete ui;
    delete in;
    if(dc!=NULL){
    dc->onUpdate=NULL;
    dc->onAdd=NULL;
    dc->onRemove=NULL;}
    if(work!=NULL){
    (*work)=false;}
}
void pointcalibration::nClick(QPoint pos){
    if(!zoneSelected){
        zone[numSelectedPoints].x=pos.x();
        zone[numSelectedPoints].y=pos.y();
        numSelectedPoints++;
        for(int i=0;i<numSelectedPoints;i++){
            circle(back,zone[i],10,Scalar(0,0,255),-1);
        }
        if(numSelectedPoints==4){
            zoneSelected=true;
            numSelectedPoints=0;
            back.setTo(cv::Scalar::all(255));
            int *num=new int[1];
            num[0]=4;
            Point zon[4];
            for (int k=0;k<4;k++){
                zon[k].x=zone[k].x;
                zon[k].y=zone[k].y;
            }
            const Point* ptt[1] = { zon };
            polylines(back,ptt,num,1,1,cvScalar(255),2);
            delete num;
            pointGrid=createPointArray(numPoints,zone,distanceFromBorder);
            for(int i=0;i<pointGrid.size();i++){
                circle(back,pointGrid[i],10,cvScalar(0,255,0),2);
            }
        }
        if(numSelectedPoints==2 && selectingRectangle){
            zoneSelected=true;
            numSelectedPoints=0;
            back.setTo(cv::Scalar::all(255));
            int *num=new int[1];
            num[0]=4;
            Point zon[4];
            zon[0].x=zone[0].x;
            zon[0].y=zone[0].y;
            zon[1].x=zone[1].x;
            zon[1].y=zone[0].y;
            zon[2].x=zone[1].x;
            zon[2].y=zone[1].y;
            zon[3].x=zone[0].x;
            zon[3].y=zone[1].y;
            for(int k=0;k<4;k++){
                zone[k].x=zon[k].x;
                zone[k].y=zon[k].y;
                //cout << k << " : "<< zone[k].x << ","<< zone[k].y << endl;
            }
            const Point* ptt[1] = { zon };
            polylines(back,ptt,num,1,1,cvScalar(255),2);
            delete num;
            pointGrid=createPointArray(numPoints,zone,distanceFromBorder);
            for(int i=0;i<pointGrid.size();i++){
                circle(back,pointGrid[i],10,cvScalar(0,255,0),2);
            }
        }
        gl1->updateImage(true,back);
    }
    else{
        zone[0]=Point2f(0,0);
        zone[1]=Point2f(screen.width(),0);
        zone[2]=Point2f(screen.width(),screen.height());
        zone[3]=Point2f(0,screen.height());
        zoneSelected=false;
        numSelectedPoints=0;
        back.setTo(cv::Scalar::all(0));
        pointGrid=createPointArray(numPoints,zone,distanceFromBorder);
        for(int i=0;i<pointGrid.size();i++){
            circle(back,pointGrid[i],10,cvScalar(0,255,0),2);
        }
        gl1->updateImage(true,back);
    }
}
void pointcalibration::keyPressEvent(QKeyEvent *event){
    //cout << "KEY : " << event->key() << endl;
    if(event->key()==81){
        close();
    }
    if(event->key()==72){
        if(in->isVisible())in->hide();
        else in->show();
    }
    if(event->key()==83){
        cout << "CALIBRATION BEGUN" << endl;
        doingCalibration=true;
        numCalibrating=0;
        calib->pointsGrid=pointGrid;
        calib->numPoints=numPoints;
        calib->numZones=(numPoints-1)*(numPoints -1);
        updateCalibrationImage(back,numCalibrating,0.0);
        /// TODO REPLACE INSTRUCTIONS
        this->setWindowOpacity(0.8);
        received0=false;
        calib->startCalibration();
    }
    if(event->key()==65){
        cout << "CALIBRATION ABORTED" << endl;
        doingCalibration=false;
        numCalibrating=0;
        this->setWindowOpacity(0.5);
    }
    if(event->key()==49){
        selectingRectangle=!selectingRectangle;
        QMessageBox* msg;
        if(selectingRectangle)
        msg=new QMessageBox(QMessageBox::Information,"Selected Method","Using Rectangle Only Method");
        else
        msg=new QMessageBox(QMessageBox::Information,"Selected Method","Using Whole Screen Method");
        msg->exec();
        cout << "Rectangle : " <<  selectingRectangle << endl;
    }
}
vector<Point2f> pointcalibration::createPointArray(int nP, Point2f *zo,double sep){
    double len[4],dir[4][2],spc[4];
    vector<Point2f> out;
    //double sep=0.05;
    for(int i=0;i<4;i++){
        len[i]=utilidades::distancePointPoint(zo[i],zo[(i+1)%4]);
        spc[i]=(1.0-2*sep)*len[i]/(nP-1);
        //cout << "Tam : "<< i << " : " << len[i] << endl;
        dir[i][0]=(zo[(i+1)%4].x-zo[i].x)/len[i];
        dir[i][1]=(zo[(i+1)%4].y-zo[i].y)/len[i];
        //cout << "Direction "<< i << " : " << dir[i][0] <<"," << dir[i][1] << endl;
    }
    ///MAKE THE GRID:::
    for(int j=0;j<nP;j++){
        Point p0,p1;
        double tam,esp,nDir[2];
        p0.x=zo[0].x-dir[3][0]*(j*spc[3]+sep*len[3]);
        p0.y=zo[0].y-dir[3][1]*(j*spc[3]+sep*len[3]);
        p1.x=zo[1].x+dir[1][0]*(j*spc[1]+sep*len[1]);
        p1.y=zo[1].y+dir[1][1]*(j*spc[1]+sep*len[1]);
        tam=utilidades::distancePointPoint(p0,p1);
        esp=(1.0-2*sep)*tam/(nP-1);
        nDir[0]=(p1.x-p0.x)/tam;
        nDir[1]=(p1.y-p0.y)/tam;
            for(int i=0;i<nP;i++){
                Point pun;
                pun.x=p0.x+nDir[0]*(i*esp+sep*tam);
                pun.y=p0.y+nDir[1]*(i*esp+sep*tam);
                out.push_back(pun);
            }
    }
    return out;
}
void pointcalibration::onAdd(int id, float x, float y){
    //cout << "ADD : LEGGO HASTA ACA ... " << x << "," << y << endl;
    if(doingCalibration && id==0){
        received0=true;
        numFramesGone=0;
        xAvrg=0;
        yAvrg=0;
    }
}
void pointcalibration::onUpdate(int id, float x, float y){
    //cout << "UP :: LEGGO HASTA ACA ... " << x << "," << y << endl;
    if(doingCalibration && id==0 && received0){
        numFramesGone++;
        updateCalibrationImage(back,numCalibrating,(double)numFramesGone/numFramesToUsable);
        xAvrg+=x;
        yAvrg+=y;
        if(numFramesGone==numFramesToUsable){
            received0=false;
            numFramesGone=0;
            xAvrg/=numFramesToUsable;
            yAvrg/=numFramesToUsable;
            calib->pointsReceived.push_back(Point2f(xAvrg,yAvrg));
            if(calib->checkReady()){
                doingCalibration=false;
                calib->processAll();
                cout << "CALIBRATION ENDED" << endl;
                doingCalibration=false;
                numCalibrating=0;
                this->setWindowOpacity(0.5);
                back.setTo(cv::Scalar::all(0));
                gl1->updateImage(true,back);
                calib->rectangle.setX((int)(zone[0].x));
                calib->rectangle.setY((int)(zone[0].y));
                calib->rectangle.setWidth((int)(zone[2].x-zone[0].x));
                calib->rectangle.setHeight((int)(zone[2].y-zone[0].y));
                calib->onlyRectangle=selectingRectangle;
                cout << "RECT : "<< calib->rectangle.x() << ","<< calib->rectangle.y() << " - " << calib->rectangle.width() << "," << calib->rectangle.height() << endl;
                if(work!=NULL){
                (*work)=false;}
                in->close();
                close();
            }
            else{
                numFramesGone=0;
                numCalibrating++;
                updateCalibrationImage(back,numCalibrating,(double)numFramesGone/numFramesToUsable);
            }
        }
    }
}
void pointcalibration::onDelete(int id){
    //cout << "DEL :: LEGGO HASTA ACA ... " << id << endl;
    if(doingCalibration && id==0){
        numFramesGone=0;
        received0=false;
    }
}
void pointcalibration::updateCalibrationImage(Mat &img, int numPC, double color){
    double col=color>1.0 ? 1.0:color;
    col=color<0.0 ? 0.0:color;
    if(numPC<calib->pointsGrid.size()){
    for(int i=0;i<numPC;i++){
        circle(img,calib->pointsGrid[i],20,cvScalar(0,255,0),5);
    }
    circle(img,calib->pointsGrid[numPC],20,cvScalar((int)((1-col)*255),(int)(col*255),0),5);
    gl1->updateImage(true,img);
    }
}
