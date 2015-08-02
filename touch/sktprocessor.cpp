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
#include "sktprocessor.h"

sktprocessor::sktprocessor()
{
salida.create(480,640,CV_32FC1);
}
sktprocessor::~sktprocessor(){
}

void sktprocessor::processImage(Mat& in,Mat& out, Mat& msk){
    out.create(480,640,CV_8UC1);
    if(cConfig->method=="Background"){
        salida=(cConfig->Background)-in;
        salida.mul(1/in);
        out=(salida>((double)(cConfig->minDistance)/10000.0)) & (salida<((double)(cConfig->maxDistance)/10000.0));
        if(cConfig->zoneSelected)
        out=out & msk & cConfig->maskBack & cConfig->maskZona;
        else
        out=out & msk & cConfig->maskBack;
    }
    else if(cConfig->zoneSelected && cConfig->method=="Plane" && cConfig->VP->GetPlPC(4)!=0.0){
        cConfig->VP->distPlanoPC(in,salida);
        out=(salida>((double)(cConfig->minDistance)/10.0)) & (salida<((double)(cConfig->maxDistance)/10.0)) & salida!=0;
        out=out & cConfig->maskZona;
    }

    else {
        out.setTo(cv::Scalar::all(0));
    }
    if(cConfig->dilIter0>0){
    dilate(out,out,Mat(),Point(-1,-1),cConfig->dilIter0);}
    if(cConfig->erIter>0){
    erode(out,out,Mat(),Point(-1,-1),cConfig->erIter);}
    if(cConfig->dilIter1>0){
    dilate(out,out,Mat(),Point(-1,-1),cConfig->dilIter1);}
}
