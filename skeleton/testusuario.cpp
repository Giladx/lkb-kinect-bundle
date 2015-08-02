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
#include "testusuario.h"
#include "ui_testusuario.h"
testusuario *testusuario::instance = 0;
XnCallbackHandle testusuario::hUserCallbacks, testusuario::hCalibrationStart, testusuario::hCalibrationComplete, testusuario::hPoseDetected, testusuario::hCalibrationInProgress, testusuario::hPoseInProgress;
testusuario::testusuario(cameras *c,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testusuario)
{
    ui->setupUi(this);
    cam=c;
    needPose=false;
    instance=this;
    cam->userG[0].RegisterUserCallbacks(instance->newUser,instance->lostUser, NULL, instance->hUserCallbacks);
    cam->userG[0].GetSkeletonCap().RegisterToCalibrationStart(instance->calibrationStart, NULL, instance->hCalibrationStart);
    cam->userG[0].GetSkeletonCap().RegisterToCalibrationComplete(instance->calibrationComplete, NULL, instance->hCalibrationComplete);
    if (cam->userG[0].GetSkeletonCap().NeedPoseForCalibration())
    {
        needPose=true;
        cam->userG[0].GetPoseDetectionCap().RegisterToPoseDetected(instance->poseDetected, NULL, instance->hPoseDetected);
        cam->userG[0].GetSkeletonCap().GetCalibrationPose(g_strPose);
    }
    cam->userG[0].GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    cam->userG[0].GetSkeletonCap().RegisterToCalibrationInProgress(instance->calibrationInProgress, NULL, instance->hCalibrationInProgress);
    cam->userG[0].GetPoseDetectionCap().RegisterToPoseInProgress(instance->poseInProgress, NULL, instance->hPoseInProgress);
    cam->userG[0].StartGenerating();
    sku=new skeletonutilities();
    for (int i=0;i<10;i++){
        Vec3b a;
        a[0]=(i%2)*255;
        a[1]=(1-(i%2))*255;
        a[2]=(float)(i%3)/2.0*255;
        colors[i]=a;
        modAnt[i]=0;
        removeUserTuio[i]=false;
    }
    sendTuio=false;
    isServerCreated=false;
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    timer=new QTimer(this);
    timer->start(20);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(hacerUpdate()));
}

testusuario::~testusuario()
{
    timer->stop();
    cam->userG[0].StopGenerating();
    cam->userG[0].UnregisterUserCallbacks(instance->hUserCallbacks);
    cam->userG[0].GetSkeletonCap().UnregisterFromCalibrationStart(instance->hCalibrationStart);
    cam->userG[0].GetSkeletonCap().UnregisterFromCalibrationComplete(instance->hCalibrationComplete);
    cam->userG[0].GetSkeletonCap().UnregisterFromCalibrationInProgress(instance->hCalibrationInProgress);
    if(needPose){
        cam->userG[0].GetPoseDetectionCap().UnregisterFromPoseDetected(instance->hPoseDetected);
    }
    cam->userG[0].GetPoseDetectionCap().UnregisterFromPoseInProgress(instance->hPoseInProgress);
    instance=0;
    delete ui;
    delete timer;
    delete sku;
    if(isServerCreated){
        delete tds;
    }
    cout << "Unregistered."<< endl;
}

void XN_CALLBACK_TYPE testusuario::newUser(xn::UserGenerator &generator, XnUserID nId, void *pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    cout << "New user, ID = " << nId<< endl;
    // New user found
    if (instance->needPose)
    {
        generator.GetPoseDetectionCap().StartPoseDetection(instance->g_strPose, nId);
    }
    else
    {
        generator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

void XN_CALLBACK_TYPE testusuario::lostUser(xn::UserGenerator &generator, XnUserID nId, void *pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    cout << "Lost user, ID = " << nId<< endl;
    if(instance->sendTuio && (instance->tds->isCreated[nId] || instance->tds->isCreatedHand[nId])){
        instance->removeUserTuio[nId]=true;
        //instance->tds->removeUserSkeleton(nId);
        //instance->tds->isCreated[nId]=false;
    }
}

void testusuario::poseDetected(xn::PoseDetectionCapability &capability, const XnChar *strPose, XnUserID nId, void *pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
    instance->cam->userG[0].GetPoseDetectionCap().StopPoseDetection(nId);
    instance->cam->userG[0].GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void testusuario::calibrationStart(xn::SkeletonCapability &capability, XnUserID nId, void *pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    cout << "Calibration Started for ID = " << nId<< endl;
}

void testusuario::calibrationComplete(xn::SkeletonCapability &capability, XnUserID nId, XnCalibrationStatus eStatus, void *pCookie)
{
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        cout << "Calibration Succeded for ID = " << nId<< endl;
        instance->cam->userG[0].GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        cout << "Calibration Failed for ID = " << nId<< endl;
        instance->cam->userG[0].GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

void testusuario::calibrationInProgress(xn::SkeletonCapability &capability, XnUserID id, XnCalibrationStatus calibrationError, void *pCookie)
{
    instance->m_Errors[id].first = calibrationError;
}

void testusuario::poseInProgress(xn::PoseDetectionCapability &capability, const XnChar *strPose, XnUserID id, XnPoseDetectionStatus poseError, void *pCookie)
{
    instance->m_Errors[id].second = poseError;
}

void testusuario::hacerUpdate()
{
    if(cam->numContextosIniciados>0){
        for(int i=0;i<10;i++){
            if(removeUserTuio[i]){
                if(tds->isCreatedHand[i]){
                    tds->removeUserHands(i);
                    tds->isCreatedHand[i]=false;
                }
                if(tds->isCreated[i]){
                    tds->removeUserSkeleton(i);
                    tds->isCreated[i]=false;
                }
                removeUserTuio[i]=false;
                cam->userG[0].GetSkeletonCap().StopTracking(i);
            }
        }
        if(cam->updateCamAndUsers(false,0)){
            cam->retriveDepthAndMask(0,dep,msk);
            if(ui->updateImageBox->isChecked()){
                if(!ui->enableRGB->isChecked()){
                    dep.convertTo(depAux,CV_8UC1,255.0/6000);
                    cvtColor(depAux,depC,CV_GRAY2RGB);}
                else{
                    cam->retriveImage(0,depC);
                    cvtColor(depC,depC,CV_BGR2RGB);
                }
            }
            XnUserID aUsers[10];
            XnUInt16 nUsers = 10;
            cam->userG[0].GetUsers(aUsers, nUsers);
            for(int i=0; i<nUsers; i++)
            {
                bool done=false;
                if(cam->userG[0].GetSkeletonCap().IsTracking(aUsers[i])){
                    if(ui->updateImageBox->isChecked()){
                        if(!ui->enableRGB->isChecked()){
                            cam->userG[0].GetUserPixels(aUsers[i], smd);
                            const XnLabel* pLabels = smd.Data();
                            Vec3b *recMat=(Vec3b*)(depC.data);
                            for (int nY=0; nY<480; nY++)
                            {
                                for (int nX=0; nX < 640; nX++)
                                {
                                    if((*pLabels)!=0 && (*pLabels)==aUsers[i]){
                                        (*recMat)=colors[aUsers[i]];
                                    }
                                    pLabels++;
                                    recMat++;
                                }
                            }
                        }
                        sku->drawSkeleton(depC,&(cam->depthG[0]),&(cam->userG[0]),aUsers[i],ui->jointConfidence->value());
                    }
                    if(sendTuio){
                        if(!ui->useOnlyHands->isChecked()){
                        XnPoint3D sk[15];
                        sku->getSkeleton3D(sk,&(cam->userG[0]),aUsers[i],ui->jointConfidence->value());
                        if(tds->isCreated[aUsers[i]])
                            tds->sendSkeleton(aUsers[i],sk);
                        else{
                            tds->addUserSkeleton(aUsers[i],sk);
                            tds->isCreated[aUsers[i]]=true;
                        }
                        }
                        else{
                            XnPoint3D hnd[2];double areas[2];
                            sku->getHands2D(hnd,&(cam->depthG[0]),&(cam->userG[0]),aUsers[i],ui->jointConfidence->value());
                            if(!ui->usePush->isChecked()){
                                if(tds->isCreatedHand[aUsers[i]])
                                    tds->sendHands(aUsers[i],hnd);
                                else{
                                    tds->addUserHands(aUsers[i],hnd);
                                    tds->isCreatedHand[aUsers[i]]=true;
                                }
                            }
                            else{
                                sku->getAreaOfJoint2D(dep,&hnd[0],smd.Data(),ui->dist2d->value(),ui->distZ->value(),&areas[0],depC);
                                sku->getAreaOfJoint2D(dep,&hnd[1],smd.Data(),ui->dist2d->value(),ui->distZ->value(),&areas[1],depC);
                                int mod=areHandsCursorOrObject(aUsers[i],areas,ui->minArea->value(),ui->maxArea->value(),ui->minDeriv->value(),ui->maxDeriv->value(),modAnt[aUsers[i]]);
                                modAnt[aUsers[i]]=mod;
                                if(ui->fromCOM->isChecked()){
                                    XnPoint3D com,neck;
                                    sku->getUserJoint3D(&(cam->userG[0]),aUsers[i],XN_SKEL_NECK,ui->jointConfidence->value());
                                    sku->getCOM3D(&com,&(cam->userG[0]),aUsers[i]);
                                    sku->getHands3D(hnd,&(cam->userG[0]),aUsers[i],ui->jointConfidence->value());
                                    sku->convertToComCoords(hnd,&com,&neck,300.0);
                                }
                                if(tds->isCreatedHand[aUsers[i]])
                                    tds->sendHands(aUsers[i],hnd,mod);
                                else{
                                    tds->addUserHands(aUsers[i],hnd);
                                    tds->isCreatedHand[aUsers[i]]=true;
                                }
                            }
                        }
                    }
                }
            }

            ui->visor->updateImage(true,depC,1.0);
        }
    }
}

void testusuario::on_sendTuioCheck_toggled(bool checked)
{
    sendTuio=checked;
    if(checked){
        QByteArray   bytes  = ui->tuioHost->text().toAscii();
        const char * host    = bytes.data();
        try{
            tds=new tuiodatasender(host,ui->tuioPort->value());
            isServerCreated=true;
            tds->server->setVerbose(ui->checkBox->isChecked());
            cout << "Tuio Sender Created" << endl;
            ui->tuioHost->setDisabled(true);
            ui->tuioPort->setDisabled(true);
            ui->checkBox->setDisabled(true);
        }
        catch(std::exception &e){
            QMessageBox* msg=new QMessageBox(QMessageBox::Critical,"Error","Could not create the tuio server.\nCheck the parameters.");
            msg->exec();
            ui->sendTuioCheck->setChecked(false);
            sendTuio=false;
        }

    }
    else{
        if(isServerCreated){
        delete tds;
        tds=NULL;
        cout << "Tuio Sender Deleted" << endl;
        isServerCreated=false;
        ui->tuioHost->setDisabled(false);
        ui->tuioPort->setDisabled(false);
        ui->checkBox->setDisabled(false);
        }
    }
}

int testusuario::areHandsCursorOrObject(int UserId, double *newAreas, double minA, double maxA, double minDer, double maxDer,int modAnt)
{
    int mod=0;
    for(int i=0;i<2;i++){
        double diff=0;
        if(handAreas[2*(UserId-1)+i]+newAreas[i]>0){
        diff=(newAreas[i]-handAreas[2*(UserId-1)+i])/(handAreas[2*(UserId-1)+i]+newAreas[i]);
        if(i==0){
        //cout << "DIFF " << i << " : " << diff << endl;
        //cout << "AREA " << i << " : " << newAreas[i] << endl;
        }
        bool sgn=diff>0;
        if(((!sgn && diff<minDer) || newAreas[i]<minA) && newAreas[i]<maxA){
            mod+=(int)pow(2.0,i);
        }
        else if((modAnt==3 || modAnt==(i+1)) && diff<maxDer && newAreas[i]<maxA){
            mod+=(int)pow(2.0,i);
        }
        handAreas[2*(UserId-1)+i]=newAreas[i];
        }
    }
    return mod;
}

