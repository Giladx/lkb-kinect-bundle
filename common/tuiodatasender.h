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
#ifndef TUIODATASENDER_H
#define TUIODATASENDER_H
#include <TuioServer.h>
#include <TuioTime.h>
#include <cameras.h>
using namespace TUIO;

class tuiodatasender
{
public:
    tuiodatasender(const char* host,int port);
    ~tuiodatasender();

    TuioServer* server;
    TuioTime time;
    ///FOR BLOB INFO
    list< TuioCursor * >::iterator cur;
    list< TuioCursor * > previousCursors;
    list< TuioObject * >::iterator obj;
    list< TuioObject * > previousObjects;
    vector<Point2f>::iterator it;

    double distances[300][300];
    bool used[300];

    void sendCursors(vector<Point2f> cursors,double minDist);
    vector<Point2f>::iterator getClosestPointToCursor(vector<Point2f> *pts,TuioCursor* cr);
    list<TuioCursor*>::iterator getClosestCursorToPoint(Point2f p,list< TuioCursor * > curs);
    double distanceCurPoint(Point2f p,TuioCursor* cr);
    ////

    ///FOR SKELETON
    TuioObject* joints[150];
    bool isCreated[10];
    void addUserSkeleton(int userID, XnPoint3D *skeleton);
    void removeUserSkeleton(int userID);
    void sendSkeleton(int userID,XnPoint3D* skeleton);
    ///

    ///FOR HAND PUSH
    TuioObject* hands[20];
    TuioCursor* handsCur[20];
    bool isCreatedHand[10];
    bool isUsingCursor[20];
    void addUserHands(int userID, XnPoint3D *hands);
    void removeUserHands(int userID);
    void sendHands(int userID,XnPoint3D* hands,int mode=0);
    ///
};

#endif // TUIODATASENDER_H
