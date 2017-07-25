//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_INDEXALL_H
#define SAMPLE_CONNECTOR_INDEXALL_H

#include "utils.h"
#include "I0.h"
#include "Ii.h"
#include "Ilive.h"


class ForMirror
{
public:
	I0 *mirrorI0;
	map<int,Ii*>* mirrorIiMap;

	ForMirror(){mirrorIiMap=NULL;}

	ForMirror(I0* index0):mirrorI0(index0){mirrorIiMap=NULL;}

	~ForMirror()
	{
		if(mirrorI0!=NULL)
        {
            delete mirrorI0;
            mirrorI0=NULL;
        }
		if(mirrorIiMap!=NULL)
		{
			map<int,Ii*>::iterator it_tmp;
			for (it_tmp=mirrorIiMap->begin();it_tmp!=mirrorIiMap->end();it_tmp++)
			{
				delete it_tmp->second;
			}
			delete mirrorIiMap;
			mirrorIiMap=NULL;
		}
	}
};

class IndexAll
{
public:
	int I0Num;
    Ilive IndexLive;
	I0 *Index0;
	map<int, Ii*> otherIndex;
	CMutex mutexI0;//不允许查询访问I0的term表
	CMutex clearIi;//不允许查询进入
    CMutex clearI0;
    CMutex clearMirror;//整个镜像库的互斥量

    vector<ForMirror*> mirrorList;

	IndexAll()
	{
		I0Num = 0;
		Index0= new I0;
	}

//	void addAudio(AudioInfo &audio_info);

	void updateScore(int id,double score);

	void output();

	void get_count();

	vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);

    ~IndexAll()
    {
		if(Index0!=NULL) delete Index0;
        map<int,Ii*>::iterator it;
        for (it=otherIndex.begin();it!=otherIndex.end();it++)
        {
            if(it->second!=NULL) delete ((it->second));
        }
    }

	void freemem()
	{
		if(Index0!=NULL) delete Index0;
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
