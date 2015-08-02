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
#include "calibrationskt.h"


calibrationskt::calibrationskt()
{
    isCalibrated=false;
    QDesktopWidget* Qaux=new QDesktopWidget();
    screen= Qaux->screenGeometry();
    delete Qaux;
    //cout << screen.width() << "," << screen.height() << endl;
    onlyRectangle=false;
    useNormalizedCoords=true;
    numZones=0;
}
calibrationskt::~calibrationskt()
{

}
void calibrationskt::setPointsToMat(vector<Point2f> in, Mat &out, bool type){
    ///GOTTEN MAT
    if(type){
        out.create(3,4,CV_32FC1);
        for(int i=0;i<4;i++){
            out.at<float>(0,i)=in[i].x;
            out.at<float>(1,i)=in[i].y;
            out.at<float>(2,i)=1.0;
        }
    }
    ///GRID MAT
    else{
        out.create(2,4,CV_32FC1);
        for(int i=0;i<4;i++){
            out.at<float>(0,i)=in[i].x;
            out.at<float>(1,i)=in[i].y;
        }
    }
}
void calibrationskt::solveTransformation(Mat &inScr, Mat &inTouch, Mat &out){
    out.create(2,3,CV_32FC1);
    Mat inv=inTouch.inv(DECOMP_SVD);
    out=inScr * inv;
}
bool calibrationskt::checkReady(){
    return pointsGrid.size()==pointsReceived.size();
}
void calibrationskt::processAll(){
    zones.clear();
    zoneCalMat.clear();
    zonePoints.clear();
    zoneCenters.clear();
    for(int i=0;i<numZones;i++){
        int ind=(int)(i/(numPoints-1))*numPoints + i%(numPoints-1);
        vector<Point2f> ptsG,ptsT;
        ptsG.push_back(pointsGrid[ind]);
        ptsG.push_back(pointsGrid[ind+1]);
        ptsG.push_back(pointsGrid[ind+numPoints]);
        ptsG.push_back(pointsGrid[ind+numPoints+1]);
        ptsT.push_back(pointsReceived[ind]);
        ptsT.push_back(pointsReceived[ind+1]);
        ptsT.push_back(pointsReceived[ind+numPoints]);
        ptsT.push_back(pointsReceived[ind+numPoints+1]);
        Mat inS,inT,calM;
        setPointsToMat(ptsG,inS,false);
        setPointsToMat(ptsT,inT,true);
        solveTransformation(inS,inT,calM);
        cout << i <<" : " << calM  << endl;
        zonePoints.push_back(ptsT);
        zoneCalMat.push_back(calM);
        zoneCenters.push_back(calculateZoneCenter(ptsT));
    }
    isCalibrated=true;
}
void calibrationskt::convertPoint(Point2f *pt){
    double x=pt->x;
    double y=pt->y;
    int i=selectBestZone(pt);
    pt->x=zoneCalMat[i].at<float>(0,0) * x+zoneCalMat[i].at<float>(0,1) * y+zoneCalMat[i].at<float>(0,2);
    pt->y=zoneCalMat[i].at<float>(1,0) * x+zoneCalMat[i].at<float>(1,1) * y+zoneCalMat[i].at<float>(1,2);
    if(onlyRectangle){
        if(useNormalizedCoords){
        pt->x=(float)(pt->x-rectangle.x())/rectangle.width();
        pt->y=(float)(pt->y-rectangle.y())/rectangle.height();}
        else{
            pt->x=(float)(pt->x-rectangle.x());
            pt->y=(float)(pt->y-rectangle.y());
        }
    }
    else{
        if(useNormalizedCoords){
    pt->x/=screen.width();
    pt->y/=screen.height();}
        }
}
void calibrationskt::convertPointVector(vector<Point2f>* in){
    for(int i=0;i<in->size();i++){
        convertPoint(&((*in)[i]));
    }
}
void calibrationskt::startCalibration(){
    pointsReceived.clear();
    isCalibrated=false;
}
int calibrationskt::selectBestZone(Point2f *p){
    int ind=0;
    double dist=1000000;
    for(int i=0;i<numZones;i++){
        double d=utilidades::distancePointPoint((*p),zoneCenters[i]);
        if(d<dist){
            ind=i;
            dist=d;
        }
    }
    return ind;
}
Point2f calibrationskt::calculateZoneCenter(vector<Point2f> zP){
    Point2f out(0.0,0.0);
    for(int i=0;i<4;i++){
        out.x+=zP[i].x;
        out.y+=zP[i].y;
    }
    out.x/=4;
    out.y/=4;
    return out;
}

void calibrationskt::convertTuioCur(TUIO::TuioCursor *tcur)
{
    double x0=tcur->getX();
    double y0=tcur->getY();
    double x,y;
    int i=selectBestZoneTuio(x,y);
    x=zoneCalMat[i].at<float>(0,0) * x0+zoneCalMat[i].at<float>(0,1) * y0+zoneCalMat[i].at<float>(0,2);
    y=zoneCalMat[i].at<float>(1,0) * x0+zoneCalMat[i].at<float>(1,1) * y0+zoneCalMat[i].at<float>(1,2);
    if(onlyRectangle){
        if(useNormalizedCoords){
            x=(float)(x-rectangle.x())/rectangle.width();
            y=(float)(y-rectangle.y())/rectangle.height();}
        else{
            x=(float)(x-rectangle.x());
            y=(float)(y-rectangle.y());
        }
    }
    else{
        if(useNormalizedCoords){
            x/=screen.width();
            y/=screen.height();
            }
        }
    /// DO UPDATE
    tcur->update(x,y,0,0,0);
}

void calibrationskt::convertTuioObj(TUIO::TuioObject *tobj)
{
    double x0=tobj->getX();
    double y0=tobj->getY();
    double x,y;
    int i=selectBestZoneTuio(x,y);
    x=zoneCalMat[i].at<float>(0,0) * x0+zoneCalMat[i].at<float>(0,1) * y0+zoneCalMat[i].at<float>(0,2);
    y=zoneCalMat[i].at<float>(1,0) * x0+zoneCalMat[i].at<float>(1,1) * y0+zoneCalMat[i].at<float>(1,2);
    if(onlyRectangle){
        if(useNormalizedCoords){
            x=(float)(x-rectangle.x())/rectangle.width();
            y=(float)(y-rectangle.y())/rectangle.height();}
        else{
            x=(float)(x-rectangle.x());
            y=(float)(y-rectangle.y());
        }
    }
    else{
        if(useNormalizedCoords){
            x/=screen.width();
            y/=screen.height();
            }
        }
    /// DO UPDATE
    tobj->update(x,y,0,0);
}

int calibrationskt::selectBestZoneTuio(double x, double y)
{
    int ind=0;
    double dist=1000000;
    for(int i=0;i<numZones;i++){
        double d=utilidades::distanceXYPoint(x,y,zoneCenters[i]);
        if(d<dist){
            ind=i;
            dist=d;
        }
    }
    return ind;
}
bool calibrationskt::saveCalibration(char *filename){
    FileStorage fs(filename, FileStorage::WRITE);
    fs << "type" << 12;
    fs << "numZones" << numZones << "useNormalized" << (int)useNormalizedCoords << "onlyRectangle" << (int)onlyRectangle;
    for(int i=0;i<numZones;i++){
        ostringstream aa,bb,cc;
        aa << "mat" << i;
        bb << "cenX" << i;
        cc << "cenY" << i;
        fs << aa.str() << zoneCalMat[i];
        fs << bb.str() << zoneCenters[i].x;
        fs << cc.str() << zoneCenters[i].y;
    }
    cout << "Saved Point Calibration File : " << filename << endl;
    return true;
}
bool calibrationskt::loadCalibration(char *filename){
    FileStorage fs(filename, FileStorage::READ);
    if(fs.isOpened()){
    int type =(int)fs["type"];
    if(type==12){
    useNormalizedCoords =(bool)((int)fs["useNormalized"]); onlyRectangle =(bool)((int)fs["onlyRectangle"]); numZones = (int)fs["numZones"];
    zoneCalMat.clear();
    zoneCenters.clear();
    for(int i=0;i<numZones;i++){
        ostringstream aa,bb,cc;
        aa << "mat" << i;
        bb << "cenX" << i;
        cc << "cenY" << i;
        Mat M; fs[aa.str()] >> M;
        Point2f pt;
        pt.x=(float)fs[bb.str()];
        pt.y=(float)fs[cc.str()];
        zoneCalMat.push_back(M);
        zoneCenters.push_back(pt);
    }
    isCalibrated=true;
    cout << "Loaded Point Calibration File : " << filename << endl;
    return true;
    }
    else{
        cout << "Could not load Point Calibration File : " << filename << " - File is not a point calibration!" << endl;
        return false;}
    }
    else
    {
        cout << "Could not load Point Calibration File : " << filename << " - Could not open the file..." << endl;
        return false;}
}
