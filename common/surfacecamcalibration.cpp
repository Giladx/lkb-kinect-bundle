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
#include "surfacecamcalibration.h"
#include "ui_surfacecamcalibration.h"

template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

surfacecamcalibration::surfacecamcalibration(cameras* c,int cam1,int cam2,Mat* calMat,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::surfacecamcalibration)
{
    numPMin=300;
    calM=calMat;
    ui->setupUi(this);
    cam=c;
    for(int i=0; i<cam->numContextosIniciados; i++)
        {
            Mat im,dep,msk,mix,depC(480,640,CV_8UC3),depT,pC,grayImg;
            imgs.push_back(im);
            depths.push_back(dep);
            masks.push_back(msk);
            mixs.push_back(mix);
            depthsC.push_back(depC);
            depthsT.push_back(depT);
            pCs.push_back(pC);
            grayImgs.push_back(grayImg);
        }
    srand((unsigned)time(0));
    timer=new QTimer(this);
    c1=cam1;c2=cam2;
    layout = new QGridLayout();
    gl1=new glwidget2d();
    layout->addWidget(gl1, 0, 0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    this->setLayout(layout);
    this->setAttribute( Qt::WA_DeleteOnClose, true );
    showFullScreen();
    timer->setSingleShot(true);
    timer->start(2000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));

}

surfacecamcalibration::~surfacecamcalibration()
{
    timer->stop();
    delete timer;
    delete layout;
    delete gl1;
    delete ui;
}
void surfacecamcalibration::printSizes(){

}
void surfacecamcalibration::hacerUpdate(){
    gl1->isNewClick=false;
    vector<Vec3d> sal1,sal2;
            drawAndGetPoints(c1,c2,numPMin,&sal1,&sal2);
            if(sal1.size()>10 && sal2.size()>10)
            {
                bool continuar=true;
                ransacfitcam* RAN=new ransacfitcam();
                RAN->setData(sal1,sal2);
                double gotThre=RAN->doRansac(0.1,0.15,20000,0.0005);
                double thrI=0.0005,por1=0.1,por2=0.15;
                int ite=20000;
                while(continuar)
                {
                    ransacDialog* rD=new ransacDialog();
                    rD->setThre(thrI);
                    rD->setError(gotThre);
                    rD->setIter(ite);
                    //rD->setMaxNum(maxNum);
                    rD->setPerFin(por2);
                    rD->setPerRan(por1);
                    int result=rD->exec();
                    if(result==1)
                       {
                           thrI=rD->getThre();
                           por1=rD->getPerRan();
                           por2=rD->getPerFin();
                           ite=rD->getIter();
                           //maxNum=rD->getMaxNum();
                           RAN->restart();
                           gotThre=RAN->doRansac(por1,por2,ite,thrI);
                       }
                    else{
                        continuar=false;
                    }
                    delete rD;

                }

                RAN->currentGoodModel.copyTo((*calM));
                delete RAN;
            }
            close();
    }
void surfacecamcalibration::drawAndGetPoints(int num1, int num2, int numMin, vector<Vec3d> *sal1, vector<Vec3d> *sal2){
       sal1->clear();
       sal2->clear();

       int thre=252;
       int numeroEncontrado[10]= {0,0,0,0,0,0,0,0,0,0};
       Mat calOut(480,640,CV_8UC1);
       utilidades::drawCircle(calOut,100,100,20,40,true,10);

       gl1->updateImage(false,calOut);

       vector<Vec4i> jer[10];
       vector<vector <Point> > contornos[10];
       Moments mom[10];
       Point centros[10];

       bool changePoint=false;
       int numFramesUsable=0;
       int numFramesPunto=0;
       int puntosG=0;
       for(;;)
       {
           if(cam->updateCam(true))
           {
               for(int i=0; i<cam->numContextosIniciados; i++)
               {
                   cam->retriveDepthAndMask(i,depths[i],masks[i]);
                   cam->retrivePointCloud(i,depths[i],pCs[i]);
                   cam->retriveImage(i,imgs[i]);
                   cvtColor(imgs[i],grayImgs[i],CV_RGB2GRAY);
                   //adaptiveThreshold(grayImgs[i], grayImgs[i], 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY, 5, -(double)thre);
                   threshold(grayImgs[i], grayImgs[i], thre, 255,CV_THRESH_BINARY);
                   //imshow(to_string(i),grayImgs[i]);
                   findContours(grayImgs[i], contornos[i], jer[i], CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
                   numeroEncontrado[i]=0;
                   for(int j=0; j<contornos[i].size(); j++)
                   {
                       if(jer[i][j][2]>=0)
                       {
                           if(jer[i][jer[i][j][2]][2]>=0)
                           {
                               //drawContours(grayImgs[i], contornos[i], j, cv::Scalar::all(120),2, 8, jer[i], INT_MAX);
                               numeroEncontrado[i]++;
                               mom[i]=moments(contornos[i][j],true);
                               centros[i].x=mom[i].m10/mom[i].m00;
                               centros[i].y=mom[i].m01/mom[i].m00;
                               //circle(grayImgs[i],centros[i],4, cvScalar(255), -1,8,0);
                           }
                       }
                   }
                   //imshow(to_string(i),grayImgs[i]);
               }
               if(numeroEncontrado[num1]==1 && numeroEncontrado[num2]==1)
               {
                   numFramesUsable++;
               }
               else
               {
                   numFramesUsable=0;
               }
               numFramesPunto++;
               if(numFramesUsable>1)
               {
                   ///ACA LO QUE HAY Q HACER: BUSCAR PUNTO 3D Y PONERLO EN CADA SALIDA
                   Vec3d s1=utilidades::getVectorFromProjectedPoint(centros[num1].x,centros[num1].y,pCs[num1]);
                   Vec3d s2=utilidades::getVectorFromProjectedPoint(centros[num2].x,centros[num2].y,pCs[num2]);
                   if(s1[0]!=0.0 && s1[1]!=0.0 && s1[2]!=0.0 && s2[0]!=0.0 && s2[1]!=0.0 && s2[2]!=0.0)
                   {
                       puntosG++;
                       sal1->push_back(s1);
                       sal2->push_back(s2);
                       //cout << sal1.size() << "," << sal2.size() << endl;
                   }
                   numFramesPunto=0;
                   numFramesUsable=0;
                   changePoint=true;
               }
               else if(numFramesPunto>5)
               {
                   numFramesPunto=0;
                   changePoint=true;
                   numFramesUsable=0;
               }

           }
           if(changePoint)
           {
               changePoint=false;
               utilidades::drawCircle(calOut,(rand()%200)+200,(rand()%300)+100,30,40,true,10);
               putText(calOut, to_string(puntosG), Point(40,40), FONT_HERSHEY_COMPLEX_SMALL, 1.0, cvScalar(255), 2, CV_AA);
               gl1->updateImage(false,calOut);
           }
           if(puntosG>numMin)
           {
               break;
           }
           QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
           if(gl1->isNewClick){
               break;
           }
       }

}
