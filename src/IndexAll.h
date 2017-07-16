//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_INDEXALL_H
#define SAMPLE_CONNECTOR_INDEXALL_H

#include "utils.h"
#include "I0.h"
#include "Ii.h"
#include "Ilive.h"


class IndexAll
{
public:
	int I0Num;
	double maxTime;
    Ilive IndexLive;
	I0 Index0;
	map<int, Ii*> otherIndex;
	CMutex mutexI0;//不允许查询访问I0的term表
	CMutex clearIi;//不允许查询进入
    CMutex clearI0;

	IndexAll()
	{
		I0Num = 0;
	}

//	void addAudio(AudioInfo &audio_info);

	void output();

	void get_count();

	vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);

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

    FamilyAll(IndexAll*myself,AudioInfo *audio_info)
    {
        me=myself;
        audio=audio_info;
    }
};

void *addAudioALLThread(void *Family);//如果要实现多线程，就必须管控所有add与merger



//vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);

#endif //SAMPLE_CONNECTOR_INDEXALL_H
