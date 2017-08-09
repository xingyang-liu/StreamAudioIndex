//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_IndexManager_H
#define HASH_0E_IndexManager_H


#include "InvertedIndex.h"

#include "../TemplateIndex/IndexTemplate.h"



class IndexManager
{
public:
	int I0Num,I0TermNum;
	long TotalTermSum;
    vector<ForMirror<InvertedIndex>*> mirrorList;
	map<int, InvertedIndex*> Indexes;
    map<int, map<string, NodeInfo *> > livePointer;
    map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
	CMutex clearInvertedIndex;//正在给map赋新值，无关I0
    CMutex clearI0;//正在给I0赋值
	CMutex clearMirror;

	IndexManager(){
		I0Num = 0;
        I0TermNum=0;
		TotalTermSum=0;
		Indexes[0]=new InvertedIndex;
	}

	IndexManager(int num)
	{
		I0Num = 0;
        I0TermNum=0;
		TotalTermSum=0;
        Indexes[0]=new InvertedIndex;
		InitialIdf();
		cout << "Initialization of idf is okay." << endl;
		buildIndex(num);
	}

	void output();

	void buildIndex(int audio_sum);

	void InitialIdf();

	void updateScore(int id,int score);

	string handleQuery(string query_str);

    ~IndexManager()
    {
        map<int,InvertedIndex*>::iterator it;
        for (it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
    }

	void freemem()
	{
		map<int,InvertedIndex*>::iterator it;
		for (it=Indexes.begin();it!=Indexes.end();it++)
		{
			delete((it->second));
		}
		Indexes.clear();
	}
};

class FamilyQuery
{
public:
	IndexManager *me;
	vector<string> * que;
	map<int, string> * na;
	vector<pair<int, double> > *ResVec;

	FamilyQuery(IndexManager* m,vector<string> * q,map<int, string> * n, vector<pair<int, double> > *R)
	{
		me=m;
		que=q;
		na=n;
		ResVec=R;
	}

};

void *searchThread(void *family);

class FamilyAll
{
public:
    IndexManager *me;
    AudioInfo *audio;
    map<string,double> *tags;

    FamilyAll(IndexManager*myself,AudioInfo *audio_info,map<string,double> *t)
    {
        me=myself;
        audio=audio_info;
        tags=t;
    }
};

void *addAudioALLThread(void *Family);//如果要实现多线程，就必须管控所有add与merger

#endif //HASH_0E_IndexManager_H
