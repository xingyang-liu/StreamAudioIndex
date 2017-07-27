//
// Created by billy on 17-7-23.
//

#ifndef HASH_0E_INDEXTEMPLATE_H
#define HASH_0E_INDEXTEMPLATE_H

#include "../src/ProgramList.h"
#include "../src/Sig.h"
#include "phonome.h"
#include "Cmp.h"
#include "InvertedIndexMergerThreadFre.h"
#include "InvertedIndexMergerThreadSig.h"
#include "InvertedIndexMergerThreadSim.h"

template <class T>
class IndexTemplate {
public:
    int AudioCount, level;
    map<T,ProgramList*> *TermIndex;
    map<int, AudioInfo> *InfoTable;
    map<T, CMutex> *TermMutex;//only for I0
    set<int> RemovedId;
    CMutex I0MutexInfo;
    CMutex mutexRemove;
    priority_queue<Sig> updateBuffer;
    CMutex bufferMutex;//由于I0不需要重新排序，所以它没有这个

    IndexTemplate()
    {
        AudioCount = 0;
        level = 0;
        TermIndex = NULL;
        TermMutex=NULL;
        InfoTable = NULL;
    }

    IndexTemplate(int l):level(l){AudioCount=0;}

    IndexTemplate(const IndexTemplate&other)
    {
        AudioCount=other.AudioCount;
        level=other.level;
        TermIndex = new map<string, ProgramList*>;
        typename map<T,ProgramList*>::iterator it_list;
        typename map<int,NodeInfo*>::iterator it_node;
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
        TermMutex = new map<T,CMutex> (*(other.TermMutex));
        RemovedId = other.RemovedId;
        I0_sort();
    }


    bool search(int id)
    {
        map<int,AudioInfo>::iterator it_tmp=InfoTable->find(id);
        return it_tmp != InfoTable->end();
    }

    void addAudio(AudioInfo &tmp_info,map<T,double> &TermFreq);

    void node_add(T term, int id, double tf);

    void addAudioLive(AudioInfo &tmp_info,map<T,double> &TermFreq,map<int, map<T, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive);

    void node_addLive(T term, int id, double tf, map<int, map<T, NodeInfo*> > &livePointer,int final,CMutex &mutexLive);

    void I0_sort();

    void update(int id,double score);

    void insert_and_remove();

    void output()
    {
        cout << "I" << level << "_count:" << AudioCount << "\n";
    }

    int get_count() { return AudioCount; }

    void MergerIndex(IndexTemplate &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<T> query, map<int, string> &name);

    virtual double computeScore(const double &time, const double &score, map<T, double> &TermFreq, const int &tagsSum,
                        const vector<T> &query);
    
    

//    template <class T>
    ~IndexTemplate()
    {
        if(TermIndex!=NULL)
        {
            map<T,ProgramList*> &other=*TermIndex;
            typename map<T,ProgramList*>::iterator it_list;
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

template <class T>
class FamilyI0Sort
{
public:
    IndexTemplate<T>*me;
    vector<int> id_list;
    int flag;//1为fre，2为sig
    typename map<T,ProgramList*>::iterator *it_list;

    FamilyI0Sort(IndexTemplate<T>* m,vector<int> l,int f,typename map<T,ProgramList*>::iterator *mit)\
    :me(m),id_list(l),flag(f){it_list=mit;}
};

template <class T>
void *I0SortThread(void *fam)
{
    FamilyI0Sort<T> &Fam=*(FamilyI0Sort<T> *)fam;
    IndexTemplate<T> *myself=Fam.me;
    vector<int> &id_list=Fam.id_list;
    int flag=Fam.flag;
    typename map<T,ProgramList*>::iterator &it=*Fam.it_list;



    if (flag==1)
    {
        sort(id_list.begin(),id_list.end(),CmpForFre<T>(it->first,myself));
        it->second->max_fresh=(*((it->second)->nodeMap))[id_list[0]];
        NodeInfo *pointer=it->second->max_fresh;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {

                (*pointer).next_fresh=(*(it->second)->nodeMap)[id_list[i]];
                pointer=(*pointer).next_fresh;
                if(i==id_list.size()-1)
                {
                    (*pointer).next_fresh=NULL;
                }

            }
        }
        else
        {
            (*pointer).next_fresh=NULL;
        }
    }
    else if(flag==2)
    {
        sort(id_list.begin(),id_list.end(),CmpForSig<T>(it->first,myself));
        it->second->max_sig=(*(it->second)->nodeMap)[id_list[0]];
        NodeInfo *pointer=it->second->max_sig;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {

                (*pointer).next_sig=(*(it->second)->nodeMap)[id_list[i]];
                pointer=(*pointer).next_sig;
                if(i==id_list.size()-1)
                {
                    (*pointer).next_sig=NULL;
                }

            }
        }
        else
        {
            (*pointer).next_sig=NULL;
        }
    }
    else if(flag==3)
    {
        sort(id_list.begin(),id_list.end(),CmpForSim<T>(it->first,myself));
        it->second->max_termFreq=(*((it->second)->nodeMap))[id_list[0]];
        NodeInfo *pointer=it->second->max_termFreq;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {

                (*pointer).next_termFreq=(*(it->second)->nodeMap)[id_list[i]];
                pointer=(*pointer).next_termFreq;
                if(i==id_list.size()-1)
                {
                    (*pointer).next_termFreq=NULL;
                }

            }
        }
        else
        {
            (*pointer).next_termFreq=NULL;
        }
    }
}


template <class T>
class ForMirror
{
public:
    CMutex mutex;
    map<int,T*>* mirrorIndexMap;
    bool flag;//为了在外面删除方便

    ForMirror(){mirrorIndexMap=NULL;flag=true;}

    ForMirror(map<int,T*>* mirror):mirrorIndexMap(mirror),flag(true){}

    ~ForMirror()
    {
        if(mirrorIndexMap!=NULL)
        {
            typename map<int,T*>::iterator it_tmp;
            for (it_tmp=mirrorIndexMap->begin();it_tmp!=mirrorIndexMap->end();it_tmp++)
            {
                delete it_tmp->second;
            }
            delete mirrorIndexMap;
            mirrorIndexMap=NULL;
        }
    }
};

template <class T>
class FamilyMerger
{
public:
    IndexTemplate<T>*me;
    IndexTemplate<T>*him;

    FamilyMerger(IndexTemplate<T>* myself,IndexTemplate<T>* other)
    {
        me=myself;
        him=other;
    }
};

template <class T>
void *invertedIndexMergerThreadFre(void *fam)
{
    InvertedIndexMergerThreadFre<T> *tt = new InvertedIndexMergerThreadFre<T>(fam);
    tt->excecuteMerge();
}

template <class T>
void *invertedIndexMergerThreadSig(void *fam)
{
    InvertedIndexMergerThreadSig<T> *tt = new InvertedIndexMergerThreadSig<T>(fam);
    tt->excecuteMerge();
}

template <class T>
void *invertedIndexMergerThreadTermFreq(void *fam)
{
    InvertedIndexMergerThreadSim<T> *tt = new InvertedIndexMergerThreadSim<T>(fam);
    tt->excecuteMerge();
}

template <class T>
void IndexTemplate<T>::update(int id,double score)
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

template<class T>
void IndexTemplate<T>::insert_and_remove()//在merger开始之前，buffer中所有的项
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
                tmp_node=it_list_i->second->max_sig;
                if(it_list_i->second->max_sig->next_sig!=NULL)//如果只有一个node就没有任何操作
                {
                    if(it_list_i->second->max_sig->id==it_buffer->first)//如果max_sig正好是要改的值，那就先把它从链里去掉
                    {
                        it_list_i->second->max_sig=it_list_i->second->max_sig->next_sig;
                    }
                    else{//也是从链中挖去这个点
                        while(tmp_node->next_sig->next_sig!=NULL&&tmp_node->next_sig->id!=it_buffer->first)
                        {
                            tmp_node=tmp_node->next_sig;
                        }
                        tmp_node->next_sig=tmp_node->next_sig->next_sig;
                    }

                    //准备重新放进去

                    if(it_buffer->second>=(*InfoTable)[it_list_i->second->max_sig->id].score)
                    {
                        (*(it_list_i->second->nodeMap))[it_buffer->first]->next_sig=it_list_i->second->max_sig;
                        it_list_i->second->max_sig=(*(it_list_i->second->nodeMap))[it_buffer->first];
                    }
                    else{
                        tmp_node=it_list_i->second->max_sig;
                        while(tmp_node->next_sig!=NULL&&(!((((*InfoTable)[tmp_node->id].score)>=it_buffer->second)&&((*InfoTable)[tmp_node->next_sig->id].score)<it_buffer->second)))
                        {
                            tmp_node=tmp_node->next_sig;
                        }
                        if(tmp_node->next_sig==NULL)
                        {
                            tmp_node->next_sig=(*(it_list_i->second->nodeMap))[it_buffer->first];
                            (*(it_list_i->second->nodeMap))[it_buffer->first]->next_sig=NULL;
                        } else{
                            (*(it_list_i->second->nodeMap))[it_buffer->first]->next_sig=tmp_node->next_sig;
                            tmp_node->next_sig=(*(it_list_i->second->nodeMap))[it_buffer->first];
                        }
                    }
                }
            }
        }
    }
}

template <class T>
void IndexTemplate<T>::addAudio(AudioInfo &tmp_info,map<T,double> &TermFreq){
    if(TermIndex==NULL)
    {
        TermIndex =new map<T,ProgramList*>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<T,CMutex>;
    }

    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    typename map<T, double>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        node_add(it->first, tmp_info.id, TermFreq[it->first]);
    }
}

template <class T>
void IndexTemplate<T>::node_add(T term, int id, double tf)
{
    typename map<T, ProgramList* >::iterator it = (*TermIndex).find(term);
    if ((it == (*TermIndex).end()))
    {
        (*TermMutex)[term] = CMutex();
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]=new ProgramList;
        (*TermIndex)[term]->addNode(tf,id);
        (*TermMutex)[term].Unlock();
    }
    else
    {
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]->addNode(tf,id);
        (*TermMutex)[term].Unlock();
    }
}

template <class T>
void IndexTemplate<T>::addAudioLive(AudioInfo &tmp_info,map<T,double> &TermFreq,map<int, map<T, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive)
{
    if(TermIndex==NULL)
    {
        TermIndex = new map<T,ProgramList*>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<T,CMutex>;
    }
    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    typename map<T, double>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        node_addLive(it->first, tmp_info.id, TermFreq[it->first],livePointer,tmp_info.final,mutexLive);
    }


    typename map<T,NodeInfo*>::iterator it_node=livePointer[tmp_info.id].begin();
    NodeInfo *tmp;
    if(tmp_info.final>0) {
        for (; it_node != livePointer[tmp_info.id].end(); it_node++) {
            if (it_node->second->flag == -1) {
                (*TermMutex)[it_node->first].Lock();
                if((*TermIndex).find(it_node->first)==TermIndex->end())
                {
                    (*TermIndex)[it_node->first]= new ProgramList;
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }else{
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }

                (*TermMutex)[it_node->first].Unlock();
            }else if(it_node->second->flag==-2333)
            {
                it_node->second->flag=-1;
            }
        }
    }else if(tmp_info.final==0)
    {
        for (; it_node != livePointer[tmp_info.id].end();) {
            string str=it_node->first;

            if (it_node->second->flag == -1) {
                if((*TermIndex).find(it_node->first)==TermIndex->end())
                {
                    (*TermIndex)[it_node->first]= new ProgramList;
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }else{
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }
                (*TermMutex)[it_node->first].Unlock();
            }

            it_node=livePointer[it_node->second->id].erase(it_node);

        }
        mutexLive.Lock();
        livePointer.erase(tmp_info.id);
        mutexLive.Unlock();
    }
}

template <class T>
void IndexTemplate<T>::node_addLive(T term, int id, double tf, map<int, map<T, NodeInfo*> > &livePointer,int final,CMutex &mutexLive)
{
    typename map<T, ProgramList* >::iterator it = (*TermIndex).find(term);
    NodeInfo *tmp;
    if ((it == (*TermIndex).end()))
    {

        (*TermMutex)[term] = CMutex();
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]=new ProgramList;
        if(livePointer[id].find(term)==livePointer[id].end())
        {
            tmp=(*TermIndex)[term]->addNode(tf,id);
        }
        else
        {
            tmp=(*TermIndex)[term]->addNode(tf+(*(livePointer[id][term])).tf,id);
        }
        (*TermMutex)[term].Unlock();
    }
    else
    {
        (*TermMutex)[term].Lock();
        if(livePointer[id].find(term)==livePointer[id].end())
        {
            tmp=(*TermIndex)[term]->addNode(tf,id);
        }
        else
        {
            tmp=(*TermIndex)[term]->addNode(tf+(*(livePointer[id][term])).tf,id);
        }
        (*TermMutex)[term].Unlock();
    }

    if(final >0)
    {
        if(livePointer[id].find(term)!=livePointer[id].end())
        {
            livePointer[id][term]->flag=0;
            livePointer[id][term]=tmp;
            livePointer[id][term]->flag=-2333;
        }
        else
        {
            livePointer[id][term]=tmp;
            livePointer[id][term]->flag=-2333;
        }

    }
    else
    {
        if(livePointer[id].find(term)!=livePointer[id].end()) {
            livePointer[id][term]->flag = 0;
            livePointer[id].erase(term);
        }
    }
}

template <class T>
double IndexTemplate<T>::computeScore(const double &time, const double &score, map<T, double> &TermFreq, const int &tagsSum,
                                   const vector<T> &query)
{
    double fre = pow(2, time - getTime());
    double sig = log(score / 10000 + 1);
    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        typename map<T, double>::iterator it = TermFreq.find(query[i]);
        if (it != TermFreq.end())
        {
            try
            {
                if (tagsSum != 0)
                {
                    sim += TermFreq[query[i]] *IdfTable[query[i]];
                }
                else
                {
                    sim += TermFreq[query[i]] * IdfTable[query[i]];
                }
            }
            catch (...)
            {
                map<string, double>::iterator it = IdfTable.find(query[i]);
                if (it != IdfTable.end())
                {
                    continue;
                }
                else
                {
                    cout << "Something wrong with computeScore()";
                }

            }
        }
    }
    double ScoreAll = (fre*0.1 + sim * 20 * 0.6 + sig*0.2)*10;
    return ScoreAll;
}

template <class T>
void IndexTemplate<T>::I0_sort()
{
    vector<int> id_list;
    typename map<int,NodeInfo*>::iterator it_id;
    NodeInfo *pointer;
    pthread_t pid1,pid2,pid3;
    map<T,ProgramList*> &other=*TermIndex;
    typename map<T,ProgramList*>::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {

        for(it_id=((it->second)->nodeMap)->begin();it_id!=((it->second)->nodeMap)->end();it_id++)
        {
            id_list.push_back(it_id->first);
        }
        if(id_list.size()!=0)
        {
            FamilyI0Sort<T> Fam1(this,id_list,1,&it);
            pthread_create(&pid1,NULL,I0SortThread<T>,(void*)&Fam1);
            FamilyI0Sort<T> Fam2(this,id_list,2,&it);
            pthread_create(&pid2,NULL,I0SortThread<T>,(void*)&Fam2);
            FamilyI0Sort<T> Fam3(this,id_list,3,&it);
            pthread_create(&pid3,NULL,I0SortThread<T>,(void*)&Fam3);

            pthread_join(pid1,NULL);
            pthread_join(pid2,NULL);
            pthread_join(pid3,NULL);

            id_list.clear();
        }
    }

}

template <class T>
void IndexTemplate<T>::MergerIndex(IndexTemplate<T> &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
    int length;
    level++;
//    AudioCount += other.AudioCount;

    //merger info_table
    pthread_t pid[3];
    int ret;
    map<int,NodeInfo*> &tmp_nodemap=*((*other.TermIndex).begin()->second->nodeMap);
    set<int>::iterator it_set;
//    map<int,AudioInfo> &tmp=(*InfoTable);
    // 在infotable删除旧的节点

    map<T,ProgramList *> &otherTermIndex=(*other.TermIndex);
    map<T,ProgramList *> &myTermIndex=(*TermIndex);


    mutexRemove.Lock();
    for (it_set=RemovedId.begin();it_set!=RemovedId.end();it_set++)
    {

        (*InfoTable).erase(*it_set);
    }


    for (it_set=other.RemovedId.begin();it_set!=other.RemovedId.end();it_set++)
    {

        (*other.InfoTable).erase(*it_set);
        RemovedId.insert(*it_set);
    }
    mutexRemove.Unlock();

    //将merge过来的infotable中的不重复的info放入该infotable中
    map<int,AudioInfo>::iterator it_info;
    for(it_info=other.InfoTable->begin();it_info!=other.InfoTable->end();it_info++)
    {
        if((*InfoTable).find(it_info->first)==(*InfoTable).end())
        {
            (*InfoTable)[it_info->first]=it_info->second;
        }
    }


    typename map<T,ProgramList*>::iterator it_list_i;
    typename map<T,ProgramList*>::iterator it_list_j;//otherIndex的programlist
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;
    map<int,NodeInfo*> ::iterator it_node_tmp;

    //将otherindex的节点复制到Index中
    for(it_list_j=other.TermIndex->begin();it_list_j!=other.TermIndex->end();it_list_j++) {
        it_list_i = TermIndex->find(it_list_j->first);
        map<T,ProgramList*> &otherone=(*other.TermIndex);
        map<T,ProgramList*> &oneone=(*TermIndex);
        T str=it_list_j->first;


        if (it_list_i ==TermIndex->end()) {
            (*TermIndex)[it_list_j->first] = new ProgramList;
            //it_node_tmp是当前programlist对应的node
            for (it_node_tmp=((*other.TermIndex)[it_list_j->first]->nodeMap)->begin();\
            it_node_tmp!=((*other.TermIndex)[it_list_j->first]->nodeMap)->end();it_node_tmp++)
            {
                if(it_node_tmp->second->flag==-1)
                {
                    (*TermIndex)[it_list_j->first]->addNode(it_node_tmp->second->tf,it_node_tmp->second->id);
                }
            }
        }else{

            for (it_node_tmp=((*other.TermIndex)[it_list_j->first]->nodeMap)->begin();\
            it_node_tmp!=((*other.TermIndex)[it_list_j->first]->nodeMap)->end();it_node_tmp++)
            {
                if(it_node_tmp->second->flag==-1){
                    (*TermIndex)[it_list_j->first]->getNodePointer(it_node_tmp->second->tf,it_node_tmp->second->id);
                }
            }
        }
    }

//    map<int,AudioInfo> &tmp=*InfoTable;

    FamilyMerger<T> one(this,&other);
    ret=pthread_create(&pid[0],NULL,invertedIndexMergerThreadFre<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    pthread_join(pid[0],NULL);


    ret=pthread_create(&pid[2],NULL,invertedIndexMergerThreadSig<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    ret=pthread_create(&pid[1],NULL,invertedIndexMergerThreadTermFreq<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    for (int i=0;i<3;i++)
    {
        pthread_join(pid[i],NULL);
    }

    AudioCount=(*InfoTable).size();

    //归并idIndex

    for(it_list_i=TermIndex->begin();it_list_i!=TermIndex->end();it_list_i++)
    {
        for(it_node_i=it_list_i->second->nodeMap->begin();it_node_i!=it_list_i->second->nodeMap->end();)
        {
            if(it_node_i->second->flag==0)
            {
                delete it_node_i->second;
                it_node_i=it_list_i->second->nodeMap->erase(it_node_i);
            }else
            {
                it_node_i++;
            }
        }
    }

    RemovedId.clear();

}

template <class T>
void IndexTemplate<T>::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<T> query, map<int, string> &name)
{
    double up_fre = 0;
    double up_sig = 0;
    int id1, id2, id3,id4;
    map<T, double> up_sim;
    double score = 0;
    map<T,double> TermFreq;
    map<int, double>::iterator it_res;
    map<int, NodeInfo *>::iterator it_tmp_node;//每个query中每个id获取全部tf时的迭代器
    ofstream out_res("InvertedIndex_Result.txt", ofstream::app);

    if(level==0)
    {
        for (int i = 0; i < query.size(); i++)
        {
            if(TermIndex==NULL) break;
            typename map<T, ProgramList*>::iterator it = (*TermIndex).find(query[i]);
            map<int,NodeInfo*>::iterator it_node;
            if (it != (*TermIndex).end())
            {
                for(it_node=it->second->nodeMap->begin();it_node!=it->second->nodeMap->end();it_node++)
                {
                    map<int, double>::iterator it_res = Result.find(it_node->second->id);
                    if (it_res == Result.end())
                    {
                        if(it_node->second->flag==-1)
                        {
                            AudioInfo &info_tmp = (*InfoTable)[it_node->second->id];
                            for (int k = 0; k < query.size(); k++)
                            {
                                typename map<T, ProgramList*>::iterator it_str = (*TermIndex).find(query[i]);
                                if (it_str != TermIndex->end())
                                {
                                    map<int,NodeInfo*> &tmp=*it_str->second->nodeMap;
                                    NodeInfo *&tmp2=it_node->second;

                                    it_tmp_node = it_str->second->nodeMap->find(it_node->second->id);
                                    if (it_tmp_node != it_str->second->nodeMap->end())
                                    {
                                        TermFreq[query[k]] = it_tmp_node->second->tf;
                                    }
                                }

                            }
                            score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query);
                            TermFreq.clear();
                            Sum += 1;
                            Result[it_node->second->id] = score;
                        }
                    }
                }
            }

        }
        vector<pair<int, double> > ResVector(Result.begin(), Result.end());
        sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
        Result.clear();
        if (Sum >= AnsNum)
        {
            for (int i = 0; i < AnsNum; i++)
            {
                Result[ResVector[i].first] = ResVector[i].second;
                map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                if (it_name != (*InfoTable).end())
                {
                    name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                }
            }
            MinScore = ResVector[AnsNum - 1].second;
            Sum = AnsNum;
        }
        else if (ResVector.size()>0)
        {
            for (int i = 0; i < ResVector.size(); i++)
            {
                Result[ResVector[i].first] = ResVector[i].second;
                map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                if (it_name != (*InfoTable).end())
                {
                    name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                }
            }

            MinScore = ResVector[ResVector.size() - 1].second;
            Sum = ResVector.size();
        }
    }
    else
    {
        if (Sum < AnsNum)
        {
            for (int i = 0; i < query.size(); i++)
            {
                map<int,AudioInfo> &tmp1=*InfoTable;

                typename map<T, ProgramList* >::iterator it = (*TermIndex).find(query[i]);
                if (it != (*TermIndex).end())
                {
                    if(it->second->max_fresh!=NULL)
                    {
                        NodeInfo *searchPointer=(it->second->max_fresh);
                        map<int,NodeInfo*> &tmp2=*it->second->nodeMap;

                        for (; searchPointer!=NULL; searchPointer=searchPointer->next_fresh)
                        {
                            it_res = Result.find(searchPointer->id);
                            if (it_res == Result.end())
                            {
                                if(searchPointer->flag==-1) {
                                    map<int, AudioInfo> &tmp3 = *InfoTable;

                                    AudioInfo &info_tmp = (*InfoTable)[searchPointer->id];

                                    for (int k = 0; k < query.size(); k++) {
                                        typename map<T, ProgramList *>::iterator it_str = (*TermIndex).find(query[k]);
                                        if (it_str != TermIndex->end()) {
                                            it_tmp_node = it_str->second->nodeMap->find(searchPointer->id);
                                            if (it_tmp_node != it_str->second->nodeMap->end()) {
                                                TermFreq[query[k]] = it_tmp_node->second->tf;
                                            }
                                        }

                                    }
                                    score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum,
                                                         query);
                                    TermFreq.clear();
                                    Sum += 1;
                                    Result[searchPointer->id] = score;
                                    if (Sum >= AnsNum)
                                        break;
                                }
                            }
                        }
                    }
                }
            }

            vector<pair<int, double> > ResVector(Result.begin(), Result.end());
            sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
            Result.clear();
            if (Sum >= AnsNum)
            {
                for (int i = 0; i < AnsNum; i++)
                {
                    Result[ResVector[i].first] = ResVector[i].second;
                    map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }
                MinScore = ResVector[AnsNum - 1].second;
                Sum = AnsNum;
            }
            else if (ResVector.size()>0)
            {
                for (int i = 0; i < ResVector.size(); i++)
                {
                    Result[ResVector[i].first] = ResVector[i].second;
                    map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }

                MinScore = ResVector[ResVector.size() - 1].second;
                Sum = ResVector.size();
            }
        }

        if (Sum >= AnsNum)
        {
            bool TerFlag = false;
            queue<NodeInfo*> pointer_que;
            for (int i = 0; i < (*TermIndex).size(); i++)
            {
                if (TerFlag)
                    break;
                up_fre = 0;
                up_sig = 0;

                for (int j = 0; j < query.size(); j++)
                {
                    up_sim[query[j]] = 0;
                }


                NodeInfo* tmp_pointer;
                priority_queue<Sig> copyBuffer(updateBuffer);

                try
                {
                    for (int j = 0; j < query.size(); j++)
                    {
                        typename map<T, ProgramList* >::iterator it_exist = (*TermIndex).find(query[j]);

                        if (it_exist != (*TermIndex).end())
                        {
                            if (i < it_exist->second->nodeMap->size())
                            {
                                //fre模块
                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second->max_fresh;
                                }
                                else
                                {
                                    tmp_pointer=pointer_que.front();
                                    pointer_que.pop();
                                }


                                id1 = (*tmp_pointer).id;
                                it_res = Result.find(id1);
                                if (it_res == Result.end())
                                {
                                    AudioInfo &info_tmp = (*InfoTable)[id1];
                                    if (tmp_pointer->flag==-1)
                                    {


                                        for (int k=0;k<query.size();k++)//因为我已经判断Result，然后
                                        {
                                            typename map<T, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
                                            if(it_str!=TermIndex->end())
                                            {
                                                map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id1);
                                                if(it_tmp_node!=it_str->second->nodeMap->end())
                                                {
                                                    TermFreq[query[k]]=it_tmp_node->second->tf;
                                                }
                                            }

                                        }
                                        score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum,
                                                             query);
                                        TermFreq.clear();
                                        if (score > MinScore)
                                        {
                                            Result[id1] = score;
                                            MinScore = score;
                                            Sum += 1;
                                        }
                                    }
                                    if (up_fre < info_tmp.time)
                                    {
                                        up_fre = info_tmp.time;
                                    }


                                }
                                if(tmp_pointer->next_fresh!=NULL)
                                {
                                    pointer_que.push(tmp_pointer->next_fresh);
                                }

                            }


                            if (i < it_exist->second->nodeMap->size())//迟早要删，先写后面的
                            {

                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second->max_sig;
                                }
                                else
                                {
                                    tmp_pointer=pointer_que.front();
                                    pointer_que.pop();
                                }

                                id2 = (*tmp_pointer).id;
                                it_res = Result.find(id2);
                                if (it_res == Result.end())
                                {
                                    AudioInfo &info_tmp = (*InfoTable)[id2];
                                    if (tmp_pointer->flag==-1)
                                    {


                                        for (int k=0;k<query.size();k++)
                                        {
                                            typename map<T, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
                                            if(it_str!=TermIndex->end())
                                            {
                                                map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id2);
                                                if(it_tmp_node!=it_str->second->nodeMap->end())
                                                {
                                                    TermFreq[query[k]]=it_tmp_node->second->tf;
                                                }
                                            }
                                        }
                                        score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum,
                                                             query);
                                        TermFreq.clear();
                                        if (score > MinScore)
                                        {
                                            Result[id2] = score;
                                            MinScore = score;
                                            Sum += 1;
                                        }
                                    }
                                    if (up_sig < info_tmp.score)
                                    {
                                        up_sig = info_tmp.score;
                                    }


                                }
                                if(tmp_pointer->next_sig!=NULL)
                                {
                                    pointer_que.push(tmp_pointer->next_sig);
                                }
                            }


                            if (i < it_exist->second->nodeMap->size())
                            {
                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second->max_termFreq;
                                }
                                else
                                {
                                    tmp_pointer=pointer_que.front();
                                    pointer_que.pop();
                                }

                                id3 = (*tmp_pointer).id;
                                it_res = Result.find(id3);
                                if (it_res == Result.end())
                                {
                                    if (tmp_pointer->flag==-1)
                                    {
                                        AudioInfo &info_tmp = (*InfoTable)[id3];
                                        for (int k=0;k<query.size();k++)
                                        {
                                            typename map<T, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
                                            if(it_str!=TermIndex->end())
                                            {
                                                map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id3);
                                                if(it_tmp_node!=it_str->second->nodeMap->end())
                                                {
                                                    TermFreq[query[k]]=it_tmp_node->second->tf;
                                                }
                                            }

                                        }



                                        score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum,
                                                             query);
                                        if (score > MinScore)
                                        {
                                            Result[id3] = score;
                                            MinScore = score;
                                            Sum += 1;
                                        }
                                    }
                                    if (up_sim[query[j]] < tmp_pointer->tf)
                                    {
                                        up_sim[query[j]] = tmp_pointer->tf;
                                    }


                                }
                                if(tmp_pointer->next_termFreq!=NULL)
                                {
                                    pointer_que.push(tmp_pointer->next_termFreq);
                                }
                            }
                            else
                            {
                                up_sim[query[j]] = 0;
                            }

                            if(copyBuffer.size()>0) {
                                while (up_sig < copyBuffer.top().get_score()) {

                                    id4 = (copyBuffer.top().get_id());
                                    copyBuffer.pop();
                                    it_res = Result.find(id4);
                                    if (it_res == Result.end()) {
                                        AudioInfo &info_tmp = (*InfoTable)[id4];
                                        for (int k = 0; k < query.size(); k++) {
                                            map<string, ProgramList *>::iterator it_str = (*TermIndex).find(
                                                    query[k]);
                                            if (it_str != TermIndex->end()) {
                                                map<int, NodeInfo *>::iterator it_tmp_node = it_str->second->nodeMap->find(
                                                        id4);
                                                if (it_tmp_node != it_str->second->nodeMap->end()) {
                                                    TermFreq[query[k]] = it_tmp_node->second->tf;
                                                }
                                            }

                                        }
                                        score = computeScore(info_tmp.time, info_tmp.score, TermFreq,
                                                             info_tmp.TagsSum,
                                                             query);
                                        TermFreq.clear();
                                        if (score > MinScore) {
                                            Result[id4] = score;
                                            MinScore = score;
                                            Sum += 1;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    try//你也可以放到与i的循环同一级，我觉得问题不大
                    {
                        if (computeScore(up_fre, up_sig, up_sim, 0, query) < MinScore)
                        {
                            TerFlag = true;
                            break;
                        }
                    }
                    catch (...)
                    {
                        cout << "computeScore() with up has something wrong." << endl;
                    }
                }
                catch (...)
                {
                    cout << "Search with up has something wrong." << endl;
                }
            }
            vector<pair<int, double> > ResVector(Result.begin(), Result.end());
            sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
            Result.clear();
            if (Sum >= AnsNum)
            {
                for (int i = 0; i < AnsNum; i++)
                {
                    Result[ResVector[i].first] = ResVector[i].second;
                    map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }
                MinScore = ResVector[AnsNum - 1].second;
                Sum = AnsNum;
            }
            else
            {
                for (int i = 0; i < ResVector.size(); i++)
                {
                    Result[ResVector[i].first] = ResVector[i].second;
                    map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }
                MinScore = ResVector[-1].second;
                Sum = ResVector.size();
            }

        }
        out_res << "size" << Sum << endl;
    }


}


#endif //HASH_0E_INDEXTEMPLATE_H
