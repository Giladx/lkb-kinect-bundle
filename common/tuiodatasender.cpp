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
#include "tuiodatasender.h"

tuiodatasender::tuiodatasender(const char* host,int port)
{
    try{
    server=new TuioServer(host,port);
    for(int i=0;i<10;i++){
        isCreated[i]=false;
        isCreatedHand[i]=false;
    }
    for(int i=0;i<20;i++){
        isUsingCursor[i]=false;
    }
    }
    catch(std::exception &e){
        cout << "Could not create"<< endl;
        throw(e);
    }
}
tuiodatasender::~tuiodatasender(){
    delete server;
}
void tuiodatasender::sendCursors(vector<Point2f> crs,double minDist){
    vector<Point2f> cursors=crs;
    if(cursors.size()>0 && cursors.size()<100){
        ///CREATE DISTANCE MATIX
        previousCursors=server->getTuioCursors();
            for(int i=0;i<cursors.size();i++){
                used[i]=false;
            }
        for(cur = previousCursors.begin(); cur != previousCursors.end( ); cur++ ){
            int numB=0;
            for (it = cursors.begin(); it!=cursors.end(); it++){
                distances[(*cur)->getCursorID()][numB]=distanceCurPoint(*it,*cur);
                numB++;
            }
        }
        time=TuioTime::getSessionTime();
        server->initFrame(time);
        bool buscar=true;
        while(buscar){
            list< TuioCursor * >::iterator curI;
            vector<Point2f>::iterator ptI;
            double dist=1000000000;
            bool hayMin=false;
            int numB=0,numU=10000000;
        for (it = cursors.begin(); it!=cursors.end(); it++){
           for(cur = previousCursors.begin(); cur != previousCursors.end( ); cur++ ){
               if(dist>distances[(*cur)->getCursorID()][numB] && !used[numB]){
                   dist=distances[(*cur)->getCursorID()][numB];
                   hayMin=true;
                   curI=cur;
                   ptI=it;
                   numU=numB;
               }
           }
           numB++;
        }
        if(hayMin){
            used[numU]=true;
            /// HERE CHANGE UPDATE ONLY IF DISTANCE IS MORE THAN... ELSE KEEP THIS POINT REPEATED
            if(distances[(*curI)->getCursorID()][numU]<minDist){
                (*ptI).x=(*curI)->getX();
                (*ptI).y=(*curI)->getY();
            }
            server->updateTuioCursor((*curI),(*ptI).x,(*ptI).y);
            ///
            previousCursors.erase(curI);
        }
        else{
            buscar=false;
        }
        }
        for(cur = previousCursors.begin(); cur != previousCursors.end( ); cur++ ){
            server->removeTuioCursor(*cur);
        }
        int numB=0;
        for (it = cursors.begin(); it!=cursors.end(); it++){
            if(!used[numB]){
                server->addTuioCursor((*it).x,(*it).y);
            }
            numB++;
        }
        server->commitFrame();
    }
    else{
        ///REMOVE ALL
        previousCursors=server->getTuioCursors();
        if(previousCursors.size()>0){
        time=TuioTime::getSessionTime();
        server->initFrame(time);
        for(cur = previousCursors.begin(); cur != previousCursors.end( ); cur++ ){
            server->removeTuioCursor(*cur);
        }
        server->commitFrame();
        }
    }
}
vector<Point2f>::iterator tuiodatasender::getClosestPointToCursor(vector<Point2f> *pts,TuioCursor* cr){
    vector<Point2f>::iterator pp=pts->begin();
    double dist0=100000;
    for (it = pts->begin(); it!=pts->end(); it++){
        double dist=sqrt(pow((cr->getX()-(*it).x),2)+pow((cr->getY()-(*it).y),2));
        if(dist<dist0){
            dist0=dist;
            pp=it;
        }
    }
    return pp;
}
list<TuioCursor*>::iterator tuiodatasender::getClosestCursorToPoint(Point2f p,list< TuioCursor * > curs){
    list<TuioCursor*>::iterator cc=curs.begin();
    double dist0=100000;
    for(cur = curs.begin(); cur != curs.end( ); cur++ ){
        double dist=sqrt(pow(((*cur)->getX()-p.x),2)+pow(((*cur)->getY()-p.y),2));
        if(dist<dist0){
            dist0=dist;
            cc=cur;
        }
    }
    return cc;
}
double tuiodatasender::distanceCurPoint(Point2f p, TuioCursor *cr){
    return sqrt(pow((cr->getX()-p.x),2)+pow((cr->getY()-p.y),2));
}

void tuiodatasender::sendSkeleton(int userID, XnPoint3D *skeleton)
{
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<15;i++){
        if(skeleton[i].X!=0.0 || skeleton[i].Y!=0.0 || skeleton[i].Z!=0.0)
        server->updateTuioObject(joints[15*(userID-1)+i],skeleton[i].X,skeleton[i].Y,skeleton[i].Z);
    }
    server->commitFrame();
}

void tuiodatasender::addUserSkeleton(int userID, XnPoint3D *skeleton)
{
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<15;i++){
        joints[15*(userID-1)+i]=server->addTuioObject(15*(userID-1)+i,skeleton[i].X,skeleton[i].Y,skeleton[i].Z);
    }
    server->commitFrame();
}

void tuiodatasender::removeUserSkeleton(int userID)
{
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<15;i++){
        server->removeTuioObject(joints[15*(userID-1)+i]);
    }
    server->commitFrame();
}

void tuiodatasender::addUserHands(int userID, XnPoint3D *hds)
{
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<2;i++){
        hands[2*(userID-1)+i]=server->addTuioObject(2*(userID-1)+i,hds[i].X,hds[i].Y,hds[i].Z);
    }
    server->commitFrame();
}

void tuiodatasender::removeUserHands(int userID)
{
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<2;i++){
        server->removeTuioObject(hands[2*(userID-1)+i]);
    }
    server->commitFrame();
}

void tuiodatasender::sendHands(int userID, XnPoint3D *hds, int mode)
{
    /// MODE 0 : Both hands as objects
    /// MODE 1 : Left as Cursor, right as object
    /// MODE 2 : Right as cursor, left as object
    /// MODE 3 : Both as cursors
    time=TuioTime::getSessionTime();
    server->initFrame(time);
    for(int i=0;i<2;i++){
        if(hds[i].X!=0.0 || hds[i].Y!=0.0 || hds[i].Z!=0.0){
            if(mode==0){
                if(isUsingCursor[2*(userID-1)+i]){
                    server->removeTuioCursor(handsCur[2*(userID-1)+i]);
                    isUsingCursor[2*(userID-1)+i]=false;
                }
                server->updateTuioObject(hands[2*(userID-1)+i],hds[i].X,hds[i].Y,hds[i].Z);
            }
            else if(mode==3){
                if(isUsingCursor[2*(userID-1)+i]){
                    server->updateTuioCursor(handsCur[2*(userID-1)+i],hds[i].X,hds[i].Y);
                }
                else{
                   handsCur[2*(userID-1)+i]=server->addTuioCursor(hds[i].X,hds[i].Y);
                   isUsingCursor[2*(userID-1)+i]=true;
                }
            }
            else{
                if(i+1==mode){
                    if(isUsingCursor[2*(userID-1)+i]){
                        server->updateTuioCursor(handsCur[2*(userID-1)+i],hds[i].X,hds[i].Y);
                    }
                    else{
                       handsCur[2*(userID-1)+i]=server->addTuioCursor(hds[i].X,hds[i].Y);
                       isUsingCursor[2*(userID-1)+i]=true;
                    }
                }
                else{
                    if(isUsingCursor[2*(userID-1)+i]){
                        server->removeTuioCursor(handsCur[2*(userID-1)+i]);
                        isUsingCursor[2*(userID-1)+i]=false;
                    }
                    server->updateTuioObject(hands[2*(userID-1)+i],hds[i].X,hds[i].Y,hds[i].Z);
                }
            }
        }
    }
    server->commitFrame();
}
