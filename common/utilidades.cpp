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
#include "utilidades.h"

utilidades::utilidades()
{
    //ctor
}

utilidades::~utilidades()
{
    //dtor
}
Vec3d utilidades::getVectorFromProjectedPoint(int x, int y, Mat& pC)
{
    Vec3d out;
    Vec3f in=pC.at<Vec3f>(y,x);
    out[0]=(double)in[0];
    out[1]=(double)in[1];
    out[2]=(double)in[2];
    return out;
}
void utilidades::drawCircle(Mat& out,int x,int y, int innerSize, int outerSize,bool triple, int innerInnerSize){
    Point centro;
    centro.x=x;
    centro.y=y;
    out.setTo(cv::Scalar::all( 0 ));
    circle(out, centro, outerSize, cvScalar(255), -1,8,0);
    circle(out, centro, innerSize, cvScalar(0), -1,8,0);
    if(triple){
        circle(out, centro, innerInnerSize, cvScalar(255), -1,8,0);
    }
}
void utilidades::reorderZone(Point2f *zone){
    Point zoneN[4],x,y;
    for(int i=0;i<4;i++){

    }
}
double utilidades::distancePointPoint(Point2f p1, Point2f p2){
    return sqrt(pow((p1.x-p2.x),2)+pow((p1.y-p2.y),2));
}

double utilidades::distanceXYPoint(double x, double y, cv::Point2f p2)
{
    return sqrt(pow((x-p2.x),2)+pow((y-p2.y),2));
}
double utilidades::vectorModule(Vec3f *pt){
    return sqrt((*pt)[0] * (*pt)[0] + (*pt)[1] * (*pt)[1] + (*pt)[2] * (*pt)[2]);
}
