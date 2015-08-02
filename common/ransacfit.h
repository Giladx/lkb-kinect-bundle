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
#ifndef RANSACFIT_H
#define RANSACFIT_H

#include <stdlib.h>
#include <ctime>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <QProgressBar>

using namespace cv;
using namespace std;
class ransacfit
{
    public:
        ransacfit();
        virtual ~ransacfit();
        /*
        Funciones
        */
        void restart();
        void setData(vector<CvPoint3D32f> datos);
        void selectRandom(vector<CvPoint3D32f> datos,double porcentaje,int maxNum=200);
        double minimumSquares(vector<CvPoint3D32f> dat);
        double doRansac(QProgressBar* QP,double threI=0.00015,double por1=0.01, double por2=0.5, int rep=1000,int maxNum=100);
        void prueba(vector<CvPoint3D32f> datos);
        double distanciaPM(CvPoint3D32f p);
        double distanciaPP(CvPoint3D32f p,CvPoint3D32f p2);
        double minimumSquaresNormalFilter(vector<Vec3f> dat, double *out);
        double currentGoodModel[5];
        CvMat *Mat1;
        CvMat *Xresultado;
        Mat MM1,Xres;
    protected:
    private:
        vector<CvPoint3D32f> data;
        vector<CvPoint3D32f> currentTestData;
        vector<CvPoint3D32f> currentTestOut;
        vector<CvPoint3D32f> currentGoodData;

        double plVal[5];
        double currentTestModel[5];

        double currentGoodThreshold;
};

#endif // RANSACFIT_H
