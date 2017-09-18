//
// Created by billy on 17-7-30.
//

#ifndef HASH_0E_PHOINDEXMANAGER_H
#define HASH_0E_PHOINDEXMANAGER_H

#include "../TextIndex/InvertedIndex.h"

using namespace std;

class PhoIndexManager{
public:
    int I0Num,I0TermNum,AudioSum;
    long totalTermSum;
    vector<ForMirror<InvertedIndex>*> mirrorList;
    btree::btree_map<int, InvertedIndex*> Indexes;
    btree::btree_map<int, btree::btree_map<int, NodeInfo *> > livePointer;
    btree::btree_map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
    CMutex clearInvertedIndex;//正在给map赋新值，无关I0
    btree::btree_map<SimilarPhoneme,IdfCode> IdfTable;
    int IdfCodeNum;

    PhoIndexManager() {
        I0Num = 0;
        I0TermNum=0;
        AudioSum=0;
        totalTermSum=0;
        IdfCodeNum=0;
        Indexes[0]=new InvertedIndex;
    }

    explicit PhoIndexManager(int num) {
        I0Num = 0;
        I0TermNum=0;
        AudioSum=0;
        totalTermSum=0;
        IdfCodeNum=0;
        Indexes[0]=new InvertedIndex;
        InitialIdf();
//        cout << "Initialization of idf is okay." << endl;
        buildIndex(num);
    }

    void output();

    void buildIndex(int audio_sum);

    void InitialIdf();

    void updateScore(int id,int score) {
        for(auto it_Index=Indexes.begin();it_Index!=Indexes.end();it_Index++)
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

    void addAudio(AudioInfo &audio, vector<SimilarPhoneme> &content);

    void handleQuery(vector<SimilarPhoneme> phones, vector<string> Result);

    void handleQuery(vector<int> queryCode, vector<double> queryIdf, vector<string> Result);

    ~PhoIndexManager()
    {
        for (auto it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
    }

    void freemem() {
        for (auto it=Indexes.begin();it!=Indexes.end();it++)
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
    vector<int> * que_code;
    vector<double> *que_idf;
    btree::btree_map<int, string> * na;
    vector<pair<int, double> > *resVec;
    btree::btree_map<int,score_ratio> *sco_ve;

    FamilyPhoQuery(PhoIndexManager* m,vector<int> * q_c,vector<double> *q_i,btree::btree_map<int, string> * n, vector<pair<int, double> > *r,\
		btree::btree_map<int,score_ratio> *sc)
    {
        me=m;
        que_code=q_c;
        que_idf=q_i;
        na=n;
        resVec=r;
        sco_ve=sc;
    }
};

void *searchPhoThread(void *family);

class FamilyPho
{
public:
    PhoIndexManager *me;
    AudioInfo *audio;
    btree::btree_map<int,double> *tags;
    int termCount;

    FamilyPho(PhoIndexManager*myself,AudioInfo *audio_info,btree::btree_map<int,double> *t, int tCount)
    {
        me=myself;
        audio=audio_info;
        tags=t;
        termCount = tCount;
    }
};

void *addAudioPhoThread(void *Family);//如果要实现多线程，就必须管控所有add与merger

struct count_node {
    int overall = 0;
    int current = 0;
    int filesNum = 0;
};

void initialInfo(string path);

#endif //HASH_0E_PHOINDEXMANAGER_H
