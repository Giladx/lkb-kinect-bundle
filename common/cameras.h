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
#ifndef CAMERAS_H
#define CAMERAS_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cv.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnPropNames.h>
#include <omp.h>

using namespace cv;
using namespace std;
using namespace xn;

class cameras
{
    public:
        cameras();
        virtual ~cameras();
        Context context[10];
        DepthGenerator depthG[10];
        ImageGenerator image[10];
        DepthMetaData depthMD[10];
        ImageMetaData imageMD[10];
        UserGenerator userG[10];
        SceneMetaData smd[10];
        XnUInt64 shadowValue;
        XnUInt64 noSampleValue;
        XnStatus rc;
        ScriptNode scriptNode;
        //EnumerationErrors errors;
        XnMapOutputMode mapMode,ImapMode;
        int numContextosIniciados;
        ///Inicia contextos, retorna numero de contextos iniciados.
        int initiateContexts(bool allPosible, int numContexts=1);
        bool updateCam(bool all, int num=0,bool depthData=true,bool imageData=true);
        bool updateCamAndUsers(bool all, int num=0,bool depthData=true,bool imageData=true);
        void retriveDepthAndMask(int num, Mat& dep,Mat& mask);
        void retriveImage(int num, Mat& img);
        void retrivePointCloud(int num,Mat& dep,Mat& out);
        void retrivePointCloudFast(int num,Mat* dep,Mat* out);
        void retrivePointCloudVector(int num,Mat& dep,XnPoint3D* out);
    protected:
    private:
        XnPoint3D* proj;//Auxiliary variable;
        XnPoint3D* real;
        XnPoint3D* fastProj[10];
        XnPoint3D* fastReal[10];
};

#endif // CAMERAS_H
