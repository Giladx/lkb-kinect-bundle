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
#ifndef SKELETONUTILITIES_H
#define SKELETONUTILITIES_H
#include <cameras.h>

class skeletonutilities
{
public:
    skeletonutilities();
    ~skeletonutilities();
    XnPoint3D getUserJoint3D(UserGenerator *ug,XnUserID player, XnSkeletonJoint eJoint,double confidence);
    XnPoint3D getUserJoint2D(DepthGenerator *dg,UserGenerator *ug,XnUserID player, XnSkeletonJoint eJoint,double confidence);
    void drawSkeleton(Mat& out,DepthGenerator *dg,UserGenerator *ug,XnUserID player,double confidence);
    void getSkeleton2D(XnPoint3D* out,DepthGenerator *dg,UserGenerator *ug,XnUserID player,double confidence);
    void getSkeleton3D(XnPoint3D* out,UserGenerator *ug,XnUserID player,double confidence);
    void drawLine(Mat& out, XnPoint3D from,XnPoint3D to);
    /// FOR HAND METHOD :
    void getHands2D(XnPoint3D* out,DepthGenerator *dg,UserGenerator *ug,XnUserID player,double confidence);
    void getHands3D(XnPoint3D* out,UserGenerator *ug,XnUserID player,double confidence);
    void getAreaOfJoint2D(Mat& dep,XnPoint3D* jointPos,const XnLabel* pLabels,double dist2d,double distZ,double *areaVal,Mat& img);
    void getAreaOfJoint3D(DepthGenerator *dg,Mat& pC,XnPoint3D* jointPos,const XnLabel* pLabels,double pointDistance,double *areaVal,Mat& img);
    void getCOM2D(XnPoint3D* out,DepthGenerator *dg,UserGenerator *ug,XnUserID player);
    void getCOM3D(XnPoint3D* out,UserGenerator *ug,XnUserID player);
    void convertToComCoords(XnPoint3D* pts,XnPoint3D* com,XnPoint3D* neck,double sizeofArm=500.0,int numJoints=2,bool normalizeUsingArm=true,bool isCOMOrigin=false);
    double distanceBetweenJoints2D(XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2,DepthGenerator *dg,UserGenerator *ug,XnUserID player,double confidence);
    double distanceBetweenJoints3D(XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2,UserGenerator *ug,XnUserID player,double confidence);
    ///
};

#endif // SKELETONUTILITIES_H
