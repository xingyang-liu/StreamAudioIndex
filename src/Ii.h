//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_II_H
#define SAMPLE_CONNECTOR_II_H


#include "I0.h"
#include "Sig.h"
#include "utils.h"

class Ii
{
public:
	int AudioCount, level, id_tmp;
	map<string, vector<int> > *TermIndexFre;
	map<string, vector<int> > *TermIndexSig;
	map<string, vector<int> > *TermIndexSim;
    priority_queue<Sig> updateBuffer;
	map<int, AudioInfo> *InfoTable;
    CMutex I0MutexInfo;//其实我也不知道为了buffer的一个小操作为Indotable写一个互斥量，划不划算
    CMutex bufferMutex;
	CMutex termIndexMutex;//用于指示开始查询，不允许修改

	Ii();

	Ii(int level0, I0 &Index0);

	void output();

	int get_count();

    void Ii_sort();

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



	map<string, vector<int> > *getPosting(int identifier)
	{
        if (identifier==1)
        {
            return TermIndexFre;
        }
        else if(identifier==2)
        {
            return TermIndexSig;
        }
        else if(identifier==3)
        {
            return TermIndexSim;
        }

	};



	~Ii()
	{
		if(TermIndexFre!=NULL)
        {
            map<string,vector<int>>::iterator it_fre;
            for (it_fre=TermIndexFre->begin();it_fre!=TermIndexFre->end();it_fre++)
            {
                it_fre->second.clear();
            }
            TermIndexFre->clear();
            delete TermIndexFre;
            TermIndexFre=NULL;
        }
		if(TermIndexSig!=NULL)
        {
            delete TermIndexSig;
            TermIndexSig=NULL;
        }
        if(TermIndexSim!=NULL)
        {
            delete TermIndexSim;
            TermIndexSim=NULL;
        }
        if(InfoTable!=NULL)
        {
            delete InfoTable;
            InfoTable=NULL;
        }
	}
};

class FamilyIi
{
public:
	Ii*me;
	Ii*him;
    int identi;

	FamilyIi(Ii* myself,Ii* other,int iden)
	{
		me=myself;
		him=other;
        identi=iden;
	}
};


template <class T>
void *MergerIndexThread(void *fam);


#endif //SAMPLE_CONNECTOR_II_H
