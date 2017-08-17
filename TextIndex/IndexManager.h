//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_IndexManager_H
#define HASH_0E_IndexManager_H


#include "InvertedIndex.h"
#include "../TemplateIndex/IndexTemplate.h"
#include "../TemplateIndex/ManagerTemplate.h"


class IndexManager: public ManagerTemplate<string, InvertedIndex>
{
public:

	IndexManager(): ManagerTemplate() {}

	explicit IndexManager(int num)
	{
		I0Num = 0;
        Indexes[0]=new InvertedIndex;
		InitialIdf();
//		cout << "Initialization of idf is okay." << endl;
		buildIndex(num);
	}

	void output() override;

	void buildIndex(int audio_sum) override;

	void InitialIdf() override;

	void updateScore(int id,int score) override {
		map<int,InvertedIndex*>::iterator it_Index;
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

	string handleQuery(string query_str);

	string handleQuery(vector<string> query);

    ~IndexManager()
    {
        map<int,InvertedIndex*>::iterator it;
        for (it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
    }

	void freemem() override {
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



//vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);

#endif //HASH_0E_IndexManager_H
