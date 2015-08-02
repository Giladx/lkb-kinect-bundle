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
#include "virtualplane.h"

virtualplane::virtualplane()
{
    //ctor
    pl=new double[6];
    pl[0]=0.0;
    pl[1]=0.0;
    pl[2]=0.0;
    pl[3]=0.0;
    pl[4]=1.0;
    pl[5]=0.0;
    plpc=new double[6];
    plpc[0]=0.0;
    plpc[1]=0.0;
    plpc[2]=0.0;
    plpc[3]=0.0;
    plpc[4]=1.0;
    plpc[5]=0.0;
    created=true;
}
virtualplane::virtualplane(double* p)
{
    //ctor
    pl=new double[5];
    pl[0]=p[0];
    pl[1]=p[1];
    pl[2]=p[2];
    pl[3]=p[3];
    pl[4]=p[4];
    pl[5]=pl[3];
    plpc=new double[5];
    plpc[0]=p[0];
    plpc[1]=p[1];
    plpc[2]=p[2];
    plpc[3]=p[3];
    plpc[4]=p[4];
    plpc[5]=plpc[3];
    created=true;
}
virtualplane::~virtualplane()
{
    //dtor
    delete pl;
    delete plpc;
}

void virtualplane::distPlano(Mat& in,Mat& out,int mod,int neg){
    for(int y=0; y<in.rows; y++)
    {
        for(int x=0; x<in.cols; x++)
        {
            if(mod==1)
            {
                if(neg==0)
                {
                    out.at<float>(y,x)=abs(pl[0]*x+pl[1]*y+in.at<float>(y,x)*pl[2]+pl[3])/pl[4];
                }
                else
                {
                    out.at<float>(y,x)=-(double)(pl[0]*x+pl[1]*y+in.at<float>(y,x)*pl[2]+pl[3])/pl[4];
                }
            }
            else
            {
                out.at<float>(y,x)=(double)(pl[0]*x+pl[1]*y+in.at<float>(y,x)*pl[2]+pl[3])/pl[4];
            }
        }
    }
}
void virtualplane::calcPlano(Mat& depth,Mat& MascaraZona,Mat& Mascara){

    Mat maskTot(480,640,CV_8UC1);
    Mat in(480,640,CV_32FC1);

    maskTot=MascaraZona & Mascara;
    depth.copyTo(in,maskTot);
    int NumeroPuntos=0;

    cout << "VIRTUAL PLANE : PERFORMING FIT, PLEASE WAIT..." << endl;

    //FOR RANSAC
    vector<CvPoint3D32f> datos;
    ransacfit *ranfi=new ransacfit();
    ///////////////
    for(int y=0; y<in.rows; y++)
    {
        for(int x=0; x<in.cols; x++)
        {
            if(in.at<float>(y,x)>0)
            {
                datos.push_back(cvPoint3D32f(x,y,in.at<float>(y,x)));
                NumeroPuntos++;
            }
        }
    }
    ranfi->setData(datos);
        double thrI=0.00015,por1=0.01,por2=0.5;
    int ite=200,maxNum=100;
    //ranfi->doRansac(QP,thrI,por1,por2,ite,maxNum);
    bool continuar=true;

            while(continuar){
            cout << "Repeat with different parameters? (y or n)" << endl;
            char rep;
            cin >> rep;
            if(rep==121){
                cout << endl << "Ideal threshold. Last : "  << thrI << endl;
                cin >> thrI;
                cout << endl <<"Random percent. Last : "  << por1 << endl;
                cin >> por1;
                cout << endl <<"Final percent. Last : "  << por2 << endl;
                cin >> por2;
                cout << endl <<"Max iterations. Last : "  << ite << endl;
                cin >> ite;
                cout << endl <<"Max Number. Last : "  << maxNum << endl;
                cin >> maxNum;
                ranfi->restart();
                //ranfi->doRansac(QP,thrI,por1,por2,ite,maxNum);
            }
            else if(rep==110){
                continuar=false;
            }
            }
    pl[0]=ranfi->currentGoodModel[0];
    pl[1]=ranfi->currentGoodModel[1];
    pl[2]=ranfi->currentGoodModel[2];
    pl[3]=ranfi->currentGoodModel[3];
    pl[4]=ranfi->currentGoodModel[4];
    pl[5]=pl[3];
    delete ranfi;
}
void virtualplane::hacerPlano(Mat& im,Mat& mascara,int fisheye, double k){
    im.create(480,640,CV_32FC1);
    im.setTo(cv::Scalar::all(0));
    for(int y=0; y<480; y++)
    {
        for(int x=0; x<640; x++)
        {
            if(mascara.at<unsigned char>(y,x)>0)
            {
                    im.at<float>(y,x)=(float)(pl[0]*x+pl[1]*y+pl[3])/(-pl[2]);
            }
        }
    }
}
void virtualplane::distPlanoPC(Mat& in,Mat& out){
    for(int y=0; y<out.rows; y++)
    {
        Vec3f* v=(Vec3f*)(in.data+in.step*y);
        float* ptr=(float*)(out.data + out.step*y);
        for(int x=0; x<out.cols; x++)
        {

            if((*v)[0]!=0 || (*v)[1]!=0 || (*v)[2]!=0){

                (*ptr)=(float)(plpc[0]*(*v)[0]+plpc[1]*(*v)[1]+(*v)[2]*plpc[2]+plpc[3])/plpc[4];

            }
            else{
                (*ptr)=0.0;
            }
        v++;ptr++;
        }
    }
}
void virtualplane::calcPlanoPC(Mat& depth,Mat& MascaraZona,Mat& Mascara,QProgressBar* QP){

    int NumeroPuntos=0;

    cout << "VIRTUAL PLANE PC: PERFORMING FIT, PLEASE WAIT..." << endl;

    //FOR RANSAC
    vector<CvPoint3D32f> datos;
    ransacfit *ranfi=new ransacfit();
    ///////////////
    for(int y=0; y<MascaraZona.rows; y++)
    {
        for(int x=0; x<MascaraZona.cols; x++)
        {
            Vec3f vals=depth.at<Vec3f>(y,x);
            if(MascaraZona.at<unsigned char>(y,x)!=0 && (vals[0]!=0 || vals[1]!=0 || vals[2]!=0) )
            {
                datos.push_back(cvPoint3D32f(vals[0],vals[1],vals[2]));
                NumeroPuntos++;
            }
        }
    }
    ranfi->setData(datos);
    double thrI=0.00015,por1=0.01,por2=0.5;
    int ite=200,maxNum=100;
    double gotThre=ranfi->doRansac(QP,thrI,por1,por2,ite,maxNum);
    bool continuar=true;
            while(continuar){
            ransacDialog* rD=new ransacDialog();
            rD->setThre(thrI);
            rD->setError(gotThre);
            rD->setIter(ite);
            rD->setMaxNum(maxNum);
            rD->setPerFin(por2);
            rD->setPerRan(por1);
            int result=rD->exec();
            if(result==1)
               {
                   thrI=rD->getThre();
                   por1=rD->getPerRan();
                   por2=rD->getPerFin();
                   ite=rD->getIter();
                   maxNum=rD->getMaxNum();
                   ranfi->restart();
                   gotThre=ranfi->doRansac(QP,thrI,por1,por2,ite,maxNum);
               }
            else{
                continuar=false;
            }
            delete rD;
            }
    plpc[0]=ranfi->currentGoodModel[0];
    plpc[1]=ranfi->currentGoodModel[1];
    plpc[2]=ranfi->currentGoodModel[2];
    plpc[3]=ranfi->currentGoodModel[3];
    plpc[4]=ranfi->currentGoodModel[4];
    plpc[5]=plpc[3];
    delete ranfi;
}
/// TODO: CHECK IF THIS IS STILL USEFULL AND SPEEDUPO THROUGH POINTERS IF IT IS
void virtualplane::refinePlaneUsingNormals(Mat& pC,Mat& pBlb,int minP,int sqSize,double minDot){
    Vec3f v;
    vector<Vec3f> puntos;
    ransacfit *ranfi=new ransacfit();
    double nor[5];
    for(int y=0; y<pBlb.rows; y++)
    {
        for(int x=0; x<pBlb.cols; x++)
        {
            if(pBlb.at<uchar>(y,x)!=0){
                puntos.clear();
                for(int y2=((y-sqSize)>=0 ? (y-sqSize):0);y2<((y+sqSize+1)>pBlb.rows ? pBlb.rows:(y+sqSize+1));y2++){
                    for(int x2=((x-sqSize)>=0 ? (x-sqSize):0);x2<((x+sqSize+1)>pBlb.cols ? pBlb.cols:(x+sqSize+1));x2++){
                        if(pBlb.at<uchar>(y2,x2)!=0){
                            v=pC.at<Vec3f>(y2,x2);
                            puntos.push_back(v);
                        }
                    }
                }
                if(puntos.size()>=minP){
                    ranfi->minimumSquaresNormalFilter(puntos,nor);
                    if(dotProd(nor,plpc)>minDot){
                        pBlb.at<uchar>(y,x)=0;
                    }
                }
                else{
                    pBlb.at<uchar>(y,x)=0;
                }
            }
            else{
                pBlb.at<uchar>(y,x)=0;
            }
        }
    }
    delete ranfi;
}
double virtualplane::dotProd(double* in1,double* in2){
    return abs(in1[0]*in2[0]+in1[1]*in2[1]+in1[2]*in2[2])/(in1[4]*in2[4]);
}
