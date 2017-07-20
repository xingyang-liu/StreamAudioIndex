//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_II_H
#define HASH_0E_II_H


#include "ListInfo.h"



class Ii
{
public:
	int AudioCount, level;
    map<string,ListInfo> *TermIndex;
	map<int, AudioInfo> *InfoTable;
	map<string, CMutex> *TermMutex;//only for I0
	set<int> RemovedId;
	CMutex I0MutexInfo;

	Ii();

	Ii(int l):level(l){AudioCount=0;}

	Ii(const Ii&other)
	{
		AudioCount=other.AudioCount;
		level=other.level;
		TermIndex = new map<string, ListInfo> (*(other.TermIndex));
		InfoTable =new map<int, AudioInfo> (*(other.InfoTable));
		TermMutex = new map<string,CMutex> (*(other.TermMutex));
        RemovedId = other.RemovedId;
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

	void addAudio(AudioInfo &tmp_info,map<string,int> &TagsNum);

	void term_add(string term, int id, int tf);

    void addAudioLive(AudioInfo &tmp_info,map<string,int> &TagsNum,map<int, map<string, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive);

    void term_addLive(string term, int id, int tf, map<int, map<string, NodeInfo*> > &livePointer,int final,CMutex &mutexLive);

    void I0_sort();

	void output();

	int get_count();

	void MergerIndex(Ii &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

	void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name);


	~Ii()
	{
		if(TermIndex!=NULL) delete TermIndex;
        if(InfoTable!=NULL) delete InfoTable;
        if(TermMutex!=NULL) delete TermMutex;
	}


};

class CmpForSig
{
public:
    string term;
    Ii * myself;
    CmpForSig(string str,Ii* me):term(str),myself(me){}

    bool operator()(int a,int b){return (*(myself->InfoTable))[a].score>(*(myself->InfoTable))[b].score;}
};

class CmpForFre
{
public:
    string term;
    Ii * myself;
    CmpForFre(string str,Ii* me):term(str),myself(me){}

    bool operator()(int a,int b){return (*myself->InfoTable)[a].time>(*myself->InfoTable)[b].time;}
};

class CmpForSim
{
public:
    string term;
    Ii * myself;
    CmpForSim(string str,Ii* me):term(str),myself(me){}
	map<int,NodeInfo*> &tmp=(*(*myself->TermIndex)[term].nodeMap);
    bool operator()(int a,int b){return ((*(*myself->TermIndex)[term].nodeMap)[a]->tf)\
        >(*(*myself->TermIndex)[term].nodeMap)[b]->tf;}
};

class FamilyI0Sort
{
public:
    Ii*me;
    vector<int> id_list;
    int flag;//1为fre，2为sig
    map<string,ListInfo>::iterator *it_list;

    FamilyI0Sort(Ii* m,vector<int> l,int f,map<string,ListInfo>::iterator *mit)\
    :me(m),id_list(l),flag(f){it_list=mit;}
};

void *I0SortThread(void *fam);

class ForMirror
{
public:
    CMutex mutex;
    map<int,Ii*>* mirrorIndexMap;
    bool flag;//为了在外面删除方便

    ForMirror(){mirrorIndexMap=NULL;flag=true;}

    ForMirror(map<int,Ii*>* mirror):mirrorIndexMap(mirror),flag(true){}

    ~ForMirror()
    {
        if(mirrorIndexMap!=NULL)
        {
            map<int,Ii*>::iterator it_tmp;
            for (it_tmp=mirrorIndexMap->begin();it_tmp!=mirrorIndexMap->end();it_tmp++)
            {
                delete it_tmp->second;
            }
            delete mirrorIndexMap;
            mirrorIndexMap=NULL;
        }
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

class FamilyMerger
{
public:
    Ii*me;
    Ii*him;

    FamilyMerger(Ii* myself,Ii* other)
    {
        me=myself;
        him=other;
    }
};

void *IiMergerThreadFre(void *fam);


void *IiMergerThreadSig(void *fam);


void *IiMergerThreadTermFreq(void *fam);



//template <class T>
//void *MergerIndexThread(void *fam);


#endif //HASH_0E_II_H
