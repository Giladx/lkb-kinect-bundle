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
#ifndef TUIOMOUSEWRAPPER_H
#define TUIOMOUSEWRAPPER_H
//#include <jni.h>
//#include <cstdlib>
//#define PATH_SEPARATOR ':' /* define it to be ':' on Solaris */
//#define USER_CLASSPATH "libTUIO.jar:." /* where Prog.class is */
//#define USER_CLASSPATH "." /* where Prog.class is */
//#include <iostream>

using namespace std;

class tuiomousewrapper
{
public:
    tuiomousewrapper();
    ~tuiomousewrapper();
    //void destroyer();
    //JNIEnv *env;
    //JavaVM *jvm;
    void press(int x,int y);
    void release();
    void move(int x, int y);
    bool created;
    //jclass cls;
    //jmethodID mid,pre,rel,mov;*/
};

#endif // TUIOMOUSEWRAPPER_H
