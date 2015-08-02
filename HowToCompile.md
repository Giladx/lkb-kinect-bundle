# Requisites #

## General ##

For the following libs we used the 32-bit versions in Windows and Ubuntu.

  * OpenNI

  * NITE

  * Avin2 Driver

  * Qt SDK (4.8) with Qt Creator

## Platform Dependent ##

### Windows ###

  * Visual Studio 2010 32 bit (for the C++ Compiler, maybe works with Visual Express C++ Compiler)

### OSX ###

  * XCode (for the compilation tools)

# How to compile #

  * Open LKB.pro inside the source folder in Qt Creator

  * Choose not to load settings

  * Select your build folder (must be on the same level as the source folder)

  * Change the Includes and Libraries directories in the .pro file (on the zone dependent to your platform) to point to your installed directories of OpenNI, Nite and avin2.

  * Compile!