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
#include "skeletonutilities.h"

skeletonutilities::skeletonutilities()
{
}
skeletonutilities::~skeletonutilities()
{
}
XnPoint3D skeletonutilities::getUserJoint3D(xn::UserGenerator *ug, XnUserID player, XnSkeletonJoint eJoint,double confidence)
{
        XnPoint3D out;
        XnSkeletonJointPosition joint;
        ug->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);
        if (joint.fConfidence < confidence)
        {
            out.X=0.0;
            out.Y=0.0;
            out.Z=0.0;
            return out;
        }
        else
        {
            out=joint.position;
            return out;
        }
}

XnPoint3D skeletonutilities::getUserJoint2D(DepthGenerator *dg,xn::UserGenerator *ug, XnUserID player, XnSkeletonJoint eJoint, double confidence)
{
    XnPoint3D out;
    XnSkeletonJointPosition joint;
    ug->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);
    if (joint.fConfidence < confidence)
    {
        out.X=0.0;
        out.Y=0.0;
        out.Z=0.0;
        return out;
    }
    else
    {
        out=joint.position;
        dg->ConvertRealWorldToProjective(1, &out, &out);
        return out;
    }
}

void skeletonutilities::drawSkeleton(cv::Mat &out, xn::DepthGenerator *dg, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    XnPoint3D head,neck,lcollar,rcollar,lshoulder,rshoulder,lelbow,relbow,lwrist,rwrist,lhand,rhand,lfinger,rfinger,torso,waist,lhip,rhip,lknee,rknee,lankle,rankle,lfoot,rfoot;
    head=getUserJoint2D(dg,ug,player,XN_SKEL_HEAD,confidence);
    neck=getUserJoint2D(dg,ug,player,XN_SKEL_NECK,confidence);
    lshoulder=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_SHOULDER,confidence);
    rshoulder=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_SHOULDER,confidence);
    lelbow=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_ELBOW,confidence);
    relbow=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_ELBOW,confidence);
    lhand=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_HAND,confidence);
    rhand=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_HAND,confidence);
    torso=getUserJoint2D(dg,ug,player,XN_SKEL_TORSO,confidence);
    lhip=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_HIP,confidence);
    rhip=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_HIP,confidence);
    lknee=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_KNEE,confidence);
    rknee=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_KNEE,confidence);
    lfoot=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_FOOT,confidence);
    rfoot=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_FOOT,confidence);
    lcollar=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_COLLAR,confidence);
    rcollar=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_COLLAR,confidence);
    lwrist=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_WRIST,confidence);
    rwrist=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_WRIST,confidence);
    waist=getUserJoint2D(dg,ug,player,XN_SKEL_WAIST,confidence);
    lfinger=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_FINGERTIP,confidence);
    rfinger=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_FINGERTIP,confidence);
    lankle=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_ANKLE,confidence);
    rankle=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_ANKLE,confidence);

    drawLine(out,head,neck);
    //drawLine(out,neck,lcollar);
    //drawLine(out,neck,rcollar);
    drawLine(out,neck,lshoulder);
    drawLine(out,neck,rshoulder);
    drawLine(out,lshoulder,lelbow);
    drawLine(out,rshoulder,relbow);
    //drawLine(out,lelbow,lwrist);
    //drawLine(out,relbow,rwrist);
    drawLine(out,lelbow,lhand);
    drawLine(out,relbow,rhand);
    drawLine(out,lhand,lfinger);
    drawLine(out,rhand,rfinger);
    drawLine(out,neck,torso);
    //drawLine(out,torso,waist);
    drawLine(out,torso,lhip);
    drawLine(out,torso,rhip);
    drawLine(out,lhip,lknee);
    drawLine(out,rhip,rknee);
    //drawLine(out,lknee,lankle);
    //drawLine(out,rknee,rankle);
    drawLine(out,lknee,lfoot);
    drawLine(out,rknee,rfoot);

}

void skeletonutilities::drawLine(cv::Mat &out, XnPoint3D from, XnPoint3D to)
{
    if((from.X!=0.0 || from.Y!=0.0) && (to.X!=0.0 || to.Y!=0.0))
        line(out, Point(from.X,from.Y), Point(to.X,to.Y), cvScalar(255,255,255),4);
}

void skeletonutilities::getSkeleton2D(XnPoint3D *out, xn::DepthGenerator *dg, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    out[0]=getUserJoint2D(dg,ug,player,XN_SKEL_HEAD,confidence);
    out[1]=getUserJoint2D(dg,ug,player,XN_SKEL_NECK,confidence);
    out[2]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_SHOULDER,confidence);
    out[3]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_SHOULDER,confidence);
    out[4]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_ELBOW,confidence);
    out[5]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_ELBOW,confidence);
    out[6]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_HAND,confidence);
    out[7]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_HAND,confidence);
    out[8]=getUserJoint2D(dg,ug,player,XN_SKEL_TORSO,confidence);
    out[9]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_HIP,confidence);
    out[10]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_HIP,confidence);
    out[11]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_KNEE,confidence);
    out[12]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_KNEE,confidence);
    out[13]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_FOOT,confidence);
    out[14]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_FOOT,confidence);
}

void skeletonutilities::getSkeleton3D(XnPoint3D *out, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    out[0]=getUserJoint3D(ug,player,XN_SKEL_HEAD,confidence);
    out[1]=getUserJoint3D(ug,player,XN_SKEL_NECK,confidence);
    out[2]=getUserJoint3D(ug,player,XN_SKEL_LEFT_SHOULDER,confidence);
    out[3]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_SHOULDER,confidence);
    out[4]=getUserJoint3D(ug,player,XN_SKEL_LEFT_ELBOW,confidence);
    out[5]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_ELBOW,confidence);
    out[6]=getUserJoint3D(ug,player,XN_SKEL_LEFT_HAND,confidence);
    out[7]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_HAND,confidence);
    out[8]=getUserJoint3D(ug,player,XN_SKEL_TORSO,confidence);
    out[9]=getUserJoint3D(ug,player,XN_SKEL_LEFT_HIP,confidence);
    out[10]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_HIP,confidence);
    out[11]=getUserJoint3D(ug,player,XN_SKEL_LEFT_KNEE,confidence);
    out[12]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_KNEE,confidence);
    out[13]=getUserJoint3D(ug,player,XN_SKEL_LEFT_FOOT,confidence);
    out[14]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_FOOT,confidence);
}

void skeletonutilities::getHands2D(XnPoint3D *out, xn::DepthGenerator *dg, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    out[0]=getUserJoint2D(dg,ug,player,XN_SKEL_LEFT_HAND,confidence);
    out[1]=getUserJoint2D(dg,ug,player,XN_SKEL_RIGHT_HAND,confidence);
}

void skeletonutilities::getHands3D(XnPoint3D *out, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    out[0]=getUserJoint3D(ug,player,XN_SKEL_LEFT_HAND,confidence);
    out[1]=getUserJoint3D(ug,player,XN_SKEL_RIGHT_HAND,confidence);
}

void skeletonutilities::getAreaOfJoint2D(cv::Mat &dep, XnPoint3D *jointPos,const XnLabel *pLabels, double dist2d, double distZ, double *areaVal, cv::Mat &img)
{
    (*areaVal)=0;
    const XnLabel* labels=pLabels;
    unsigned short *p=(unsigned short*)dep.data;
    Vec3b *im=(Vec3b*)img.data;
    int jX=(int)(jointPos->X-dist2d)>0 ? (int)(jointPos->X-dist2d):jointPos->X;
    int jY=(int)(jointPos->Y-dist2d)>0 ? (int)(jointPos->Y-dist2d):jointPos->Y;
    labels+=640*jY+jX;
    p+=640*jY+jX;
    im+=640*jY+jX;
    int MaxDistX=((int)dist2d+jointPos->X)<640 ? (((int)dist2d+jointPos->X)-jX):(640-jX);
    int MaxDistY=((int)dist2d+jointPos->Y)<480 ? (((int)dist2d+jointPos->Y)-jY):(480-jY);
    for(int y=0;y<MaxDistY;y++){
        for(int x=0;x<MaxDistX;x++){
            if((*labels)!=0 && abs((*p)-jointPos->Z)<distZ){
                (*areaVal)++;
                (*im)[1]=255;
            }
            p++;labels++;im++;
        }
        p+=640-MaxDistX;
        labels+=640-MaxDistX;
        im+=640-MaxDistX;
    }
    //(*areaVal)*=(float)(dep.at<unsigned short>(jointPos->Y,jointPos->X))/2000.0;
}

void skeletonutilities::getAreaOfJoint3D(xn::DepthGenerator *dg, cv::Mat &pC, XnPoint3D *jointPos,const XnLabel *pLabels, double pointDistance, double *areaVal, cv::Mat &img)
{
}

void skeletonutilities::getCOM2D(XnPoint3D *out, xn::DepthGenerator *dg, xn::UserGenerator *ug, XnUserID player)
{
    ug->GetCoM(player,*out);
    dg->ConvertRealWorldToProjective(1,out,out);
}

void skeletonutilities::getCOM3D(XnPoint3D *out, xn::UserGenerator *ug, XnUserID player)
{
    ug->GetCoM(player,*out);
}

double skeletonutilities::distanceBetweenJoints2D(XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2, xn::DepthGenerator *dg, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    XnPoint3D p1,p2;
    p1=getUserJoint2D(dg,ug,player,eJoint1,confidence);
    p2=getUserJoint2D(dg,ug,player,eJoint2,confidence);
    if((p1.X!=0.0 || p1.Y!=0.0) && (p2.X!=0.0 || p2.Y!=0.0)){
        return sqrt(pow((p1.X-p2.X),2)+pow((p1.Y-p2.Y),2));
    }
    else{
        return 100.0;
    }
}

double skeletonutilities::distanceBetweenJoints3D(XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2, xn::UserGenerator *ug, XnUserID player, double confidence)
{
    XnPoint3D p1,p2;
    p1=getUserJoint3D(ug,player,eJoint1,confidence);
    p2=getUserJoint3D(ug,player,eJoint2,confidence);
    if((p1.X!=0.0 || p1.Y!=0.0) && (p2.X!=0.0 || p2.Y!=0.0)){
        return sqrt(pow((p1.X-p2.X),2)+pow((p1.Y-p2.Y),2)+pow((p1.Z-p2.Z),2));
    }
    else{
        return 500.0;
    }
}

void skeletonutilities::convertToComCoords(XnPoint3D *pts, XnPoint3D *com, XnPoint3D *neck, double sizeofArm, int numJoints, bool normalizeUsingArm, bool isCOMOrigin)
{
    XnPoint3D *pt=pts;
    for(int i=0;i<numJoints;i++){
        if(!isCOMOrigin){

        }
        if(normalizeUsingArm){
            double sizeofArmX=sizeofArm + 200;
            pt->X=(pt->X-com->X+sizeofArmX)/(2*sizeofArmX);
            pt->Y=1.0-(pt->Y-com->Y)/(2*sizeofArm);
        }
        pt++;
    }
}


