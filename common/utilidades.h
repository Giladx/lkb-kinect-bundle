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
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdlib.h>
#include <ctime>
#include <cv.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <cameras.h>
#include <sstream>

using namespace cv;
using namespace std;
using namespace xn;

class utilidades
{
    public:
        utilidades();
        virtual ~utilidades();
        static Vec3d getVectorFromProjectedPoint(int x, int y, Mat& pC);
        static void drawCircle(Mat& out,int x,int y, int innerSize, int outerSize,bool triple=false, int innerInnerSize=0);
        static void reorderZone(Point2f* zone);
        static double distancePointPoint(Point2f p1, Point2f p2);
        static double distanceXYPoint(double x,double y, Point2f p2);
        static double vectorModule(Vec3f* pt);
    protected:
    private:
};

#endif // UTILIDADES_H
