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
#include "tuiomousewrapper.h"

tuiomousewrapper::tuiomousewrapper()
{
         /*jint pX,pY;
         jintArray pts;
         jclass integerClass;
         jobjectArray args;*/
         created=false;

         /*JavaVMInitArgs vm_args;
         JavaVMOption options[1];
         options[0].optionString ="-Djava.class.path=" USER_CLASSPATH;
         vm_args.version = 0x00010002;
         vm_args.options = options;
         vm_args.nOptions = 1;
         vm_args.ignoreUnrecognized = JNI_TRUE;

         pX = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);

         cls = env->FindClass("TuioMouse");
         mid = env->GetStaticMethodID(cls, "main",
                                         "([Ljava/lang/String;)V");
         pre = env->GetStaticMethodID(cls, "press",
                                         "(II)V");
         rel = env->GetStaticMethodID(cls, "release",
                                         "()V");
         mov = env->GetStaticMethodID(cls, "move",
                                         "(II)V");
         if (mid == NULL || pre==NULL || rel==NULL || mov==NULL) {
             destroyer();
         }
         else{
             jclass stringClass = env->FindClass( "java/lang/String");
             jstring jstr;
             jstr = env->NewStringUTF( " from CPP!");
             jobjectArray args = env->NewObjectArray( 1, stringClass, jstr);
             env->CallStaticVoidMethod( cls, mid,args);
         created=true;
         cout << "Estamos!!" << endl;
         }*/
}
tuiomousewrapper::~tuiomousewrapper(){
    if(created){
        //destroyer();
    }
}
/*void tuiomousewrapper::destroyer(){
    if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
    }
    jvm->DestroyJavaVM();
    created=false;
}*/
void tuiomousewrapper::move(int x, int y){
    //env->CallStaticVoidMethod(cls,mov,x,y);
    //env->CallVoidMethod(cls,mov,x,y);
}
void tuiomousewrapper::press(int x, int y){
    //env->CallStaticVoidMethod(cls,pre,x,y);
    //env->CallVoidMethod(cls,pre,x,y);
}
void tuiomousewrapper::release(){
    //env->CallStaticVoidMethod(cls,rel);
    //env->CallVoidMethod(cls,rel);
}
