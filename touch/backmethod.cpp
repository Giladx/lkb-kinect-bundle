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
#include "backmethod.h"
template <class T>
inline std::string to_string (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}
backmethod::backmethod()
{
    //ctor
    nback=0;
    cont=true;
    ACC.create(480,640,CV_32FC1);
    sqACC.create(480,640,CV_32FC1);
    ACCsq.create(480,640,CV_32FC1);
    varianza2.create(480,640,CV_32FC1);
    maskVar.create(480,640,CV_8UC1);
    maskACC.create(480,640,CV_8UC1);
    total=99;
}

backmethod::~backmethod()
{
    //dtor
}
bool backmethod::proc(double varVal,Mat& nue,Mat& mask,Mat& Base,Mat& maskBack){
if(cont)
            {
                if(nback==0)
                {
                    maskACC.setTo(cv::Scalar::all(255));
                    ACC.setTo(cv::Scalar::all(0));
                    ACCsq.setTo(cv::Scalar::all(0));
                    sqACC.setTo(cv::Scalar::all(0));
                    maskVar.setTo(cv::Scalar::all(255));
                }
                if(nback>total)
                {
                    ACC.convertTo(ACC,CV_32FC1,1.0/(total+1));
                    sqACC.convertTo(sqACC,CV_32FC1,1.0/((total+1)));
                    ACCsq=ACC.mul(ACC);
                    varianza2=abs(sqACC-ACCsq);
                    for(int x=0;x<varianza2.rows;x++){
                    for(int y=0;y<varianza2.cols;y++){
                        varianza2.at<float>(x,y)=sqrt(varianza2.at<float>(x,y));
                    }
                    }
                    varianza2=varianza2.mul(1/ACC);
                    threshold(varianza2, maskVar, varVal/1000.0,1.0,CV_THRESH_BINARY_INV);
                    maskVar.convertTo(maskVar,CV_8UC1,255);
                    maskACC=maskACC & maskVar;
                   ACC.convertTo(Base,CV_32FC1,1.0);


                    maskACC.copyTo(maskBack);
                    cont=false;
                    nback=-1;
                }
                else
                {
                    maskACC=maskACC  &  mask;
                    ACC=ACC + nue;
                    sqACC=nue.mul(nue);
                }
                nback++;
                if(cont){return false;}
                else{return true;}
            }
    else{
            return false;
    }
}
