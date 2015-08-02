/*
tuiomultiplexer.h - LTmgc (Ludique's TUIO Multiplexer, Gestures and Screen Calibrator)

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
#ifndef TUIOMULTIPLEXER_H
#define TUIOMULTIPLEXER_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <TuioServer.h>
#include <TuioClient.h>
#include <QSpinBox>
#include <iostream>
#include <QSignalMapper>
#include <gestureutils.h>
#include <glwidget2d.h>
#include <QTimer>
#include <pointcalibration.h>
#include <calibrationskt.h>
#include <QMessageBox>

#ifdef _WIN32 || _WIN64
#include <windows.h>
#else
#include <unistd.h>
#endif
#define NUM_SER 10
#define NUM_REGS 12

using namespace TUIO;
using namespace std;
namespace Ui {
    class tuiomultiplexer;
}

class tuiomultiplexer : public QDialog,TuioListener
{
    Q_OBJECT

public:
    explicit tuiomultiplexer(QWidget *parent = 0);
    ~tuiomultiplexer();
    QSpinBox *ports[NUM_SER];
    QLineEdit *hosts[NUM_SER];
    QLabel *labels[NUM_SER];
    QCheckBox *actives[NUM_SER];
    QGridLayout *grid;
    QGridLayout *gridGest;
    vector<QSpinBox*> gestID;
    vector<QPushButton*> trainGest;
    vector<glwidget2d*> gestIm;
    vector<Mat> gestMats;
    QLabel *titulos[6];
    TuioTime time;
    TuioServer* servers[NUM_SER];
    TuioClient* client;
    vector<TuioObject* > gestObjs;
    vector<bool> gestObjsSent;
    calibrationskt* calSKT;
    double xNew,yNew;
    int kindNew;

    void addTuioObject(TuioObject *tobj);
    void updateTuioObject(TuioObject *tobj);
    void removeTuioObject(TuioObject *tobj);

    void addTuioCursor(TuioCursor *tcur);
    void updateTuioCursor(TuioCursor *tcur);
    void removeTuioCursor(TuioCursor *tcur);

    void refresh(TuioTime frameTime);
    bool clientCreated;
    bool clientHasBeenUsed;
    bool serverCreated[NUM_SER];

    vector<TuioCursor*> curToAdd;
    vector<TuioCursor*> curToUpdate;
    vector<TuioCursor*> curToRemove;
    vector<TuioObject*> objToAdd;
    vector<TuioObject*> objToUpdate;
    vector<TuioObject*> objToRemove;
    vector<TuioCursor*> curToAdd2;
    vector<TuioCursor*> curToUpdate2;
    vector<TuioCursor*> curToRemove2;
    vector<TuioObject*> objToAdd2;
    vector<TuioObject*> objToUpdate2;
    vector<TuioObject*> objToRemove2;
    vector<TuioCursor*>::iterator curI;
    vector<TuioObject*>::iterator objI;
    vector<vector <int> > gestures;

    QSignalMapper *signalMapper;
    QSignalMapper *signalMapperGest;
    vector<bool> isTraining;
    vector<bool> updategestMats;
    vector<bool> updategestRecogn;
    vector<int> updategestRecognMillis;
    vector<bool> isTrained;
    vector<QCheckBox*> gestDetected;
    gestureutils gu;
    QTimer *timer;
    bool updateM;
    pointcalibration* pp;
    bool workingPP;
    bool areWeCalibrating;

    void drawGest(Mat& out,TuioCursor* tcur);

private slots:
    void on_isListening_toggled(bool checked);
    void activated(int nC);
    void startTraining(int num);
    void on_addGesture_released();
    void updateMats();

    void on_pointCalStart_released();

    void on_normCoordinates_toggled(bool checked);

    void on_saveCal_released();

    void on_loadCal_released();

private:
    Ui::tuiomultiplexer *ui;
};

#endif // TUIOMULTIPLEXER_H

