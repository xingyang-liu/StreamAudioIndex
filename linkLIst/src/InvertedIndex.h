

#ifndef HASH_0E_InvertedIndex_H
#define HASH_0E_InvertedIndex_H


#include "ProgramList.h"
#include "Sig.h"

class InvertedIndex
{
public:
	int AudioCount, level;
    map<string,ProgramList*> *TermIndex;
	map<int, AudioInfo> *InfoTable;
	map<string, CMutex> *TermMutex;//only for I0貌似用处不大
	set<int> RemovedId;
	CMutex I0MutexInfo;//I0的info正在被修改
    CMutex mutexRemove;//removeId正在被修改
	CMutex termIndexMutex;//大致就是我进入这里查询的时候，TermIndex不能改变
    priority_queue<Sig> updateBuffer;
    CMutex bufferMutex;//由于I0不需要重新排序，所以它没有这个

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
        if(InfoTable==NULL)
        {
            return false;
        }
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

	void addAudio(AudioInfo &tmp_info,map<string,double> &TermFreq);

	void node_add(string term, int id, double tf);
	
	void update(int id,int score)
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

    void addAudioLive(AudioInfo &tmp_info,map<string,double> &TermFreq,map<int, map<string, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive);

    void node_addLive(string term, int id, double tf, map<int, map<string, NodeInfo*> > &livePointer,int final,CMutex &mutexLive);

    void I0_sort();

	void output();

	int get_count();

	void MergerIndex(InvertedIndex &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

	void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name);

	double computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
						const vector<string> &query);
    
    void insert_and_remove()//在merger开始之前，buffer中所有的项
    {
        NodeInfo* tmp_node;
        map<int,double> tmp_buffer;
        for(int i=0;i<updateBuffer.size();i++)
        {
            tmp_buffer[updateBuffer.top().id]=updateBuffer.top().score;
            updateBuffer.pop();
        }
        map<int,double>::iterator it_buffer;
        map<int,NodeInfo*>::iterator it_node;
        map<string,ProgramList*>::iterator it_list_i;
        for(it_list_i=TermIndex->begin();it_list_i!=TermIndex->end();it_list_i++)
        {
            for(it_buffer=tmp_buffer.begin();it_buffer!=tmp_buffer.end();it_buffer++)
            {
                if(it_list_i->second->nodeMap->find(it_buffer->first)!=it_list_i->second->nodeMap->end())
                {
                    tmp_node=it_list_i->second->max_fresh;
                    if(it_list_i->second->max_fresh->next_fresh!=NULL)//如果只有一个node就没有任何操作
                    {
                        if(it_list_i->second->max_fresh->id==it_buffer->first)//如果max_fresh正好是要改的值，那就先把它从链里去掉
                        {
                            it_list_i->second->max_fresh=it_list_i->second->max_fresh->next_fresh;
                        }
                        else{//也是从链中挖去这个点
                            while(tmp_node->next_fresh->next_fresh!=NULL&&tmp_node->next_fresh->id!=it_buffer->first)
                            {
                                tmp_node=tmp_node->next_fresh;
                            }
                            tmp_node->next_fresh=tmp_node->next_fresh->next_fresh;
                        }

                        //准备重新放进去

                        if(it_buffer->second>=(*InfoTable)[it_list_i->second->max_fresh->id].score)
                        {
                            (*(it_list_i->second->nodeMap))[it_buffer->first]->next_fresh=it_list_i->second->max_fresh;
                            it_list_i->second->max_fresh=(*(it_list_i->second->nodeMap))[it_buffer->first];
                        }
                        else{
                            tmp_node=it_list_i->second->max_fresh;
                            while(tmp_node->next_fresh!=NULL&&(!((((*InfoTable)[tmp_node->id].score)>=it_buffer->second)&&((*InfoTable)[tmp_node->next_fresh->id].score)<it_buffer->second)))
                            {
                                tmp_node=tmp_node->next_fresh;
                            }
                            if(tmp_node->next_fresh==NULL)
                            {
                                tmp_node->next_fresh=(*(it_list_i->second->nodeMap))[it_buffer->first];
                                (*(it_list_i->second->nodeMap))[it_buffer->first]->next_fresh=NULL;
                            } else{
                                (*(it_list_i->second->nodeMap))[it_buffer->first]->next_fresh=tmp_node->next_fresh;
                                tmp_node->next_fresh=(*(it_list_i->second->nodeMap))[it_buffer->first];
                            }
                        }
                    }



                }
            }
        }
    }

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
    map<int,InvertedIndex*>* mirrorIndexMap;

    ForMirror(){mirrorIndexMap=NULL;}

    ForMirror(map<int,InvertedIndex*>* mirror):mirrorIndexMap(mirror){}

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
