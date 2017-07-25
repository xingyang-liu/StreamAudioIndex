//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_II_H
#define SAMPLE_CONNECTOR_II_H


#include "I0.h"
#include "utils.h"

class Ii
{
public:
	int AudioCount, level, id_tmp;
	map<string, vector<Fre> > *TermIndexFre;
	map<string, vector<Sig> > *TermIndexSig;
	map<string, vector<TermFreq> > *TermIndexSim;
    priority_queue<Sig> updateBuffer;
	map<int, AudioInfo> *InfoTable;
    CMutex I0MutexInfo;//其实我也不知道为了buffer的一个小操作为Indotable写一个互斥量，划不划算
    CMutex bufferMutex;

	Ii();

	Ii(int level0, I0 &Index0);

	void output();

	int get_count();

//	void test();

	void MergerIndex(Ii &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

	void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name);

    void updateScore(int id,int score)
    {
        I0MutexInfo.Lock();
        map<int, AudioInfo>::iterator it_info=InfoTable->find(id);
        if(it_info!=InfoTable->end())
        {
            it_info->second.score=score;
        }
        I0MutexInfo.Unlock();

        bufferMutex.Lock();
        updateBuffer.push(Sig(id,score));
        bufferMutex.Unlock();
    }
//
//	void *MergerIndexSim(void *arg);
//
//	void *MergerIndexSig(void *arg);
//
//	void *MergerIndexFre(void *arg);



	map<string, vector<Fre> > *getPosting(Fre &other)
	{
		return TermIndexFre;
	};

	map<string, vector<Sig> > *getPosting(Sig &other)
	{
		return TermIndexSig;
	};

	map<string, vector<TermFreq> > *getPosting(TermFreq &other)
	{
		return TermIndexSim;
	};

	~Ii()
	{
		if(TermIndexFre!=NULL) delete TermIndexFre;
		if(TermIndexSig!=NULL) delete TermIndexSig;
        if(TermIndexSim!=NULL) delete TermIndexSim;
        if(InfoTable!=NULL) delete InfoTable;
	}
};

class FamilyIi
{
public:
	Ii*me;
	Ii*him;

	FamilyIi(Ii* myself,Ii* other)
	{
		me=myself;
		him=other;
	}
};


template <class T>
void *MergerIndexThread(void *fam);


#endif //SAMPLE_CONNECTOR_II_H
