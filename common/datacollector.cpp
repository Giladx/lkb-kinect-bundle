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
#include "datacollector.h"

datacollector::datacollector()
{
    onUpdate=NULL;
    onAdd=NULL;
    onRemove=NULL;
}
datacollector::~datacollector()
{
}
void datacollector::updateData(vector<Point2f> newData,double minDist){
    if(newData.size()>0 && newData.size()<300){
        ///POPULATE DISTANCE MATIX
        for(int i=0;i<newData.size();i++){
            used[i]=false;
        }
        for(mapIt = data.begin(); mapIt != data.end( ); ++mapIt){
            int numB=0;
            for (it = newData.begin(); it!=newData.end(); ++it){
                distances[mapIt->first][numB]=distancePointPoint(Point2f(mapIt->second.x,mapIt->second.y),*it);
                numB++;
            }
        }
        ///* TO AID IN THE SEARCH *///
        map<int,Point2f> aux=data;
        bool buscar=true;
        while(buscar){
            int mI=0;
            Point2f pI(0,0);
            double dist=1000000000;
            bool hayMin=false;
            int numB=0,numU=10000000;
        for (it = newData.begin(); it!=newData.end(); ++it){
            if(!used[numB]){
           for(mapIt = aux.begin(); mapIt != aux.end( ); ++mapIt){
               if(dist>distances[mapIt->first][numB]){
                   dist=distances[mapIt->first][numB];
                   hayMin=true;
                   mI=mapIt->first;
                   pI.x=it->x;pI.y=it->y;
                   numU=numB;
               }
           }
           }
           numB++;
        }
        if(hayMin){
            used[numU]=true;
            ///UPDATE THE VALUE
            /// HERE CHANGE UPDATE ONLY IF DISTANCE IS MORE THAN...
            if(distances[mI][numU]>minDist){
                data[mI]=pI;
            }
            aux.erase(aux.find(mI));
            if(onUpdate==NULL){
            cout << "set cur " << mI << " : " << data[mI].x << "," <<  data[mI].y << endl;
            if(useMouse && mI==0){
                tmw->move(data[mI].x,data[mI].y);
            }
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            else{onUpdate(mI,data[mI].x,data[mI].y);}
            /////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        else{
            buscar=false;
        }
        }
        for(mapIt = aux.begin(); mapIt != aux.end( ); ++mapIt ){
            //REMOVE THOSE NOT USED
            data.erase(data.find(mapIt->first));
            if(onRemove==NULL){
            cout << "del cur " << mapIt->first << endl;
            if(useMouse && mapIt->first==0){
                tmw->release();
            }
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            else{onRemove(mapIt->first);}
            /////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        int numB=0;
        for (it = newData.begin(); it!=newData.end(); ++it){
            if(!used[numB]){
                ///CHECK FOR FREE ID AND ASSIGN THE NEW VALUE:
                for(int u=0;u<300;u++){
                    if(data.count(u)==0){
                        data[u]=Point2f(it->x,it->y);
                        if(onAdd==NULL){
                            cout << "add cur "<< u << " : " << it->x <<"," <<it->y << endl;
                            if(useMouse && u==0){
                                tmw->press(it->x,it->y);
                            }
                        }
                        /////////////////////////////////////////////////////////////////////////////////////////////////////
                        else{
                            onAdd(u,it->x,it->y);
                        }
                        /////////////////////////////////////////////////////////////////////////////////////////////////////
                        break;
                    }
                }
            }
            numB++;
        }
        //cout << "DATA SIZE : " << data.size() << endl;
    }
    else{
        if(data.size()>0){
        for(mapIt = data.begin(); mapIt != data.end( ); ++mapIt ){
            //REMOVE ALL
            if(onRemove==NULL){
            cout << "del cur " << mapIt->first << endl;
            if(useMouse && mapIt->first==0){
                tmw->release();
            }
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            else{onRemove(mapIt->first);}
            /////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        data.clear();
        }
    }
}
double datacollector::distancePointPoint(Point2f p1, Point2f p2){
    return sqrt(pow((p1.x-p2.x),2)+pow((p1.y-p2.y),2));
}
