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
#include "cameras.h"
using namespace cv;
using namespace std;
using namespace xn;
cameras::cameras()
{
    //ctor
    mapMode.nXRes = XN_VGA_X_RES;
    mapMode.nYRes = XN_VGA_Y_RES;
    mapMode.nFPS = 30;
    ImapMode.nXRes = XN_VGA_X_RES;
    ImapMode.nYRes = XN_VGA_Y_RES;
    ImapMode.nFPS = 30;
    numContextosIniciados=0;
    proj = new XnPoint3D[640*480];
    real = new XnPoint3D[640*480];
    for(int i=0;i<10;i++){
        fastProj[i]=new XnPoint3D[640*480];
        fastReal[i]=new XnPoint3D[640*480];
    }
}

cameras::~cameras()
{
    //dtor
    for(int i=0; i<numContextosIniciados; i++)
    {
        context[i].StopGeneratingAll();
        context[i].Release();
    }
    for(int i=0;i<10;i++){
        delete fastProj[i];
        delete fastReal[i];
    }
    delete proj;
    delete real;
}
bool cameras::updateCam(bool all, int num,bool depthData,bool imageData){
    if(numContextosIniciados!=0){
    if(!all)
    {
        rc=context[num].WaitOneUpdateAll(depthG[num]);
        if(rc==XN_STATUS_OK)
        {
            if(depthData)
                depthG[num].GetMetaData(depthMD[num]);
            if(imageData)
                image[num].GetMetaData(imageMD[num]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        bool funciono=true;
        for(int i=0; i< numContextosIniciados; i++)
        {
            rc=context[i].WaitOneUpdateAll(depthG[i]);
            if(rc==XN_STATUS_OK)
            {
                if(depthData)
                    depthG[i].GetMetaData(depthMD[i]);
                if(imageData)
                    image[i].GetMetaData(imageMD[i]);
            }
            else
            {
                funciono=false;
            }
        }
        return funciono;
    }
    }
    else{
        return false;
    }
}
int cameras::initiateContexts(bool allPosible, int numContexts){
    rc=context[0].Init();
    int numTot=0;
    if(rc==XN_STATUS_OK)
    {
        xn::NodeInfoList devicesList;
        rc = context[0].EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, devicesList, 0 );
        xn::NodeInfoList::Iterator it=devicesList.Begin();
        xn::NodeInfo deviceNode=*it;
        if( rc != XN_STATUS_OK )
        {
            cout << "Failed to enumerate production trees " << endl;
            context[0].Release();
            return numTot;
        }
        for(it=devicesList.Begin(); it!=devicesList.End(); it++)
        {
            deviceNode=*it;
            if(numTot!=0)
            {
                context[numTot].Init();
            }
            context[numTot].CreateProductionTree( deviceNode );
            depthG[numTot].Create(context[numTot]);
            image[numTot].Create(context[numTot]);
            userG[numTot].Create(context[numTot]);

            depthG[numTot].SetMapOutputMode(mapMode);
            image[numTot].SetMapOutputMode(ImapMode);
            depthG[numTot].GetAlternativeViewPointCap().SetViewPoint(image[numTot]);
            xnSetMirror(depthG[numTot],true);
            xnSetMirror(image[numTot],true);
            xnSetMirror(userG[numTot],true);
            if(numTot==0)
            {
                depthG[0].GetIntProperty( "ShadowValue", shadowValue );
                depthG[0].GetIntProperty("NoSampleValue", noSampleValue );
            }


            depthG[numTot].StartGenerating();
            image[numTot].StartGenerating();
            //context[numTot].SetGlobalMirror(true);
            numTot++;
            if(!allPosible && numTot==numContexts)
            {
                break;
            }
        }
        numContextosIniciados=numTot;
        return numTot;
    }
    else
    {
        context[0].Release();
        numContextosIniciados=numTot;
        return numTot;
    }
}
void cameras::retriveDepthAndMask(int num, Mat& dep,Mat& mask){
            if(num<numContextosIniciados){
                dep.create(480,640,CV_16UC1);
            memcpy( dep.data, depthMD[num].Data(), 640*480*sizeof(XnDepthPixel) );
            mask = (dep == noSampleValue) | (dep == shadowValue) | (dep == 0);
            dep.setTo( cv::Scalar::all( 0 ), mask );
            mask = (mask==0);
            }
}
void cameras::retriveImage(int num, Mat& img){
    if(num<numContextosIniciados){
            img.create(480,640,CV_8UC3);
            memcpy( img.data, imageMD[num].RGB24Data(), 640*480*sizeof(XnRGB24Pixel) );
            cvtColor(img,img,CV_BGR2RGB);
            }
}
void cameras::retrivePointCloud(int num,Mat& dep,Mat& out){
    if(num<numContextosIniciados){
    int rows=480,cols=640;
    out.create(rows,cols,CV_32FC3);
    /// NEEDED? : out.setTo(cv::Scalar::all( 0 ));
    XnPoint3D* projP = proj;
    XnPoint3D* realP = real;
    unsigned short* imgP;
    for( int y = 0; y < rows; y++ )
    {
        imgP = (unsigned short*)(dep.data + dep.step*y);
        for( int x = 0; x < cols; x++ )
        {
            (*projP).X = x;
            (*projP).Y = y;
            (*projP).Z = (*imgP);
            projP++;imgP++;
        }
        imgP=NULL;
    }
    depthG[num].ConvertProjectiveToRealWorld(cols*rows, proj, real);
    realP=real;
    for( int y = 0; y < rows; y++ )
    {
        Point3f* ptr=(Point3f*)(out.data + out.step*y);
        unsigned short* depP=(unsigned short*)(dep.data + dep.step*y);
        for( int x = 0; x < cols; x++ )
        {
            if((*depP) == 0 ) // not valid
            {
                (*ptr).x=0.0;
                (*ptr).y=0.0;
                (*ptr).z=0.0;//=cv::Point3f( 0.0, 0.0, 0.0 );
               }
            else
            {
                (*ptr).x=(*realP).X;
                (*ptr).y=(*realP).Y;
                (*ptr).z=(*realP).Z;
                //(*ptr) = cv::Point3f( (*realP).X, (*realP).Y, (*realP).Z); // in mm
            }
            depP++;ptr++;realP++;
        }
    }
    }
}
void cameras::retrivePointCloudVector(int num,Mat& dep,XnPoint3D* out){
    if(num<numContextosIniciados){
    int rows=480,cols=640;
    XnPoint3D* projP=proj;
    unsigned short* imgP;
    for( int y = 0; y < rows; y++ )
    {
        imgP = (unsigned short*)(dep.data + dep.step*y);
        for( int x = 0; x < cols; x++ )
        {
            (*projP).X = x;
            (*projP).Y = y;
            (*projP).Z = (*imgP);
            projP++;imgP++;
        }
        imgP=NULL;
    }
    depthG[num].ConvertProjectiveToRealWorld(cols*rows, proj, out);
    }
}
void cameras::retrivePointCloudFast(int num,Mat* dep,Mat* out){
    if(num<numContextosIniciados){
    int rows=480,cols=640;
    (*out).create(rows,cols,CV_32FC3);
    /// NEEDED? : out.setTo(cv::Scalar::all( 0 ));
    XnPoint3D* projP = fastProj[num];
    XnPoint3D* realP = fastReal[num];
    unsigned short* imgP;

    for( int y = 0; y < rows; y++ )
    {
        imgP = (unsigned short*)((*dep).data + (*dep).step*y);
        for( int x = 0; x < cols; x++ )
        {
            (*projP).X = x;
            (*projP).Y = y;
            (*projP).Z = (*imgP);
            projP++;imgP++;
        }
        imgP=NULL;
    }
    depthG[num].ConvertProjectiveToRealWorld(cols*rows, fastProj[num], fastReal[num]);
    realP=fastReal[num];
    for( int y = 0; y < rows; y++ )
    {
        Point3f* ptr=(Point3f*)((*out).data + (*out).step*y);
        unsigned short* depP=(unsigned short*)((*dep).data + (*dep).step*y);
        for( int x = 0; x < cols; x++ )
        {
            if((*depP) == 0 ) // not valid
            {
                (*ptr).x=0.0;
                (*ptr).y=0.0;
                (*ptr).z=0.0;//=cv::Point3f( 0.0, 0.0, 0.0 );
               }
            else
            {
                (*ptr).x=(*realP).X;
                (*ptr).y=(*realP).Y;
                (*ptr).z=(*realP).Z;
                //(*ptr) = cv::Point3f( (*realP).X, (*realP).Y, (*realP).Z); // in mm
            }
            depP++;ptr++;realP++;
        }
    }
    }
}

bool cameras::updateCamAndUsers(bool all, int num, bool depthData, bool imageData)
{
    if(numContextosIniciados!=0){
    if(!all)
    {
        rc=context[num].WaitOneUpdateAll(userG[num]);
        if(rc==XN_STATUS_OK)
        {
            if(depthData)
                depthG[num].GetMetaData(depthMD[num]);
            if(imageData)
                image[num].GetMetaData(imageMD[num]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        bool funciono=true;
        for(int i=0; i< numContextosIniciados; i++)
        {
            rc=context[i].WaitOneUpdateAll(userG[num]);
            if(rc==XN_STATUS_OK)
            {
                if(depthData)
                    depthG[i].GetMetaData(depthMD[i]);
                if(imageData)
                    image[i].GetMetaData(imageMD[i]);
            }
            else
            {
                funciono=false;
            }
        }
        return funciono;
    }
    }
    else{
        return false;
    }
}

