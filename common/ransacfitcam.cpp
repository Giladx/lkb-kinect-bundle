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
#include "ransacfitcam.h"

using namespace cv;
using namespace std;
ransacfitcam::ransacfitcam(){

    //iterations=10000;
    //thresholdIdeal=0.0015;
    currentTestThreshold=10000000000.0;
    currentGoodThreshold=10000000000.0;
    srand(time(NULL));
    //ctor
}
ransacfitcam::~ransacfitcam(){
    //dtor
}
void ransacfitcam::restart(){

    //iterations=10000;
    //thresholdIdeal=0.0015;
    currentTestThreshold=10000000000.0;
    currentGoodThreshold=10000000000.0;
    //ctor
}
void ransacfitcam::selectRandom(vector<Vec3d> datos1,vector<Vec3d> datos2,double porcentaje){
    vector<Vec3d> sample1,sample2,notSample1,notSample2;
    int numDat=porcentaje*datos1.size();
    int* pos=new int[numDat];
    bool va=false;
    int num;
    for(int h=0; h<numDat; h++)
    {
        while(!va)
        {
            num=rand()%datos1.size();
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
        sample1.push_back(datos1[num]);
        sample2.push_back(datos2[num]);
        va=false;
    }
    bool nono=true;
    for(int h=0; h<datos1.size(); h++)
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
            notSample1.push_back(datos1[h]);
            notSample2.push_back(datos2[h]);
        }
        nono=true;
    }
    currentTestData1=sample1;
    currentTestOut1=notSample1;
    currentTestData2=sample2;
    currentTestOut2=notSample2;
    delete pos;
}
void ransacfitcam::setData(vector<Vec3d> datos1, vector<Vec3d> datos2){
    cout << "Num : " << datos1.size() << endl;
    data1=datos1;
    data2=datos2;
    //doRansac();
}
Mat ransacfitcam::calcularModelo(vector<Vec3d> datos1,vector<Vec3d> datos2){
    Mat ca1 = Mat(3,datos1.size(), CV_64F), ca2=Mat(4,datos1.size(), CV_64F);
            for(int i=0;i<datos1.size();i++){
                for(int j=0;j<4;j++){
                    if(j!=3){
                    ca1.at<double>(j,i)=datos1[i][j];
                    ca2.at<double>(j,i)=datos2[i][j];
                    }
                    else{
                    ca2.at<double>(j,i)=1.0;
                    }
                }
            }
    Mat CC=ca1*(ca2.inv(DECOMP_SVD));
    return CC;
}
double ransacfitcam::doRansac(/*QProgressBar* QP,*/double porRan, double porFin,int iterTot,double threId){

    bool terminar=false;
    int repets=0;
    while(!terminar)
    {
        selectRandom(data1,data2,porRan);
        Mat CalAct=calcularModelo(currentTestData1,currentTestData2);
        double threS=diferenciaPromedio(CalAct,currentTestData1,currentTestData2);
        for(int j=0; j<currentTestOut1.size(); j++)
        {
            if(distanciaVectores(currentTestOut1[j], multiplicacion(CalAct,currentTestOut2[j]))<threS)
            {
                currentTestData1.push_back(currentTestOut1[j]);
                currentTestData2.push_back(currentTestOut2[j]);
            }
        }
        threS=diferenciaPromedio(CalAct,currentTestData1,currentTestData2);
        if(threS<currentGoodThreshold && currentTestData1.size()>data1.size()*porFin)
        {
            currentGoodThreshold=threS;
            currentGoodData1=currentTestData1;
            currentGoodData2=currentTestData2;
            CalAct.copyTo(currentGoodModel);
            cout << "Error iteracion " << repets << " : " << currentGoodThreshold << endl;
            if(threS<threId)
            {
                terminar=true;
                cout << "ERROR LOGRADO FINAL: " << currentGoodThreshold << endl;
                //cout << "Plano Logrado : " << currentGoodModel[0] << "," << currentGoodModel[1] << "," << currentGoodModel[2] << endl;
            }
        }
        repets++;
        if(repets>iterTot){
            terminar=true;
            cout << "FIN DE REPETICIONES" << endl;
            cout << "ERROR LOGRADO FINAL: " << currentGoodThreshold << endl;
            //cout << "Plano Logrado : " << currentGoodModel[0] << "," << currentGoodModel[1] << "," << currentGoodModel[2] << endl;
        }
        //QP->setValue(100*((float)repets/iterTot));
    }
    return currentGoodThreshold;
}
double ransacfitcam::diferenciaPromedio(Mat calibracion,vector<Vec3d> datos1,vector<Vec3d> datos2){
    double suma=0;
    for(int i=0;i<datos1.size();i++){
        suma+=distanciaVectores(datos1[i],multiplicacion(calibracion,datos2[i]));
    }
    return (suma/datos1.size());
}
double ransacfitcam::distanciaVectores(Vec3d v1, Vec3d v2){
    return sqrt(pow((v1[0]-v2[0]),2) + pow((v1[1]-v2[1]),2) +pow((v1[2]-v2[2]),2));
}
Vec3d ransacfitcam::multiplicacion(Mat CC,Vec3d vec){
    Vec3d Salida;
                    for(int filas=0;filas<3;filas++){
                            Salida[filas]=0.0;
                        for(int cols=0;cols<4;cols++){
                            if(cols!=3){
                            Salida[filas]+=vec[cols]*(CC.at<double>(filas,cols));
                            }
                            else{
                            Salida[filas]+=CC.at<double>(filas,cols);
                            }
                        }
                    }
    return Salida;
}
