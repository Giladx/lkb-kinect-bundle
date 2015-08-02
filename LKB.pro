#-------------------------------------------------
#
# LKB qmake
#
#-------------------------------------------------
QT       += core gui opengl

TARGET = LKB
TEMPLATE = app

INCLUDEPATH += includes/TUIO
INCLUDEPATH += includes/oscpack
INCLUDEPATH += common touch skeleton multiviewer scenerecorder

win32:{ LIBS += ws2_32.lib winmm.lib -l$$PWD/libs/opencv_core233 -l$$PWD/libs/opencv_imgproc233 -l$$PWD/libs/opencv_features2d233 -l$$PWD/libs/opencv_calib3d233 -l$$PWD/libs/opencv_flann233 -fopenmp
        LIBS += -L"C:\Program Files (x86)\PrimeSense\NITE\Lib" -L"C:\Program Files (x86)\OpenNI\Lib" -lopenNI -lXnVNite_1_5_2
        INCLUDEPATH += includes/OpenCV/Win/opencv
        INCLUDEPATH += includes/OpenCV/Win
        INCLUDEPATH += "C:\Program Files (x86)\OpenNI\Include" "C:\Program Files (x86)\PrimeSense\NITE\Include"
        QMAKE_CXXFLAGS += "/openmp"
        RC_FILE += $$PWD/winicon.rc
}
unix:{
    !macx{
        INCLUDEPATH += /usr/include/ni
        INCLUDEPATH += /usr/include/ni/nite
        INCLUDEPATH += includes/OpenCV/Linux/opencv
        INCLUDEPATH += includes/OpenCV/Linux/
        LIBS += -L$$PWD/libs -lopencv_core -lopencv_imgproc -lopencv_core
        LIBS += -lopencv_calib3d -lopencv_imgproc -lopencv_core -lopencv_features2d
        LIBS += -lopencv_imgproc -lopencv_core -lopencv_flann -lOpenNI -fopenmp -lXnVNite_1_5_2
    }
    else{
        INCLUDEPATH += /usr/include/ni
        INCLUDEPATH += /usr/include/ni/nite
        INCLUDEPATH += includes/OpenCV/Mac/opencv
        INCLUDEPATH += includes/OpenCV/Mac/
        LIBS += -L$$PWD/libs/osx -lopencv_core -lopencv_imgproc -lopencv_core -lopencv_calib3d -lopencv_imgproc -lopencv_core -lopencv_features2d -lopencv_imgproc -lopencv_core -lopencv_flann
        LIBS += -fopenmp -lXnVNite_1_5_2 -lOpenNI -lz
        ICON = qtcreator.icns
        QMAKE_INFO_PLIST = Info.plist
    }
    QMAKE_CXXFLAGS += -fopenmp
}

OTHER_FILES += qtcreator.rc Info.plist \
    winicon.rc \
    isotipoLudique.png \
    isotipo.png \
    isotipo.ico \
    isotipo.icns


SOURCES += main.cpp\
        mainwindow.cpp \
    includes/TUIO/TuioTime.cpp \
    includes/TUIO/TuioServer.cpp \
    includes/TUIO/TuioClient.cpp \
    includes/oscpack/osc/OscTypes.cpp \
    includes/oscpack/osc/OscReceivedElements.cpp \
    includes/oscpack/osc/OscPrintReceivedElements.cpp \
    includes/oscpack/osc/OscOutboundPacketStream.cpp \
    includes/oscpack/ip/IpEndpointName.cpp \
    common/utilidades.cpp \
    common/tuiomultiplexer.cpp \
    common/tuiomousewrapper.cpp \
    common/tuiodatasender.cpp \
    common/surfacecamcalibration.cpp \
    common/ransacfitcam.cpp \
    common/ransacfit.cpp \
    common/ransacdialog.cpp \
    common/pointcalibration.cpp \
    common/instructions.cpp \
    common/glwidget2d.cpp \
    common/glwidget.cpp \
    common/gestureutils.cpp \
    common/datacollector.cpp \
    common/cameras.cpp \
    common/calibrationskt.cpp \
    skeleton/testusuario.cpp \
    skeleton/skeletonutilities.cpp \
    touch/virtualplane.cpp \
    touch/testblobsmixto.cpp \
    touch/sktprocessor.cpp \
    touch/sktinfoextractor.cpp \
    touch/camerasettings2d.cpp \
    touch/camerasettings.cpp \
    touch/camconfig.cpp \
    touch/backmethod.cpp \
    common/camcalibration.cpp \
    multiviewer/multi3dviewer.cpp \
    common/glwidgetcloud.cpp \
    aboutdialog.cpp \
    common/qtimagewidget.cpp

win32:SOURCES+=includes/oscpack/ip/win32/UdpSocket.cpp \
    includes/oscpack/ip/win32/NetworkingUtils.cpp
unix:SOURCES+=includes/oscpack/ip/posix/UdpSocket.cpp \
    includes/oscpack/ip/posix/NetworkingUtils.cpp


HEADERS  += mainwindow.h \
    includes/TUIO/TuioTime.h \
    includes/TUIO/TuioServer.h \
    includes/TUIO/TuioPoint.h \
    includes/TUIO/TuioObject.h \
    includes/TUIO/TuioListener.h \
    includes/TUIO/TuioCursor.h \
    includes/TUIO/TuioContainer.h \
    includes/TUIO/TuioClient.h \
    includes/oscpack/osc/OscTypes.h \
    includes/oscpack/osc/OscReceivedElements.h \
    includes/oscpack/osc/OscPrintReceivedElements.h \
    includes/oscpack/osc/OscPacketListener.h \
    includes/oscpack/osc/OscOutboundPacketStream.h \
    includes/oscpack/osc/OscHostEndianness.h \
    includes/oscpack/osc/OscException.h \
    includes/oscpack/osc/MessageMappingOscPacketListener.h \
    includes/oscpack/ip/UdpSocket.h \
    includes/oscpack/ip/TimerListener.h \
    includes/oscpack/ip/PacketListener.h \
    includes/oscpack/ip/NetworkingUtils.h \
    includes/oscpack/ip/IpEndpointName.h \
    common/utilidades.h \
    common/tuiomultiplexer.h \
    common/tuiomousewrapper.h \
    common/tuiodatasender.h \
    common/surfacecamcalibration.h \
    common/ransacfitcam.h \
    common/ransacfit.h \
    common/ransacdialog.h \
    common/pointcalibration.h \
    common/instructions.h \
    common/glwidget2d.h \
    common/glwidget.h \
    common/gestureutils.h \
    common/datacollector.h \
    common/cameras.h \
    common/calibrationskt.h \
    skeleton/testusuario.h \
    skeleton/skeletonutilities.h \
    touch/virtualplane.h \
    touch/testblobsmixto.h \
    touch/sktprocessor.h \
    touch/sktinfoextractor.h \
    touch/camerasettings2d.h \
    touch/camerasettings.h \
    touch/camconfig.h \
    touch/backmethod.h \
    common/camcalibration.h \
    multiviewer/multi3dviewer.h \
    common/glwidgetcloud.h \
    aboutdialog.h \
    common/qtimagewidget.h

FORMS    += forms/mainwindow.ui \
    forms/camerasettings.ui \
    forms/camerasettings2d.ui \
    forms/ransacdialog.ui \
    forms/surfacecamcalibration.ui \
    forms/testblobsmixto.ui \
    forms/sktinfoextractor.ui \
    forms/pointcalibration.ui \
    forms/instructions.ui \
    forms/testusuario.ui \
    forms/tuiomultiplexer.ui \
    forms/camcalibration.ui \
    forms/multi3dviewer.ui \
    aboutdialog.ui

RESOURCES += \
    recursos.qrc

OTHER_FILES += \
    License Info/LKB-License \
    License Info/OpenCV/readme.txt \
    License Info/OpenCV/license.txt \
    License Info/oscpack/license \
    License Info/QT/NQSThirdPartyAgreement.txt \
    License Info/QT/NQSDevAgreement.txt \
    License Info/QT/Notifications_API_terms_and_conditions.txt \
    License Info/QT/LocationTerms.txt \
    License Info/QT/LICENSE.LGPL \
    License Info/QT/LICENSE.GPL3 \
    License Info/QT/LICENSE.FDL \
    License Info/QT/LGPL_EXCEPTION.txt \
    License Info/TUIO/LICENSE.txt \
    License Info/LKB-License
