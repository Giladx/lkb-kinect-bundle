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
#ifndef TESTUSUARIO_H
#define TESTUSUARIO_H

#include <QDialog>
#include <cameras.h>
#include <glwidget2d.h>
#include <QTimer>
#include <skeletonutilities.h>
#include <tuiodatasender.h>
#include <QMessageBox>

using namespace xn;
namespace Ui {
    class testusuario;
}

class testusuario : public QDialog
{
    Q_OBJECT

public:
    explicit testusuario(cameras* c,QWidget *parent = 0);
    ~testusuario();
    cameras* cam;
    Ui::testusuario *ui;
    XnChar g_strPose[20];
    bool needPose;
    QTimer *timer;
    static testusuario* instance;
    static XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
    // Callback: New user was detected
    static void XN_CALLBACK_TYPE newUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
    // Callback: An existing user was lost
    static void XN_CALLBACK_TYPE lostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
    // Callback: Detected a pose
    static void XN_CALLBACK_TYPE poseDetected(PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie);
    // Callback: Started calibration
    static void XN_CALLBACK_TYPE calibrationStart(SkeletonCapability& capability, XnUserID nId, void* pCookie);
    // Callback: Finished calibration
    static void XN_CALLBACK_TYPE calibrationComplete(SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie);
    static void XN_CALLBACK_TYPE calibrationInProgress(SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie);
    static void XN_CALLBACK_TYPE poseInProgress(PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie);
    std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;
    SceneMetaData smd;
    Mat dep,msk,depC,depAux;
    skeletonutilities *sku;
    Vec3b colors[10];
    bool sendTuio;
    tuiodatasender* tds;
    bool isServerCreated;
    double handAreas[20];
    int modAnt[10];
    int areHandsCursorOrObject(int UserId, double* newAreas, double minA,double maxA,double minDer,double maxDer,int modAnt);
    bool removeUserTuio[10];
private slots:
        void hacerUpdate();
        void on_sendTuioCheck_toggled(bool checked);
};

#endif // TESTUSUARIO_H
