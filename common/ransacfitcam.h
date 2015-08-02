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
#ifndef RANSACFITCAM_H
#define RANSACFITCAM_H

#include <stdlib.h>
#include <stdio.h>
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

class ransacfitcam
{
    public:
        ransacfitcam();
        virtual ~ransacfitcam();
        /*
        Funciones
        */
        void setData(vector<Vec3d> datos1, vector<Vec3d> datos2);
        void selectRandom(vector<Vec3d> datos1,vector<Vec3d> datos2,double porcentaje);
        Mat calcularModelo(vector<Vec3d> datos1,vector<Vec3d> datos2);
        double diferenciaPromedio(Mat calibracion,vector<Vec3d> datos1,vector<Vec3d> datos2);
        double doRansac(/*QProgressBar* QP,*/double porRan, double porFin,int iterTot,double threId);
        Vec3d multiplicacion(Mat CC,Vec3d vec);
        double distanciaVectores(Vec3d v1, Vec3d v2);
        Mat currentGoodModel;
        void restart();
    protected:
    private:
        vector<Vec3d> data1,data2;
        vector<Vec3d> currentTestData1,currentTestData2;
        vector<Vec3d> currentTestOut1,currentTestOut2;
        vector<Vec3d> currentGoodData1,currentGoodData2;

        double thresholdIdeal;
        double currentTestThreshold;
        double currentGoodThreshold;

};

#endif // RANSACFITCAM_H
