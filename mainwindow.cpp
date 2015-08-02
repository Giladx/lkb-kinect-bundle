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
#include "mainwindow.h"
#include "ui_mainwindow.h"


inline string concatenate(std::string const& name, int i)
{
    stringstream s;
    s << name << i;
    return s.str();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam=new cameras();
    int num=cam->initiateContexts(true);
    ui->scroll3d->setLayout(ui->verticalLayout3d);
    ui->scroll2d->setLayout(ui->verticalLayout2d);
    cout << "CONTEXTOS : " << num << endl;
    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(cameraSettingsCli(int)));
    for(int i=0;i< num;i++){
        QPushButton* but=new QPushButton( QString::fromStdString(concatenate("Camera ",i)));
        ui->verticalLayout3d->addWidget(but);
        signalMapper->setMapping(but, i);
        connect(but, SIGNAL(clicked()), signalMapper, SLOT(map()));
    }
    QSignalMapper *signalMapper2 = new QSignalMapper(this);
    connect(signalMapper2,SIGNAL(mapped(int)),this,SLOT(cameraSettingsCli2(int)));
    for(int i=0;i< num;i++){
        QPushButton* but=new QPushButton( QString::fromStdString(concatenate("Camera ",i)));
        ui->verticalLayout2d->addWidget(but);
        signalMapper2->setMapping(but, i);
        connect(but, SIGNAL(clicked()), signalMapper2, SLOT(map()));
        camconfig* cc=new camconfig();
        camConfigs.push_back(cc);
    }
    if(num>0){
        ui->testinfo->setEnabled(true);
        ui->testUser->setEnabled(true);
        ui->mixViewer->setEnabled(true);
    }
    if(num>1){
        ui->pushButton->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->calibrator3D->setEnabled(true);
    }
    ////SCREEN INFO
    QDesktopWidget* Qaux=new QDesktopWidget();
    QRect screen= Qaux->screenGeometry();
    delete Qaux;
    cout << screen.width() << "," << screen.height() << endl;
    ///////////////////////////////////////////////////////////
    sktCal=new calibrationskt();
    tmw=new tuiomousewrapper();
    if(cam->numContextosIniciados>1){
        ui->testUser->setDisabled(true);
    }
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    loadGeneralSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cam;
    camConfigs.clear();
    delete sktCal;
    delete tmw;
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}


void MainWindow::cameraSettingsCli(int nC)
{
    cameraSettings *camS=new cameraSettings();
    camS->SetGLWCameras(cam,nC);
    camS->setAttribute( Qt::WA_DeleteOnClose, true );
    camS->show();
}
void MainWindow::cameraSettingsCli2(int nC)
{
    cameraSettings2d *camS=new cameraSettings2d(cam,nC,camConfigs[nC]);
    camS->setAttribute( Qt::WA_DeleteOnClose, true );
    camS->show();
}

void MainWindow::on_pushButton_released()
{
    surfacecamcalibration *camC=new surfacecamcalibration(cam,0,1,&matCal[0][1]);
}


void MainWindow::on_pushButton_3_released()
{
    //if(matCal[0][1].rows==3){
    testblobsmixto* tbm=new testblobsmixto(cam,matCal[0][1],camConfigs);
    tbm->setAttribute( Qt::WA_DeleteOnClose, true );
    tbm->show();
    //}
}



void MainWindow::on_testinfo_released()
{
    vector<Mat> aux;
    aux.push_back(matCal[0][1]);
    sktinfoextractor* sktI=new sktinfoextractor(cam,aux,camConfigs,sktCal,tmw);
    sktI->setAttribute( Qt::WA_DeleteOnClose, true );
    sktI->show();
}

void MainWindow::on_testUser_released()
{
    testusuario* tU=new testusuario(cam);
    tU->setAttribute( Qt::WA_DeleteOnClose, true );
    tU->show();
}

void MainWindow::on_tuioMulti_released()
{
    tuiomultiplexer *tm=new tuiomultiplexer();
    tm->show();
}

void MainWindow::on_calibrator3D_released()
{
    camcalibration* cC=new camcalibration(cam,0,1,&matCal[0][1],this);
    cC->show();
}

void MainWindow::on_mixViewer_released()
{
    multi3dviewer* mV=new multi3dviewer(cam,matCal[0][1]);
    mV->setAttribute( Qt::WA_DeleteOnClose, true );
    mV->show();
}
void MainWindow::on_actionAbout_triggered()
{
    aboutdialog* ad=new aboutdialog();
    ad->setAttribute( Qt::WA_DeleteOnClose, true );
    ad->exec();
}

void MainWindow::on_actionLoadPC_triggered()
{
    QString fN = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("PointCalibration (*.pkl)"));
    QByteArray ba = fN.toLocal8Bit();
    char *fileN = ba.data();
    if(fN!=""){
    sktCal->loadCalibration(fileN);}
}

void MainWindow::on_actionSavePC_triggered()
{
    QFileDialog fileDialog(this, tr("Save File"),"",tr("PointCalibration (*.pkl)"));
    fileDialog.setDefaultSuffix("pkl");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    if(fileDialog.exec()){
    QString fN = fileDialog.selectedFiles().first();
    if(fN!=""){
    QByteArray ba = fN.toLocal8Bit();
    char *fileN = ba.data();
    sktCal->saveCalibration(fileN);
    }
    }
}

void MainWindow::on_actionLoadCS_triggered()
{
    QString fN = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("CamSettings (*.cst)"));
    QByteArray ba = fN.toLocal8Bit();
    char *fileN = ba.data();
    if(fN!=""){
        loadCamConfigs(fileN);
    }
}

void MainWindow::on_actionSaveCS_triggered()
{
    QFileDialog fileDialog(this, tr("Save File"),"",tr("CamSettings (*.cst)"));
    fileDialog.setDefaultSuffix("cst");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    if(fileDialog.exec()){
    QString fN = fileDialog.selectedFiles().first();
    if(fN!=""){
    QByteArray ba = fN.toLocal8Bit();
    char *fileN = ba.data();
    saveCamConfigs(fileN);
        }
    }
}

void MainWindow::saveCamConfigs(char *filename){
    if(cam->numContextosIniciados>0){
    FileStorage fs(filename, FileStorage::WRITE);
    fs << "type" << 16;
    fs << "numcams" << cam->numContextosIniciados;
    for(int i=0;i<cam->numContextosIniciados;i++){
        camconfig* cc=camConfigs[i];
        ostringstream ints[7],mats[3],bools[2],floats[7],zone[8],met;
        met << "met" << i;
        for(int k=0;k<7;k++){
            ints[k] << "ints" << i << "-" << k;
            floats[k] << "floats" << i<< "-" << k;
            if(k<3)
                mats[k]<< "mats" << i<< "-" << k;
            if(k<2)
                bools[k]<< "bools" << i<< "-" << k;
        }
        for(int k=0;k<8;k++){
            zone[k] << "zone" << i << "-" << k;
        }
        QByteArray mm = cc->method.toLocal8Bit();
        fs << met.str() << mm.data();
        fs << ints[0].str() << cc->dilIter0;
        fs << ints[1].str() << cc->dilIter1;
        fs << ints[2].str() << cc->erIter;
        fs << ints[3].str() << cc->maxDistance;
        fs << ints[4].str() << cc->minBlobSize;
        fs << ints[5].str() << cc->minDistance;
        fs << ints[6].str() << cc->deltaForAuto;
        fs << mats[0].str() << cc->Background;
        fs << mats[1].str() << cc->maskBack;
        fs << mats[2].str() << cc->maskZona;
        for(int k=0;k<6;k++)
        {fs << floats[k].str() << cc->VP->GetPlPC(k);}
        fs << floats[6].str() << cc->variance;
        fs << bools[0].str() << (int)cc->useCenter << bools[1].str() << (int)cc->zoneSelected;
        for(int k=0;k<4;k++){
            fs << zone[2*k].str() << cc->zone[k].x;
            fs << zone[2*k+1].str() << cc->zone[k].y;
        }

    }
    cout << "Saved Camera Settings File : " << filename << endl;}
    else
        cout << "No Camera Settings to Save" << endl;
}
void MainWindow::loadCamConfigs(char *filename){
    if(cam->numContextosIniciados==0){
        cout << "No cameras available to load settings" << endl;
    }
    else{
    FileStorage fs(filename, FileStorage::READ);
    if(fs.isOpened()){
    int type =(int)fs["type"];
    if(type==16){
        int numCams=(int)fs["numcams"];
        int finalCams=numCams<=cam->numContextosIniciados ? numCams:cam->numContextosIniciados;
        for(int i=0;i<finalCams;i++){
            camconfig* cc=camConfigs[i];
            ostringstream ints[7],mats[3],bools[2],floats[7],zone[8],met;
            met << "met" << i;
            for(int k=0;k<7;k++){
                ints[k] << "ints" << i << "-" << k;
                floats[k] << "floats" << i<< "-" << k;
                if(k<3)
                    mats[k]<< "mats" << i<< "-" << k;
                if(k<2)
                    bools[k]<< "bools" << i<< "-" << k;
            }
            for(int k=0;k<8;k++){
                zone[k] << "zone" << i << "-" << k;
            }
            fs[mats[0].str()] >> cc->Background;
            fs[mats[1].str()] >> cc->maskBack;
            fs[mats[2].str()] >> cc->maskZona;
            cc->useCenter=(bool)((int)fs[bools[0].str()]);
            cc->zoneSelected=(bool)((int)fs[bools[1].str()]);
            cc->variance=(float)fs[floats[6].str()];
            for(int k=0;k<6;k++)
            {cc->VP->SetPlPC(k,(double)fs[floats[k].str()]);}
            for(int k=0;k<4;k++){
                cc->zone[k].x=(int)fs[zone[2*k].str()];
                cc->zone[k].y=(int)fs[zone[2*k+1].str()];
            }
            cc->dilIter0=(int)fs[ints[0].str()];
            cc->dilIter1=(int)fs[ints[1].str()];
            cc->erIter=(int)fs[ints[2].str()];
            cc->maxDistance=(int)fs[ints[3].str()];
            cc->minBlobSize=(int)fs[ints[4].str()];
            cc->minDistance=(int)fs[ints[5].str()];
            cc->deltaForAuto=(int)fs[ints[6].str()];
            QString metodo=QString::fromStdString((string)fs[met.str()]);
            cc->method=metodo;
        }
    cout << "Loaded Camera Settings File : " << filename << endl;
    }
    else
        cout << "Could not load Camera Settings File : " << filename << " - File is not camera settings!" << endl;
    }
    else
        cout << "Could not load Camera Settings File: " << filename << " - Could not open the file..." << endl;
    }
}
void MainWindow::saveGeneralSettings(char *filename, char *filenameCS, char *filenamePC, string host, int port){
    sktCal->saveCalibration(filenamePC);
    saveCamConfigs(filenameCS);
    FileStorage fs(filename, FileStorage::WRITE);
    fs << "Host" << host << "Port" << port << "CamSettingsFile" << string(filenameCS) << "PointCalFile" << string(filenamePC) << "autostart" << 0;
    cout << "Saved General Settings File : " << filename << endl;
}

void MainWindow::on_actionSaveGS_triggered()
{
    saveGeneralSettings();
}
void MainWindow::loadGeneralSettings(char *filename){
    QFile fl(filename);
    if(fl.exists()){
        fl.close();
   FileStorage fs(filename, FileStorage::READ);
   int autostart=(int)fs["autostart"];
   string host=(string)fs["Host"];
   int port=(int)fs["Port"];
   string cSet=(string)fs["CamSettingsFile"];
   string pCal=(string)fs["PointCalFile"];
   char *cs = new char[cSet.size() + 1];
   std::strcpy ( cs, cSet.c_str() );
   char *pc = new char[pCal.size() + 1];
   std::strcpy ( pc, pCal.c_str() );
   sktCal->loadCalibration(pc);
   loadCamConfigs(cs);
   if(autostart!=0){
   vector<Mat> aux;
   aux.push_back(matCal[0][1]);
   sktinfoextractor* sktI=new sktinfoextractor(cam,aux,camConfigs,sktCal,tmw,host,port,true);
   sktI->setAttribute( Qt::WA_DeleteOnClose, true );
   sktI->showMinimized();
   this->setWindowState(Qt::WindowMinimized);
   }
    }
    else{
        fl.close();
    }
}

void MainWindow::on_actionLoadGS_triggered()
{
    loadGeneralSettings();
}
