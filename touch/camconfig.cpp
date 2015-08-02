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
#include "camconfig.h"

camconfig::camconfig()
{
    BM=new backmethod();
    VP=new virtualplane();
    Background.create(480,640,CV_32FC1);
    maskZona.create(480,640,CV_8UC1);
    maskBack.create(480,640,CV_8UC1);
    plane.create(480,640,CV_32FC1);
    zone[0].x=0;zone[0].y=0;
    zone[1].x=640;zone[1].y=0;
    zone[2].x=640;zone[2].y=480;
    zone[3].x=0;zone[3].y=480;
    minDistance=30;
    maxDistance=300;
    minBlobSize=20;
    deltaForAuto=50;
    dilIter0=0;
    erIter=0;
    dilIter1=0;
    //maskSize=1.0;
    //planeDisplacement=0.0;
    useCenter=true;
    method="Background";
    zoneSelected=false;
    maskBack.setTo(cv::Scalar::all(255));
    variance=995.2;
}
camconfig::~camconfig(){
    delete BM;
    delete VP;
}
