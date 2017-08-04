//
// Created by billy on 17-7-30.
//

#ifndef HASH_0E_PHOINDEXMANAGER_H
#define HASH_0E_PHOINDEXMANAGER_H

#include "PhonomeIndex.h"

class PhoIndexManager {
public:
    int I0Num;
    map<Phonome, double> idfTable;
    vector<ForMirror<PhonomeIndex>*> mirrorList;
    map<int, PhonomeIndex*> Indexes;
    map<int, map<Phonome, NodeInfo *> > livePointer;
    map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
    CMutex clearInvertedIndex;//正在给map赋新值，无关I0
    CMutex clearI0;//正在给I0赋值
    CMutex clearMirror;

    PhoIndexManager(){
        I0Num = 0;
        Indexes[0]=new PhonomeIndex;
    }

    PhoIndexManager(int num)
    {
        I0Num = 0;
        Indexes[0]=new PhonomeIndex;
        InitialIdf();
//        cout << "Initialization of idf is okay." << endl;
        buildIndex(num);
    }

    void output();

    void buildIndex(int audio_sum);

    void InitialIdf();

    void updateScore(int id,int score)
    {
        map<int,PhonomeIndex*>::iterator it_Index;
        for(it_Index=Indexes.begin();it_Index!=Indexes.end();it_Index++)
        {
            if(it_Index->second->level!=0)
            {
                if(it_Index->second->search(id))
                {
                    it_Index->second->update(id,score);
                }
            }
            else
            {
                if(it_Index->second->search(id))
                {
                    it_Index->second->I0MutexInfo.Lock();
                    (*it_Index->second->InfoTable)[id].score=score;
                    it_Index->second->I0MutexInfo.Unlock();
                }
            }
        }
    }

    string handleQuery(vector<Phonome> query_str);

    ~PhoIndexManager()
    {
        map<int,PhonomeIndex*>::iterator it;
        for (it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
    }

    void freemem()
    {
        map<int,PhonomeIndex*>::iterator it;
        for (it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
        Indexes.clear();
    }
};

class FamilyPhoQuery
{
public:
    PhoIndexManager *me;
    vector<Phonome> * que;
    map<int, string> * na;
    vector<pair<int, double> > *ResVec;

    FamilyPhoQuery(PhoIndexManager* m,vector<Phonome> * q,map<int, string> * n, vector<pair<int, double> > *R)
    {
        me=m;
        que=q;
        na=n;
        ResVec=R;
    }
};

void *searchPhoThread(void *family);

class FamilyPho
{
public:
    PhoIndexManager *me;
    AudioInfo *audio;
    map<Phonome,double> *tags;

    FamilyPho(PhoIndexManager*myself,AudioInfo *audio_info,map<Phonome,double> *t)
    {
        me=myself;
        audio=audio_info;
        tags=t;
    }
};

void *addAudioPhoThread(void *Family);//如果要实现多线程，就必须管控所有add与merger

struct count_node {
//    int overall = 0;
    int current = 0;
    int filesNum = 0;
};

void initialInfo(string path);

#endif //HASH_0E_PHOINDEXMANAGER_H
