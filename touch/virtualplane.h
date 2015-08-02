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
#ifndef VIRTUALPLANE_H
#define VIRTUALPLANE_H
#include <stdlib.h>
#include <ctime>
#include <cv.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ransacfit.h"
#include <vector>
#include <iostream>
#include <QProgressBar>
#include <ransacdialog.h>
#include <QDialogButtonBox>

using namespace cv;
using namespace std;

class virtualplane
{
public:
    virtualplane();
    virtualplane(double* p);
    virtual ~virtualplane();
    double GetPl(int indx)
    {
        if(indx<6 && created){
        return pl[indx];}
        else return 0.0;
    }
    void SetPl(int indx, double val)
    {
        if(indx<6 && created){
            pl[indx] = val;}
    }
    double GetPlPC(int indx)
    {
        if(indx<6 && created){
        return plpc[indx];}
        else return 0.0;
    }
    void SetPlPC(int indx, double val)
    {
        if(indx<6 && created){
            plpc[indx] = val;}
    }
    bool created;
    /*Functions for plane parameter calculation, virtual plane creation and distance to plane meassure*/
    void distPlano(Mat& in,Mat& out,int mod,int neg);
    void calcPlano(Mat& depth,Mat& MascaraZona,Mat& Mascara);
    void hacerPlano(Mat& im,Mat& mascara,int fisheye=0, double k=0);
    void distPlanoPC(Mat& in,Mat& out);
    void calcPlanoPC(Mat& depth,Mat& MascaraZona,Mat& Mascara,QProgressBar* QP);
    void refinePlaneUsingNormals(Mat& pC,Mat& pBlb,int minP,int sqSize,double minDot);
    double dotProd(double* in1,double* in2);
/////////////////////////////////////////////////////////////////////////////
protected:
private:
    double *pl,*plpc;

};

#endif // VIRTUALPLANE_H
