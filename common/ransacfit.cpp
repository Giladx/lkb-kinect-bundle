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
#include "ransacfit.h"

ransacfit::ransacfit()
{
    currentGoodThreshold=10000000000.0;
    srand(time(NULL));
    Mat1=cvCreateMat(4,4,CV_32FC1);
    Xresultado=cvCreateMat(4,1,CV_32FC1);
    MM1=Mat1;
    Xres=Xresultado;
    //ctor
}

ransacfit::~ransacfit()
{
    //dtor
}
void ransacfit::restart(){
    currentGoodThreshold=10000000000.0;
}

///TODO IMPROVE USING VECTOR SHUFFLE AND BOOLEAN RANDOM
void ransacfit::selectRandom(vector<CvPoint3D32f> datos, double porcentaje,int maxNum)
{
    vector<CvPoint3D32f> sample,notSample;
    int numDat=porcentaje*datos.size();
    if(numDat>maxNum){numDat=maxNum;}
    int* pos=new int[numDat];
    bool va=false;
    int num;
    for(int h=0; h<numDat; h++)
    {
        while(!va)
        {
            num=rand()%datos.size();
            va=true;
            for(int k=0; k<h; k++)
            {
                if(pos[k]==num )//|| distanciaPP(datos[pos[k]],datos[num])<10.0)
                {
                    va=false;
                }
            }
        }
        pos[h]=num;
        sample.push_back(datos[num]);
        va=false;
    }
    bool nono=true;
    for(int h=0; h<datos.size(); h++)
    {
        for(int k=0; k<numDat; k++)
        {
            if(h==pos[k])
            {
                nono=false;
            }
        }
        if(nono)
        {
            notSample.push_back(datos[h]);
        }
        nono=true;
    }
    currentTestData=sample;
    currentTestOut=notSample;
    delete pos;
}
void ransacfit::setData(vector<CvPoint3D32f> datos)
{
    cout << "Num : " << datos.size() << endl;
    data=datos;
}
double ransacfit::minimumSquares(vector<CvPoint3D32f> dat)
{
    double sXX=0,sXY=0,sYY=0,sZX=0,sZY=0,sX=0,sY=0,sZ=0,sZZ=0;
    for(int j=0; j<dat.size(); j++)
    {
        sXX+=dat[j].x*dat[j].x;
        sXY+=dat[j].x*dat[j].y;
        sYY+=dat[j].y*dat[j].y;
        sZX+=dat[j].z*dat[j].x;
        sZY+=dat[j].z*dat[j].y;
        sX+=dat[j].x;
        sY+=dat[j].y;
        sZ+=dat[j].z;
        sZZ+=dat[j].z*dat[j].z;
    }
    MM1.at<float>(0,0)=sXX;
    MM1.at<float>(0,1)=sXY;
    MM1.at<float>(0,2)=sZX;
    MM1.at<float>(0,3)=sX;
    MM1.at<float>(1,0)=sXY;
    MM1.at<float>(1,1)=sYY;
    MM1.at<float>(1,2)=sZY;
    MM1.at<float>(1,3)=sY;
    MM1.at<float>(2,0)=sZX;
    MM1.at<float>(2,1)=sZY;
    MM1.at<float>(2,2)=sZZ;
    MM1.at<float>(2,3)=sZ;
    MM1.at<float>(3,0)=sX;
    MM1.at<float>(3,1)=sY;
    MM1.at<float>(3,2)=sZ;
    MM1.at<float>(3,3)=dat.size();
    double pll[4];
    cv::SVD::solveZ(MM1,Xres);
    pll[0]=Xres.at<float>(0,0);
    pll[1]=Xres.at<float>(1,0);
    pll[2]=Xres.at<float>(2,0);
    pll[3]=Xres.at<float>(3,0);
    double pT[5];
    pT[0]=pll[0];
    pT[1]=pll[1];
    pT[2]=pll[2];
    pT[3]=pll[3];
    pT[4]=sqrt(pT[0]*pT[0]+pT[1]*pT[1]+pT[2]*pT[2]);
    double error=0;
    for(int j=0; j<dat.size(); j++)
    {
        error+=sqrt(pow((pT[0]*dat[j].x+pT[1]*dat[j].y+dat[j].z*pT[2]+pT[3])/pT[4],2));
    }
    for (int o=0;o<5;o++){
    currentTestModel[o]=pT[o];}
    return (double)error/dat.size();
}
double ransacfit::doRansac(QProgressBar* QP,double threI,double por1, double por2, int rep,int maxNum)
{

    bool terminar=false;
    int repets=0;
    while(!terminar)
    {
        selectRandom(data,por1,maxNum);
        double threS=minimumSquares(currentTestData);
        for(int j=0; j<currentTestOut.size(); j++)
        {
            if(distanciaPM(currentTestOut[j])<threS)
            {
                currentTestData.push_back(currentTestOut[j]);
            }
        }
        threS=minimumSquares(currentTestData);
        if(threS<currentGoodThreshold && currentTestData.size()>data.size()*por2)
        {
            currentGoodThreshold=threS;
            currentGoodData=currentTestData;
            for (int o=0;o<5;o++){
            currentGoodModel[o]=currentTestModel[o];}

            if(threS<threI)
            {
                terminar=true;
                cout << "RANSAC Threshold accomplished." << endl;
                cout << "Best model error : " << currentGoodThreshold << endl;
            }
            else{
                cout << "Error from iteration " << repets << " : " << currentGoodThreshold << endl;
            }
        }
        repets++;
        if(repets>rep){
            terminar=true;
            cout << "Maximum iterations reached." << endl;
            cout << "Best model error : " << currentGoodThreshold << endl;
        }
        QP->setValue(100*((float)repets/rep));
    }
    return currentGoodThreshold;
}
double ransacfit::distanciaPM(CvPoint3D32f p)
{
    return sqrt(pow((currentTestModel[0]*p.x+currentTestModel[1]*p.y+currentTestModel[2]*p.z+currentTestModel[3])/currentTestModel[4],2));
}
double ransacfit::distanciaPP(CvPoint3D32f p,CvPoint3D32f p2)
{
    return sqrt((p.x-p2.x)*(p.x-p2.x)+(p.y-p2.y)*(p.y-p2.y)+(p.z-p2.z)*(p.z-p2.z));
}

double ransacfit::minimumSquaresNormalFilter(vector<Vec3f> dat, double *out)
{
    double sXX=0,sXY=0,sYY=0,sZX=0,sZY=0,sX=0,sY=0,sZ=0,sZZ=0;

    for(int j=0; j<dat.size(); j++)
    {
        sXX+=dat[j][0]*dat[j][0];
        sXY+=dat[j][0]*dat[j][1];
        sYY+=dat[j][1]*dat[j][1];
        sZX+=dat[j][2]*dat[j][0];
        sZY+=dat[j][2]*dat[j][1];
        sX+=dat[j][0];
        sY+=dat[j][1];
        sZ+=dat[j][2];
        sZZ+=dat[j][2]*dat[j][2];
    }

    MM1.at<float>(0,0)=sXX;
    MM1.at<float>(0,1)=sXY;
    MM1.at<float>(0,2)=sZX;
    MM1.at<float>(0,3)=sX;
    MM1.at<float>(1,0)=sXY;
    MM1.at<float>(1,1)=sYY;
    MM1.at<float>(1,2)=sZY;
    MM1.at<float>(1,3)=sY;
    MM1.at<float>(2,0)=sZX;
    MM1.at<float>(2,1)=sZY;
    MM1.at<float>(2,2)=sZZ;
    MM1.at<float>(2,3)=sZ;
    MM1.at<float>(3,0)=sX;
    MM1.at<float>(3,1)=sY;
    MM1.at<float>(3,2)=sZ;
    MM1.at<float>(3,3)=dat.size();
    cv::SVD::solveZ(MM1,Xres);
    out[0]=Xres.at<float>(0,0);
    out[1]=Xres.at<float>(1,0);
    out[2]=Xres.at<float>(2,0);
    out[3]=Xres.at<float>(3,0);
    out[4]=sqrt(out[0]*out[0]+out[1]*out[1]+out[2]*out[2]);
    double error=0;
    for(int j=0; j<dat.size(); j++)
    {
        error+=sqrt(pow((out[0]*dat[j][0]+out[1]*dat[j][1]+dat[j][2]*out[2]+out[3])/out[4],2));
    }
    return (double)error/dat.size();
}
