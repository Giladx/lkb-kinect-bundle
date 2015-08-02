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
#include "gestureutils.h"


gestureutils::gestureutils()
{
}

vector<int> gestureutils::getGestureFromCursor(int numZones, TUIO::TuioCursor *tcur, double minDistance)
{
    vector <int> gesture;
    std::list<TuioPoint> path0=tcur->getPath();
    vector<TuioPoint> pathVec;
    vector<TuioPoint> path;
    TuioPoint prev(0,0,0);
    bool first=true;
    double zoneAngles=360.0/numZones;
    //int mm=0;
    //cout << "Paso : " << mm++ << endl;

    for(list<TuioPoint>::iterator it=path0.begin();it!=path0.end();it++){
        pathVec.push_back(*it);
    }
    //cout << "Paso : " << mm++ << endl;
    path=normalizeGesture(pathVec,100,100,minDistance);
    //path=DouglasPeucker(pathVec,minDistance,0,path0.size()-1);
    //path=pathVec;
    float angAnt=0.0;
    //cout << "Paso : " << mm++ << endl;
    for(vector<TuioPoint>::iterator it=path.begin();it!=path.end();it++){
        if(first){
            prev=*it;
            first=false;
        }
        else{
            //cout << "Distance between tuios : " << prev.getDistance(&(*it)) << endl;
            if(prev.getDistance(&(*it))>minDistance){
                bool newP=false;
                float angle=prev.getAngleDegrees(&(*it));
                if(angle>=0 && angle <=360)// && fabs(angle-angAnt)>zoneAngles/2.0)
                {
                int zonesPassed=(int)(angle/zoneAngles);
                int zonesPassed1=(zonesPassed+1)%numZones;
                if(fabs(zonesPassed*zoneAngles - angle)<fabs((zonesPassed+1)*zoneAngles - angle)){
                    if(gesture.size()==0){
                        gesture.push_back(zonesPassed);
                        newP=true;
                    }
                    else{
                        if(gesture[gesture.size()-1]!=zonesPassed){
                            gesture.push_back(zonesPassed);
                            newP=true;
                        }
                    }
                }
                else{
                    if(gesture.size()==0){
                        gesture.push_back(zonesPassed1);
                        newP=true;
                    }
                    else{
                        if(gesture[gesture.size()-1]!=zonesPassed1){
                            gesture.push_back(zonesPassed1);
                            newP=true;
                        }
                    }
                }
                if(newP){
                prev=*it;}
                //angAnt=angle;
                }
            }
        }
    }
    cout << "Gesture : ";
    for(int i=0;i<gesture.size();i++){
        cout << gesture[i];
    }
    cout << endl;
    return gesture;
}

vector<TUIO::TuioPoint> gestureutils::DouglasPeucker(vector<TUIO::TuioPoint> path, double dist, int startInd,int endInd)
{
    vector<TuioPoint> results0,results1,results2;
    //Find the point with the maximum distance
     double dmax = 0.0,d=0.0;
     int index = startInd;
     for(int i=startInd+1;i<endInd;i++){
         d=distancePointLine(path[i],path[startInd],path[endInd]);
         if(d>dmax){
             index=i;
             dmax=d;
         }
     }
     if(dmax >= dist){
      //Recursive call
      results1 = DouglasPeucker(path,dist,startInd,index);
      results2 = DouglasPeucker(path,dist,index,endInd);
      // Build the result list
      /// TODO : FIX : results1.erase(results1.end());
      results1.insert(results1.end(), results2.begin(), results2.end() );
      //results1.splice(results1.end(), results2);
      results0=results1;
      }
     else{
         for(int i=startInd;i<endInd;i++){
             results0.push_back(path[i]);
         }
     }

     //Return the result
     return results0;
}

double gestureutils::distancePointLine(TUIO::TuioPoint p, TUIO::TuioPoint l0, TUIO::TuioPoint l1)
{
    double A=l1.getY()-l0.getY();
    double B=l0.getX()-l1.getX();
    double C=l0.getX()*(l0.getY()-l1.getY())+l0.getY()*(l1.getX()-l0.getX());
    //cout << "distance : " << fabs(p.getX()*A+p.getY()*B+C)/sqrt(pow(A,2)+pow(B,2)) << endl;
    return fabs(p.getX()*A+p.getY()*B+C)/sqrt(pow(A,2)+pow(B,2));
}

bool gestureutils::compareGestures(vector<int> *gesture, vector<int> *test)
{
    if(gesture->size()!=test->size()){
        return false;
    }
    else{
        bool tst=true;
        for(int i=0;i<gesture->size();i++){
            if((*gesture)[i]!=(*test)[i]){
                tst=false;
                break;
            }
        }
        return tst;
    }
}

vector<TUIO::TuioPoint> gestureutils::normalizeGesture(vector<TUIO::TuioPoint> in, int width, int height,double minDistance)
{
    double minX=1000000.0;
    double minY=1000000.0;
    double maxX=-1000000.0;
    double maxY=-1000000.0;
    vector<TuioPoint> pathFixed;
    for(vector<TuioPoint>::iterator it=in.begin();it!=in.end();it++){
        double x=(*it).getX();
        double y=(*it).getY();
        minX=minX<x ? minX:x;
        minY=minY<y ? minY:y;
        maxX=maxX>x ? maxX:x;
        maxY=maxY>y ? maxY:y;
    }
    double scaleX=width/(maxX-minX);
    double scaleY=height/(maxY-minY);
    double scaleG=scaleX>scaleY ? scaleY:scaleX;
    for(vector<TuioPoint>::iterator it=in.begin();it!=in.end();it++){
        pathFixed.push_back(TuioPoint(((*it).getX()-minX) * scaleG,((*it).getY()-minY) * scaleG));
    }
    pathFixed=DouglasPeucker(pathFixed,minDistance,0,pathFixed.size()-1);
    return pathFixed;
}
