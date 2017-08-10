//
// Created by billy on 17-8-8.
//

#ifndef HASH_0E_MANAGERTEMPLATE_H
#define HASH_0E_MANAGERTEMPLATE_H

#include <iostream>
#include <map>
#include <vector>
#include "IndexTemplate.h"

using namespace std;

template <class T, class TIndex>
class ManagerTemplate {
public:
    int I0Num;
    vector<ForMirror<TIndex>*> mirrorList;
    map<int, TIndex*> Indexes;
    map<int, map<T, NodeInfo *> > livePointer;
    map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
    CMutex clearInvertedIndex;//正在给map赋新值，无关I0
    CMutex clearI0;//正在给I0赋值
    CMutex clearMirror;

    ManagerTemplate() {
        I0Num = 0;
        Indexes[0]=new TIndex;
    }

    virtual void output()=0;

    virtual void buildIndex(int audio_sum)=0;

    virtual void InitialIdf()=0;

    virtual void updateScore(int id,int score)=0;

    virtual string handleQuery(vector<T> query_str)=0;

    virtual void freemem()=0;
};


#endif //HASH_0E_MANAGERTEMPLATE_H
