//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_INDEXALL_H
#define HASH_0E_INDEXALL_H


#include "Ii.h"



class IndexAll
{
public:
	int I0Num;
    vector<ForMirror*> mirrorList;
	map<int, Ii*> otherIndex;
    map<int, map<string, NodeInfo *> > livePointer;
    map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
	CMutex clearIi;//正在给map赋新值，无关I0
    CMutex clearI0;//正在给I0赋值

	IndexAll()
	{
		I0Num = 0;
        otherIndex[0]=new Ii;
	}

	void output();

    ~IndexAll()
    {
        map<int,Ii*>::iterator it;
        for (it=otherIndex.begin();it!=otherIndex.end();it++)
        {
            delete((it->second));
        }
    }

	void freemem()
	{
		map<int,Ii*>::iterator it;
		for (it=otherIndex.begin();it!=otherIndex.end();it++)
		{
			delete((it->second));
		}
		otherIndex.clear();
	}
};

class FamilyQuery
{
public:
	IndexAll *me;
	vector<string> * que;
	map<int, string> * na;
	vector<pair<int, double> > *ResVec;

	FamilyQuery(IndexAll* m,vector<string> * q,map<int, string> * n, vector<pair<int, double> > *R)
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
    IndexAll *me;
    AudioInfo *audio;
    map<string,int> *tags;

    FamilyAll(IndexAll*myself,AudioInfo *audio_info,map<string,int> *t)
    {
        me=myself;
        audio=audio_info;
        tags=t;
    }
};

void *addAudioALLThread(void *Family);//如果要实现多线程，就必须管控所有add与merger



//vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);

#endif //HASH_0E_INDEXALL_H
