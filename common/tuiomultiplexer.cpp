/*
tuiomultiplexer.cpp - LTmgc (Ludique's TUIO Multiplexer, Gestures and Screen Calibrator)

Version 0.1 Copyright (C) 2012 LudiqueLabs

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications,
and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software in
       a product, an acknowledgment in the product documentation would be appreciated
       but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented
       as being the original software.

    3. This notice may not be removed or altered from any source distribution.*/
#include "tuiomultiplexer.h"
#include "ui_tuiomultiplexer.h"

tuiomultiplexer::tuiomultiplexer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tuiomultiplexer)
{
    ui->setupUi(this);
    grid=new QGridLayout();
    gridGest=new QGridLayout();
    titulos[0]=new QLabel("N");
    titulos[1]=new QLabel("Host");
    titulos[2]=new QLabel("Port");
    titulos[3]=new QLabel("Active");
    titulos[4]=new QLabel("Gesture ID");
    titulos[5]=new QLabel("Train Gesture");
    gridGest->addWidget(titulos[4],0,0,1,1);
    gridGest->addWidget(titulos[5],0,1,1,1);
    gridGest->setRowStretch(0,0);
    grid->addWidget(titulos[0],0,0,1,1);
    grid->addWidget(titulos[1],0,1,1,1);
    grid->addWidget(titulos[2],0,2,1,1);
    grid->addWidget(titulos[3],0,3,1,1);
    signalMapper = new QSignalMapper(this);
    signalMapperGest=new QSignalMapper(this);
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(activated(int)));
    connect(signalMapperGest,SIGNAL(mapped(int)),this,SLOT(startTraining(int)));
    for(int i=0;i<NUM_SER;i++){
        ports[i]=new QSpinBox();
        ports[i]->setMinimum(0);
        ports[i]->setMaximum(65000);
        ports[i]->setValue(3333+i+1);
        hosts[i]=new QLineEdit("localhost");
        labels[i]=new QLabel(QString::number(i+1));
        actives[i]=new QCheckBox();
        grid->addWidget(labels[i],i+1,0,1,1);
        grid->addWidget(hosts[i],i+1,1,1,1);
        grid->addWidget(ports[i],i+1,2,1,1);
        grid->addWidget(actives[i],i+1,3,1,1);
        signalMapper->setMapping(actives[i], i);
        connect(actives[i], SIGNAL(released()), signalMapper, SLOT(map()));
        serverCreated[i]=false;
    }
    grid->setRowStretch(0,0);
    for(int i=0;i<NUM_SER;i++){
        grid->setRowStretch(i+1,1);
    }
    ui->scrollAreaContentTUIO->setLayout(grid);
    //ui->groupBox_2->setLayout(grid);
    ui->scrollAreaWidgetContents->setLayout(gridGest);
        ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        ui->scrollAreaWidgetContents->resize(ui->scrollGest->size().width() ,ui->scrollGest->size().height());
        ui->scrollGest->setWidgetResizable(true);
        ui->scrollGest->setWidget(ui->scrollAreaWidgetContents);
        ui->scrollAreaWidgetContents->adjustSize();
    this->setAttribute( Qt::WA_DeleteOnClose, true );
    clientCreated=false;
    clientHasBeenUsed=false;
    updateM=false;
    areWeCalibrating=false;
    pp=NULL;
    kindNew=4;
    workingPP=false;
    calSKT=new calibrationskt();
    this->setWindowIcon(QIcon(":/imagenes/isotipo.png"));
    timer=new QTimer(this);
    timer->start(30);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateMats()));
    //calSKT->saveCalibration();
}

tuiomultiplexer::~tuiomultiplexer()
{
    if(clientHasBeenUsed){
        client->removeAllTuioListeners();
        client->disconnect();
        client->socket->AsynchronousBreak();
        delete client;
    }
    delete ui;
    for(int i=0;i<NUM_SER;i++){
        delete ports[i];
        delete hosts[i];
        delete labels[i];
        delete actives[i];
        if(serverCreated[i]){
            delete servers[i];
        }
    }
    for(int i=0;i<6;i++){
        delete titulos[i];
    }
    delete grid;
    delete gridGest;
    for(int i=0;i<gestID.size();i++){
        delete gestID[i];
        delete trainGest[i];
        delete gestIm[i];
        delete gestObjs[i];
        delete gestDetected[i];
    }
    gestMats.clear();
    delete calSKT;
}

void tuiomultiplexer::addTuioObject(TUIO::TuioObject *tobj)
{
    if(!actives[0]->isChecked()){
    cout << "OBJECT ADDED" << endl;
    }
    objToAdd.push_back(tobj);
}

void tuiomultiplexer::updateTuioObject(TUIO::TuioObject *tobj)
{
    if(!actives[0]->isChecked()){
    cout << "OBJECT UP" << endl;
    }
    objToUpdate.push_back(tobj);
}

void tuiomultiplexer::removeTuioObject(TUIO::TuioObject *tobj)
{
    if(!actives[0]->isChecked()){
    cout << "OBJECT REM" << endl;
    }
    objToRemove.push_back(tobj);
}

void tuiomultiplexer::addTuioCursor(TUIO::TuioCursor *tcur)
{
    if(!actives[0]->isChecked()){
    cout << "CUR ADDED" << endl;
    }
    curToAdd.push_back(tcur);
    if(pp!=NULL && tcur->getCursorID()==0){
        xNew=tcur->getX();
        yNew=tcur->getY();
        kindNew=0;
    }
}

void tuiomultiplexer::updateTuioCursor(TUIO::TuioCursor *tcur)
{
   /* if(!actives[0]->isChecked()){
    cout << "CUR UPP" << endl;
    }*/
    curToUpdate.push_back(tcur);
    if(pp!=NULL && tcur->getCursorID()==0){
        xNew=tcur->getX();
        yNew=tcur->getY();
        kindNew=1;
    }
}

void tuiomultiplexer::removeTuioCursor(TUIO::TuioCursor *tcur)
{
    if(!actives[0]->isChecked()){
    cout << "CUR REM" << endl;
    }
    if(pp!=NULL && tcur->getCursorID()==0){
        xNew=tcur->getX();
        yNew=tcur->getY();
        kindNew=2;
    }
    double init=(double)(tcur->getStartTime().getMicroseconds())/1000000+tcur->getStartTime().getSeconds();
    double fin=(double)(tcur->getTuioTime().getMicroseconds())/1000000+tcur->getTuioTime().getSeconds();
    double dif=fin-init;
    if(dif<ui->maxDuration->value() && dif>ui->minDuration->value())
    {
        ///FUNCTION THAT GETS GESTURE
        cout << "Send gesture to process" << endl;
        vector<int> pru=gu.getGestureFromCursor(ui->numZones->value(),tcur,ui->minDistance->value());
        cout << "Gesture processed" << endl;
        for(int i=0;i<isTrained.size();i++){
            if(isTrained[i]){
                if(gu.compareGestures(&pru,&(gestures[i]))){
                    cout << "Gesture recognized : " << ((QSpinBox*)gestID[i])->value() << endl;
                    updategestRecogn[i]=true;
                    /// GET GESTURE CENTER OF MASS
                    std::list<TuioPoint> path0=tcur->getPath();
                    Point2f pp(0,0);
                    int counter=0;
                    for(list<TuioPoint>::iterator it=path0.begin();it!=path0.end();it++){
                        pp.x+=(*it).getX();
                        pp.y+=(*it).getY();
                        counter++;
                    }
                    if(counter!=0){
                        pp.x/=counter;
                        pp.y/=counter;
                    }
                    /// IF CALIBRATED CONVERT
                    if(ui->sendCalibrated->isChecked() && calSKT->isCalibrated){
                        calSKT->convertPoint(&pp);
                    }
                    /// UPDATE THE TUIO OBJECT GESTURE
                    //if(((TuioObject*)gestObjs[i])->getSymbolID()!=gestID[i]->value()){
                       /// TODO : FIX FOR CHANGED VALUE :: ERROR !!!
                        //delete gestObjs[i];
                        //gestObjs[i]=new TuioObject(tcur->getTuioTime(),1,gestID[i]->value(),pp.x,pp.y,0.0);
                    //}
                    //else{
                        ((TuioObject*)gestObjs[i])->update(tcur->getTuioTime(),pp.x,pp.y,0.0);
                    //}
                    gestObjsSent[i]=false;
                }
            }
        }
        /// GESTURES
        for(int i=0;i<isTraining.size();i++){
            if(isTraining[i] && pru.size()>0){
                            gestures[i]=pru;
                            gestMats[i].setTo(cv::Scalar::all(0));
                            drawGest(gestMats[i],tcur);
                            updategestMats[i]=true;
                            isTraining[i]=false;
                            trainGest[i]->setText("Train");
                            updateM=true;
                            isTrained[i]=true;
                    }
        }
        /**********************************/
    }

    curToRemove.push_back(tcur);
}

void tuiomultiplexer::refresh(TUIO::TuioTime frameTime)
{
    curToAdd2=curToAdd;
    curToUpdate2=curToUpdate;
    curToRemove2=curToRemove;
    objToAdd2=objToAdd;
    objToUpdate2=objToUpdate;
    objToRemove2=objToRemove;
    //cout << curToAdd2.size() << "," << curToUpdate2.size()<< "," << curToRemove2.size()<< "," <<objToAdd2.size()<< "," << objToUpdate2.size()<< "," << objToRemove2.size() << endl;
    /// MULTIPLEXER
    if(ui->sendCalibrated->isChecked() && calSKT->isCalibrated){
        for (curI = curToAdd2.begin(); curI!=curToAdd2.end(); curI++)
        {
            calSKT->convertTuioCur(*curI);
        }
        for (curI = curToUpdate2.begin(); curI!=curToUpdate2.end(); curI++)
        {
            calSKT->convertTuioCur(*curI);
        }
        for (curI = curToRemove2.begin(); curI!=curToRemove2.end(); curI++)
        {
            //calSKT->convertTuioCur(*curI);
        }
        for (objI = objToAdd2.begin(); objI!=objToAdd2.end(); objI++)
        {
            calSKT->convertTuioObj(*objI);
        }
        for (objI = objToUpdate2.begin(); objI!=objToUpdate2.end(); objI++)
        {
            calSKT->convertTuioObj(*objI);
        }
        for (objI = objToRemove2.begin(); objI!=objToRemove2.end(); objI++)
        {
            //calSKT->convertTuioObj(*objI);
        }
    }
    bool gestureSending=false;
    for(int j=0;j<gestures.size();j++){
        if(!gestObjsSent[j]){
            gestureSending=true;
        }
    }
    for(int i=0;i<NUM_SER;i++){
        if(serverCreated[i] && (curToAdd2.size()>0 || curToUpdate2.size()>0 || curToRemove2.size()>0 || objToAdd2.size()>0 || objToUpdate2.size()>0 || objToRemove2.size()>0)){
            servers[i]->initFrame(frameTime);
            for (curI = curToAdd2.begin(); curI!=curToAdd2.end(); curI++)
            {
                //cout << "Añadio ID : " << (*curI)->getCursorID() << endl;
                servers[i]->addExternalTuioCursor(*curI);
            }
            for (curI = curToUpdate2.begin(); curI!=curToUpdate2.end(); curI++)
            {
                bool found=false;
                list<TuioCursor*> tcrs=servers[i]->getTuioCursors();
                for(list<TuioCursor*>::iterator it = tcrs.begin();it!=tcrs.end();it++){
                    if((*it)->getCursorID()==(*curI)->getCursorID()){
                        servers[i]->updateExternalTuioCursor(*curI);
                        //(*curI)->update(frameTime,0.0f,0.0f);
                        found=true;
                        break;
                    }
                }
                if(!found)
                servers[i]->addExternalTuioCursor(*curI);
            }
            for (curI = curToRemove2.begin(); curI!=curToRemove2.end(); curI++)
            {
                //cout << "Empezo..."<< endl;
                list<TuioCursor*> tcrs=servers[i]->getTuioCursors();
                for(list<TuioCursor*>::iterator it = tcrs.begin();it!=tcrs.end();it++){
                    if((*it)->getCursorID()==(*curI)->getCursorID()){
                        servers[i]->removeExternalTuioCursor((*it));
                        //cout << "REMOVED : " << (*curI)->getCursorID() << endl;
                        break;
                    }
                }
                //cout << "Paso la busqueda" << endl;
            }
            for (objI = objToAdd2.begin(); objI!=objToAdd2.end(); objI++)
            {
                servers[i]->addExternalTuioObject(*objI);
            }
            for (objI = objToUpdate2.begin(); objI!=objToUpdate2.end(); objI++)
            {
                bool found=false;
                list<TuioObject*> tobj=servers[i]->getTuioObjects();
                for(list<TuioObject*>::iterator it = tobj.begin();it!=tobj.end();it++){
                    if((*it)->getSymbolID()==(*objI)->getSymbolID()){
                        servers[i]->updateExternalTuioObject(*objI);
                        found=true;
                        break;
                    }
                }
                if(!found)
                    servers[i]->addExternalTuioObject(*objI);
            }
            for (objI = objToRemove2.begin(); objI!=objToRemove2.end(); objI++)
            {
                list<TuioObject*> tobj=servers[i]->getTuioObjects();
                for(list<TuioObject*>::iterator it = tobj.begin();it!=tobj.end();it++){
                    if((*it)->getSymbolID()==(*objI)->getSymbolID()){
                        servers[i]->removeExternalTuioObject(*it);
                        break;
                    }
                }
            }
            for(int j=0;j<gestures.size();j++){
                if(!gestObjsSent[j]){
                    servers[i]->addExternalTuioObject(gestObjs[j]);
                }
            }
            servers[i]->commitFrame();
        }
    }
    if(gestureSending){
        #ifdef _WIN32 || _WIN64
        Sleep(ui->objectAlive->value()*1000);
        #else
        usleep(ui->objectAlive->value()*1000000);
        #endif
        for(int i=0;i<NUM_SER;i++){
            if(serverCreated[i]){
                servers[i]->initFrame(frameTime+ui->objectAlive->value()*1000000);
                for(int j=0;j<gestures.size();j++){
                    if(!gestObjsSent[j]){
                        servers[i]->removeExternalTuioObject(gestObjs[j]);
                    }
                }
                servers[i]->commitFrame();
            }
        }
        for(int j=0;j<gestures.size();j++){
            if(!gestObjsSent[j]){
                gestObjsSent[j]=true;
            }
        }
    }
    /***************************************************************/
    curToAdd.clear();
    curToUpdate.clear();
    curToRemove.clear();
    objToAdd.clear();
    objToUpdate.clear();
    objToRemove.clear();
}

void tuiomultiplexer::on_isListening_toggled(bool checked)
{
    /// TODO : FIX ERROR IF PORT IS BOUND BY OTHER
    ui->port->setDisabled(checked);
    if(checked && !clientCreated){
        if(clientHasBeenUsed){
            delete client;
        }
        client=new TuioClient(ui->port->value());
        client->addTuioListener(this);
        client->connect();
        clientCreated=true;
        clientHasBeenUsed=true;
    }
    else{
            client->disconnect();
            client->removeAllTuioListeners();
            client->disconnect();
            client->socket->AsynchronousBreak();
        //delete client;
        //client=NULL;
        clientCreated=false;
    }
}

void tuiomultiplexer::activated(int nC)
{
    if(actives[nC]->isChecked()){
    try{
        QByteArray   bytes  = hosts[nC]->text().toAscii();
        const char * host    = bytes.data();
        servers[nC]=new TuioServer(host,ports[nC]->value());
        serverCreated[nC]=true;
        hosts[nC]->setDisabled(true);
        ports[nC]->setDisabled(true);
    }
    catch(std::exception &e){
        cout << "Could not create server : "<< nC+1 << endl;
    }
    }
    else{
        if(servers[nC]!=NULL){
            delete servers[nC];
            serverCreated[nC]=false;
        }
        hosts[nC]->setDisabled(false);
        ports[nC]->setDisabled(false);
    }
    //cout << "ACTIVATED : " << nC << " - "<< actives[nC]->isChecked() << endl;
}

void tuiomultiplexer::on_addGesture_released()
{
    QSpinBox *gID=new QSpinBox();
    QPushButton* gT=new QPushButton("Train");
    glwidget2d* glW=new glwidget2d();
    QCheckBox* gd=new QCheckBox();
    gd->setChecked(false);
    gd->setEnabled(false);
    gd->setFixedWidth(20);
    gd->setFixedHeight(20);
    gestDetected.push_back(gd);
    gestID.push_back(gID);
    trainGest.push_back(gT);
    gestIm.push_back(glW);
    glW->setFixedWidth(50);
    glW->setFixedHeight(50);
    gestMats.push_back(Mat(100,100,CV_8UC1));
    gridGest->addWidget(gID,gestID.size()+1,0,1,1);
    gridGest->addWidget(gT,gestID.size()+1,1,1,1);
    gridGest->addWidget(glW,gestID.size()+1,2,1,1);
    gridGest->addWidget(gd,gestID.size()+1,3,1,1);
    gID->setMinimum(gestID.size()+150-1);
    gID->setMaximum(300);
    gID->setValue(gestID.size()+150);
    signalMapperGest->setMapping(gT, gestID.size()-1);
    connect(gT, SIGNAL(released()), signalMapperGest, SLOT(map()));
    isTraining.push_back(false);
    isTrained.push_back(false);
    updategestMats.push_back(false);
    updategestRecogn.push_back(false);
    updategestRecognMillis.push_back(0);
    vector<int> ge;
    gestures.push_back(ge);
    gestObjs.push_back(new TuioObject(gestID.size()+150,gestID.size()+150,0,0,0));
    gestObjsSent.push_back(true);
    //gID->setEnabled(false);
}

void tuiomultiplexer::startTraining(int num)
{
    if(!isTraining[num]){
    trainGest[num]->setText("Stop");
    isTraining[num]=true;
    }
    else{
        trainGest[num]->setText("Train");
        isTraining[num]=false;
    }
}

void tuiomultiplexer::drawGest(cv::Mat &out, TUIO::TuioCursor *tcur)
{
    double minX=1000000.0;
    double minY=1000000.0;
    double maxX=-1000000.0;
    double maxY=-1000000.0;
    list<TuioPoint> path=tcur->getPath();
    vector<Point2f> pathCV;
    vector<TuioPoint> pathToFix,pathFixed;
    for(list<TuioPoint>::iterator it=path.begin();it!=path.end();it++){
        pathToFix.push_back(*it);
    }
    pathFixed=gu.DouglasPeucker(pathToFix,ui->minDistance->value(),0,pathToFix.size()-1);
    for(vector<TuioPoint>::iterator it=pathFixed.begin();it!=pathFixed.end();it++){
    double x=(*it).getX();
    double y=(*it).getY();
    pathCV.push_back(Point2f(x,y));
    minX=minX<x ? minX:x;
    minY=minY<y ? minY:y;
    maxX=maxX>x ? maxX:x;
    maxY=maxY>y ? maxY:y;
    }

    double scaleX=80.0/(maxX-minX);
    double scaleY=80.0/(maxY-minY);
    double scaleG=scaleX>scaleY ? scaleY:scaleX;
    Point* ge=new Point[pathCV.size()];
    //Point ge[pathCV.size()];
    int counter=0;
    for(vector<Point2f>::iterator it=pathCV.begin();it!=pathCV.end();it++){
        ge[counter].x=(int)(((*it).x-minX) * scaleG)+10;
        ge[counter].y=(int)(((*it).y-minY) * scaleG)+10;
        counter++;
    }

    int *num=new int[1];
    num[0]=pathCV.size()-1;
    const Point* ptt[1] = { ge };
    polylines(out,ptt,num,1,0,cvScalar(255),3);
    delete num;
    delete ge;
}

void tuiomultiplexer::updateMats()
{
    if(updateM){
    for(int i=0;i<updategestMats.size();i++){
        if(updategestMats[i]){
            ((glwidget2d*)gestIm[i])->updateImage(false,gestMats[i]);
            updategestMats[i]=false;
        }
    }
    updateM=false;
    }
    if(kindNew!=4 && pp!=NULL){
        if(pp->doingCalibration && workingPP){
        switch(kindNew){
        case 0:
            pp->onAdd(0,xNew,yNew);
            break;
        case 1:
            pp->onUpdate(0,xNew,yNew);
            break;
        case 2:
            pp->onDelete(0);
            break;
        default:
            break;
        }
        }
        kindNew=4;
    }
    for(int i=0;i<updategestRecogn.size();i++){
        if(updategestRecogn[i]){
            if(updategestRecognMillis[i]==0){
                gestDetected[i]->setChecked(true);
            }
            updategestRecognMillis[i]++;
            if(30*updategestRecognMillis[i]>ui->objectAlive->value()*1000){
                updategestRecognMillis[i]=0;
                updategestRecogn[i]=false;
                gestDetected[i]->setChecked(false);
            }
        }
    }
}

void tuiomultiplexer::on_pointCalStart_released()
{
    if(ui->isListening->isChecked() && !workingPP){
        workingPP=true;
        pp=new pointcalibration(calSKT,NULL,ui->numPointsSpin->value(),ui->distanceFromBorder->value(),ui->framesPointBox->value(),&workingPP);
    }
    else{
        QMessageBox* msg=new QMessageBox(QMessageBox::Critical,"Error","We need to listen some tuio messages... \n \n or maybe you have a calibration running");
        msg->exec();
    }
}

void tuiomultiplexer::on_normCoordinates_toggled(bool checked)
{
    calSKT->useNormalizedCoords=checked;
}


void tuiomultiplexer::on_saveCal_released()
{
    calSKT->saveCalibration();
}

void tuiomultiplexer::on_loadCal_released()
{
    calSKT->loadCalibration();
}
