# Pre-Requisites #

For all the OS's you need OpenNI, NITE and avin2 drivers installed. Below are the links to where you can find them for Linux (use 32-bit versions), Windows (use 32-bit versions) and OSX:

OpenNI (Binaries) and NITE (Compliant Middleware Binaries):

http://www.openni.org/Downloads/OpenNIModules.aspx

avin2 Driver :

https://github.com/avin2/SensorKinect

## Other requirements ##

### Linux ###

You need QT libraries. You can install them with the following command in terminal:

**sudo apt-get install libqt4-core libqt4-gui libqt4-opengl**

If you still get missing libs try a full instalation (with IDE included):

**sudo apt-get install qtcreator**

### Osx ###

You need libusb. Install macports and then libusb before OpenNI, NITE and avin2.

### Windows ###

**NOTE :** Windows SDK for Kinect has drivers that are not compliant with openni. This software is based on OpenNI for camera images retrieval (if we get some requests/support we will make a windows sdk version). You need to uninstall Windows Kinect SDK before installing OpenNI.

After installing openNI you might probably get an "OpenNI.dll not found" error. Copy OpenNI.dll (from the OpenNI dir) and XnVNITE\_1\_5\_2 (from the PrimeSense/NITE dir) in the same dir as LKB.exe or make OpenNI's install directory visible for every app. Google a little on "Enviromental Variable" and OpenNI in windows.

# Running #

Just execute LKB (if you have any Kinect cameras connected it takes a small time to start due to the camera startup).If after some 30 second you dont see any windows try running LKB in console or terminal to see problems you might have had.