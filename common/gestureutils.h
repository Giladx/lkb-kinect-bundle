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
#ifndef GESTUREUTILS_H
#define GESTUREUTILS_H
#include "TuioServer.h"
#include "iostream"
#include <vector>
#include <list>
#include <stdlib.h>

using namespace TUIO;
using namespace std;

class gestureutils
{
public:
    gestureutils();
    vector<int> getGestureFromCursor(int numZones,TuioCursor* tcur, double minDistance);
    bool compareGestures(vector<int>* gesture,vector<int>* test);
    void saveGestures(vector<vector <int> > gestures/*, string file */);
    bool loadGestures(vector<vector <int> >* gestures,int* zones/*, string file*/);
    vector<TuioPoint> DouglasPeucker(vector<TuioPoint> path, double dist, int startInd,int endInd);
    double distancePointLine(TuioPoint p , TuioPoint l0, TuioPoint l1);
    vector<TuioPoint> normalizeGesture(vector<TuioPoint> in, int width, int height, double minDistance);
};

#endif // GESTUREUTILS_H
