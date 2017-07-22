

#ifndef HASH_0E_InvertedIndex_H
#define HASH_0E_InvertedIndex_H


#include "ProgramList.h"


class InvertedIndex
{
public:
	int AudioCount, level;
    map<string,ProgramList*> *TermIndex;
	map<int, AudioInfo> *InfoTable;
	map<string, CMutex> *TermMutex;//only for I0
	set<int> RemovedId;
	CMutex I0MutexInfo;
    CMutex mutexRemove;

	InvertedIndex();

	InvertedIndex(int l):level(l){AudioCount=0;}

	InvertedIndex(const InvertedIndex&other)
	{
		AudioCount=other.AudioCount;
		level=other.level;
		TermIndex = new map<string, ProgramList*>;
        map<string,ProgramList*>::iterator it_list;
        map<int,NodeInfo*>::iterator it_node;
        ProgramList* tmp;
        for(it_list=other.TermIndex->begin();it_list!=other.TermIndex->end();it_list++)
        {
            tmp=new ProgramList;
            for(it_node=it_list->second->nodeMap->begin();it_node!=it_list->second->nodeMap->end();it_node++)
            {
                tmp->addNode(it_node->second->tf,it_node->second->id);
            }
            (*TermIndex)[it_list->first]=tmp;
        }
		InfoTable =new map<int, AudioInfo> (*(other.InfoTable));
		TermMutex = new map<string,CMutex> (*(other.TermMutex));
        RemovedId = other.RemovedId;
        I0_sort();
	}

	bool search(int id)
	{
		map<int,AudioInfo>::iterator it_tmp=InfoTable->find(id);
		if(it_tmp!=InfoTable->end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void addAudio(AudioInfo &tmp_info,map<string,int> &TermFreq);

	void node_add(string term, int id, int tf);

    void addAudioLive(AudioInfo &tmp_info,map<string,int> &TermFreq,map<int, map<string, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive);

    void node_addLive(string term, int id, int tf, map<int, map<string, NodeInfo*> > &livePointer,int final,CMutex &mutexLive);

    void I0_sort();

	void output();

	int get_count();

	void MergerIndex(InvertedIndex &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

	void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name);

	double computeScore(const double &time, const double &score, map<string, int> &TermFreq, const int &tagsSum,
						const vector<string> &query);

	~InvertedIndex()
	{
		if(TermIndex!=NULL)
        {
            map<string,ProgramList*> &other=*TermIndex;
            map<string,ProgramList*>::iterator it_list;
            for (it_list=TermIndex->begin();it_list!=TermIndex->end();it_list++)
            {
                delete it_list->second;
            }
            delete TermIndex;
        }
        if(InfoTable!=NULL) delete InfoTable;
        if(TermMutex!=NULL) delete TermMutex;
	}


};

class CmpForSig
{
public:
    string term;
    InvertedIndex * myself;
    CmpForSig(string str,InvertedIndex* me):term(str),myself(me){}

    bool operator()(int a,int b){return (*(myself->InfoTable))[a].score>(*(myself->InfoTable))[b].score;}
};

class CmpForFre
{
public:
    string term;
    InvertedIndex * myself;
    CmpForFre(string str,InvertedIndex* me):term(str),myself(me){}

    bool operator()(int a,int b){return (*myself->InfoTable)[a].time>(*myself->InfoTable)[b].time;}
};

class CmpForSim
{
public:
    string term;
    InvertedIndex * myself;
    CmpForSim(string str,InvertedIndex* me):term(str),myself(me){}
	map<int,NodeInfo*> &tmp=(*(*myself->TermIndex)[term]->nodeMap);
    bool operator()(int a,int b){return ((*(*myself->TermIndex)[term]->nodeMap)[a]->tf)\
        >(*(*myself->TermIndex)[term]->nodeMap)[b]->tf;}
};

class FamilyI0Sort
{
public:
    InvertedIndex*me;
    vector<int> id_list;
    int flag;//1为fre，2为sig
    map<string,ProgramList*>::iterator *it_list;

    FamilyI0Sort(InvertedIndex* m,vector<int> l,int f,map<string,ProgramList*>::iterator *mit)\
    :me(m),id_list(l),flag(f){it_list=mit;}
};

void *I0SortThread(void *fam);

class ForMirror
{
public:
    CMutex mutex;
    map<int,InvertedIndex*>* mirrorIndexMap;
    bool flag;//为了在外面删除方便

    ForMirror(){mirrorIndexMap=NULL;flag=true;}

    ForMirror(map<int,InvertedIndex*>* mirror):mirrorIndexMap(mirror),flag(true){}

    ~ForMirror()
    {
        if(mirrorIndexMap!=NULL)
        {
            map<int,InvertedIndex*>::iterator it_tmp;
            for (it_tmp=mirrorIndexMap->begin();it_tmp!=mirrorIndexMap->end();it_tmp++)
            {
                delete it_tmp->second;
            }
            delete mirrorIndexMap;
            mirrorIndexMap=NULL;
        }
    }
};

class FamilyInvertedIndex
{
public:
	InvertedIndex*me;
	InvertedIndex*him;

	FamilyInvertedIndex(InvertedIndex* myself,InvertedIndex* other)
	{
		me=myself;
		him=other;
	}
};

class FamilyMerger
{
public:
    InvertedIndex*me;
    InvertedIndex*him;

    FamilyMerger(InvertedIndex* myself,InvertedIndex* other)
    {
        me=myself;
        him=other;
    }
};

void *InvertedIndexMergerThreadFre(void *fam);


void *InvertedIndexMergerThreadSig(void *fam);


void *InvertedIndexMergerThreadTermFreq(void *fam);



//template <class T>
//void *MergerIndexThread(void *fam);


#endif //HASH_0E_InvertedIndex_H
