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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cameras.h>
#include <camerasettings.h>
#include <camerasettings2d.h>
#include <camconfig.h>
#include <glwidget2d.h>
#include <sktinfoextractor.h>
#include <iostream>
#include "glwidget.h"
#include "QPushButton"
#include "QSignalMapper"
#include "surfacecamcalibration.h"
#include "testblobsmixto.h"
#include "QDesktopWidget"
#include <pointcalibration.h>
#include <calibrationskt.h>
#include <omp.h>
#include <tuiomousewrapper.h>
#include <testusuario.h>
#include <tuiomultiplexer.h>
#include <camcalibration.h>
#include <multi3dviewer.h>
#include <aboutdialog.h>
#include <QFileDialog>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    cameras *cam;
    vector<camconfig*> camConfigs;
    Mat matCal[10][10];
    calibrationskt* sktCal;
    tuiomousewrapper* tmw;
private slots:

    void on_actionQuit_triggered();

    void cameraSettingsCli(int nC);
    void cameraSettingsCli2(int nC);

    void on_pushButton_released();

    void on_pushButton_3_released();


    void on_testinfo_released();

   /* void on_actionSave_calibration_2_triggered();

    void on_actionOpen_calibration_triggered();*/

    void on_testUser_released();

    void on_tuioMulti_released();

    void on_calibrator3D_released();

    void on_mixViewer_released();


    void on_actionAbout_triggered();

    void on_actionLoadPC_triggered();

    void on_actionSavePC_triggered();

    void on_actionLoadCS_triggered();

    void on_actionSaveCS_triggered();

    void saveCamConfigs(char *filename="camsettings.cst");

    void loadCamConfigs(char *filename="camsettings.cst");

    void loadGeneralSettings(char *filename="generalsettings.gst");

    void saveGeneralSettings(char *filename="generalsettings.gst",char *filenameCS="camsettings.cst",char *filenamePC="pointcal.pkl",string host="localhost",int port=3333);
    void on_actionSaveGS_triggered();

    void on_actionLoadGS_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
