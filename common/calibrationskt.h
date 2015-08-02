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

#ifndef CALIBRATIONSKT_H
#define CALIBRATIONSKT_H
#include "cameras.h"
#include "utilidades.h"
#include "QDesktopWidget"
#include "QRect"
#include <TuioServer.h>

using namespace TUIO;
using namespace cv;

class calibrationskt
{
public:
    calibrationskt();
    ~calibrationskt();
    int numPoints;
    int numZones; //
    vector<vector < Point2f > > zonePoints;
    vector<Point2f> zoneCenters; //
    vector<Mat> zones;
    vector<Point2f> pointsGrid;
    vector<Point2f> pointsReceived;
    vector<Mat> zoneCalMat; //
    bool isCalibrated;
    void setPointsToMat(vector<Point2f> in,Mat& out,bool type);
    void solveTransformation(Mat& inS,Mat& inT,Mat& out);
    bool checkReady();
    void processAll();
    void convertPoint(Point2f* pt);
    void convertPointVector(vector<Point2f>* in);
    void convertTuioCur(TuioCursor* tcur);
    void convertTuioObj(TuioObject* tobj);
    void startCalibration();
    int selectBestZone(Point2f* p);
    int selectBestZoneTuio(double x, double y);
    Point2f calculateZoneCenter(vector<Point2f> zP);
    QRect screen;
    QRect rectangle;
    bool onlyRectangle; //
    bool useNormalizedCoords; //
    bool saveCalibration(char *filename="pointcal.pkl");
    bool loadCalibration(char *filename="pointcal.pkl");
};

#endif // CALIBRATIONSKT_H
