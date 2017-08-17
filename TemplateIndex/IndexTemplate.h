//
// Created by billy on 17-7-23.
//

#ifndef HASH_0E_INDEXTEMPLATE_H
#define HASH_0E_INDEXTEMPLATE_H

#include "../BasicStructure/ProgramList.h"
#include "Sig.h"
#include "../PhonomeIndex/Phoneme.h"
#include "Cmp.h"
#include "../Merge/InvertedIndexMergerThreadFre.h"
#include "../Merge/InvertedIndexMergerThreadSig.h"
#include "../Merge/InvertedIndexMergerThreadSim.h"
#include "../Sort/I0SortThreadFre.h"
#include "../Sort/I0SortThreadSig.h"
#include "../Sort/I0SortThreadSim.h"

template <class T>
class IndexTemplate {
public:
    int AudioCount, level,TermCount,MergeCount;
    map<T,ProgramList*> *TermIndex;
    map<int, AudioInfo> *InfoTable;
    map<T, CMutex> *TermMutex;//only for I0
//    set<int> RemovedId;
    CMutex I0MutexInfo;
//    CMutex mutexRemove;
    CMutex termIndexMutex;//大致就是我进入这里查询的时候，TermIndex不能改变
    priority_queue<Sig> updateBuffer;
    CMutex bufferMutex;//由于I0不需要重新排序，所以它没有这个


    IndexTemplate()
    {
        AudioCount = 0;
        level = 0;
        TermCount=0;
        MergeCount=1;
        TermIndex = NULL;
        TermMutex=NULL;
        InfoTable = NULL;
    }

    IndexTemplate(int l):level(l){AudioCount=0;TermCount=0;MergeCount=1;}

    IndexTemplate(const IndexTemplate&other)
    {
        AudioCount=other.AudioCount;
        level=other.level;
        TermCount=other.TermCount;
        MergeCount=other.MergeCount;
        TermIndex = new map<T, ProgramList*>;
        typename map<T,ProgramList*>::iterator it_list;
        ProgramList* tmp;
        for(it_list=other.TermIndex->begin();it_list!=other.TermIndex->end();it_list++)
        {
            tmp= it_list->second->clone();
            (*TermIndex)[it_list->first]=tmp;
        }
        InfoTable =new map<int, AudioInfo> (*(other.InfoTable));
        TermMutex = new map<T,CMutex> (*(other.TermMutex));
    }

    bool search(int id)
    {
        if(InfoTable==NULL){
            return false;
        } else{
            map<int,AudioInfo>::iterator it_tmp=InfoTable->find(id);
            return it_tmp != InfoTable->end();
        }
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
        cout << "I" << level << "_AudioCount: " << AudioCount <<"\tTermCount: "<<TermCount<< "\n";
    }

    int get_count() { return AudioCount; }

    void MergerIndex(IndexTemplate &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<T> query, map<int, string> &name,map<int,score_ratio> &sco_vec,vector<double> &idf_vec);


    virtual double computeScore(const double &time, const double &score, map<T, double> &TermFreq, const int &tagsSum,
                                const vector<T> &query,const vector<double> &idf_vec,map<int,score_ratio> &sco_vec,int id) = 0;



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
            TermIndex=NULL;
        }
        if(InfoTable!=NULL) delete InfoTable;
        if(TermMutex!=NULL) delete TermMutex;
    }
};

template <class T>
class FamilyI0Sort
{
public:
    ProgramList* list;
    IndexTemplate<T>*me;
//    vector<int> id_list;
//    int flag;//1为fre，2为sig
//    typename map<T,ProgramList*>::iterator *it_list;
    FamilyI0Sort(ProgramList* l, IndexTemplate<T>* m): list(l), me(m){}
//    FamilyI0Sort(IndexTemplate<T>* m,vector<int> l,int f,typename map<T,ProgramList*>::iterator *mit)\
//    :me(m),id_list(l),flag(f){it_list=mit;}
};

template <class T>
void *Index0SortThreadFre(void *fam)
{
    auto sortList = new I0SortThreadFre<T>(fam);
    sortList->excecuteSort();
}

template <class T>
void *Index0SortThreadSig(void *fam)
{
    auto sortList = new I0SortThreadSig<T>(fam);
    sortList->excecuteSort();
}

template <class T>
void *Index0SortThreadSim(void *fam)
{
    auto sortList = new I0SortThreadSim<T>(fam);
    sortList->excecuteSort();
}

template <class T>
class ForMirror
{
public:
    CMutex mutex;
    map<int,T*>* mirrorIndexMap;
//    bool flag;//为了在外面删除方便

    ForMirror(){mirrorIndexMap=NULL;}

    ForMirror(map<int,T*>* mirror):mirrorIndexMap(mirror){}

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
    auto tt = new InvertedIndexMergerThreadFre<T>(fam);
    tt->excecuteMerge();
}

template <class T>
void *invertedIndexMergerThreadSig(void *fam)
{
    auto tt = new InvertedIndexMergerThreadSig<T>(fam);
    tt->excecuteMerge();
}

template <class T>
void *invertedIndexMergerThreadTermFreq(void *fam)
{
    auto tt = new InvertedIndexMergerThreadSim<T>(fam);
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
    typename map<T,ProgramList*>::iterator it_list_i;
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
    TermCount+=tmp_info.Termcount;

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
    static int count = 0;
    count++;
//    float a[13] = {-852.138245, 147.000305, 6.73238754, 136.820389, 45.5558205, -56.3611641, -68.4375534, -46.7173233, -3.07027578, -2.05147624, -22.4066162, -30.0924377};
//    static Phoneme label = Phoneme(a);
//    Phoneme p = (Phoneme)term;
//    if (p == label) {
//        cout << "3781!!" << endl;
//    }
    if ((it == (*TermIndex).end()))
    {
        (*TermMutex)[term] = CMutex();
        termIndexMutex.Lock();//感觉基本上修改全部的，都需要用这个互斥锁
        (*TermIndex)[term]=new ProgramList;
        termIndexMutex.Unlock();
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]->addNode(tf,id);
        (*TermMutex)[term].Unlock();
    }
    else
    {
        auto lock = (*TermMutex)[term];
        lock.Lock();
        auto test = (*TermIndex)[term];
        test->addNode(tf,id);
        lock.Unlock();
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
    TermCount+=tmp_info.Termcount;

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

                if((*TermIndex).find(it_node->first)==TermIndex->end())
                {
                    (*TermMutex)[it_node->first] = CMutex();
                    termIndexMutex.Lock();//感觉基本上修改全部的，都需要用这个互斥锁
                    (*TermIndex)[it_node->first]=new ProgramList;
                    termIndexMutex.Unlock();
                    (*TermMutex)[it_node->first].Lock();
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    (*TermMutex)[it_node->first].Unlock();
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }else{
                    (*TermMutex)[it_node->first].Lock();
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    (*TermMutex)[it_node->first].Unlock();
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                }
            }else if(it_node->second->flag==-2333)
            {
                it_node->second->flag=-1;
            }
        }
    }else if(tmp_info.final==0)
    {
        for (; it_node != livePointer[tmp_info.id].end();) {
            T str=it_node->first;

            if (it_node->second->flag == -1) {
                if((*TermIndex).find(it_node->first)==TermIndex->end())
                {
                    termIndexMutex.Lock();
                    (*TermIndex)[it_node->first]= new ProgramList;
                    termIndexMutex.Unlock();

                    (*TermMutex)[it_node->first].Lock();
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                    (*TermMutex)[it_node->first].Unlock();
                }else{
                    (*TermMutex)[it_node->first].Lock();
                    tmp=(*TermIndex)[it_node->first]->addNode(it_node->second->tf,it_node->second->id);
                    it_node->second->flag=0;
                    it_node->second=tmp;
                    it_node->second->flag=-1;
                    (*TermMutex)[it_node->first].Unlock();
                }
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
        termIndexMutex.Lock();
        (*TermIndex)[term]=new ProgramList;
        termIndexMutex.Unlock();
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
    mutexLive.Lock();
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
    mutexLive.Unlock();
}


template <class T>
void IndexTemplate<T>::I0_sort()
{
    vector<int> id_list;
    typename map<int,NodeInfo*>::iterator it_id;
    pthread_t pid1,pid2,pid3;
    map<T,ProgramList*> &other=*TermIndex;
    typename map<T,ProgramList*>::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {
        FamilyI0Sort<T> Fam1(it->second, this);
        pthread_create(&pid1,NULL,Index0SortThreadFre<T>,(void*)&Fam1);
        FamilyI0Sort<T> Fam2(it->second, this);
        pthread_create(&pid2,NULL,Index0SortThreadSim<T>,(void*)&Fam2);
        FamilyI0Sort<T> Fam3(it->second, this);
        pthread_create(&pid3,NULL,Index0SortThreadSig<T>,(void*)&Fam3);

        pthread_join(pid1,NULL);
        pthread_join(pid2,NULL);
        pthread_join(pid3,NULL);

        id_list.clear();
    }

}

template <class T>
void IndexTemplate<T>::MergerIndex(IndexTemplate<T> &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
//    int length;
    level++;
    pthread_t pid[3];
    int ret;
    insert_and_remove();
    other.insert_and_remove();
    auto forCheck = (*other.TermIndex).begin()->second;
    map<int,NodeInfo*> &tmp_nodemap=*((*other.TermIndex).begin()->second->nodeMap);
//    set<int>::iterator it_set;
//    map<int,AudioInfo> &tmp=(*InfoTable);
    // 在infotable删除旧的节点
    map<T,ProgramList *> &otherTermIndex=(*other.TermIndex);
    map<T,ProgramList *> &myTermIndex=(*TermIndex);



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
    map<int,NodeInfo*> ::iterator it_node_tmp_j;
    map<int,NodeInfo*> ::iterator it_node_tmp_i;

    //将otherindex的节点复制到Index中
    for(it_list_j=other.TermIndex->begin();it_list_j!=other.TermIndex->end();it_list_j++) {
        it_list_i = TermIndex->find(it_list_j->first);
        T str=it_list_j->first;


        if (it_list_i ==TermIndex->end()) {
            (*TermIndex)[it_list_j->first] = new ProgramList(*it_list_j->second);
        }else{

            for (it_node_tmp_j=((*other.TermIndex)[it_list_j->first]->nodeMap)->begin();\
            it_node_tmp_j!=((*other.TermIndex)[it_list_j->first]->nodeMap)->end();it_node_tmp_j++)
            {

                map<int,NodeInfo*>&thu=*(*TermIndex)[it_list_j->first]->nodeMap;

                it_node_tmp_i=(*TermIndex)[it_list_j->first]->nodeMap->find(it_node_tmp_j->second->id);
                if(it_node_tmp_i==(*TermIndex)[it_list_j->first]->nodeMap->end())
                {
                    (*(*TermIndex)[it_list_j->first]->nodeMap)[it_node_tmp_j->second->id]=it_node_tmp_j->second;//如果other的termindex中存在myself中没有的id，就拿过来
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

    pthread_join(pid[2],NULL);


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

    for(it_list_j=other.TermIndex->begin();it_list_j!=other.TermIndex->end();it_list_j++) {
        delete it_list_j->second->nodeMap;
        it_list_j->second->nodeMap=NULL;
    }

    for(it_list_i=TermIndex->begin();it_list_i!=TermIndex->end();) {
        if(it_list_i->second->nodeMap->size()==0)
        {
            delete it_list_i->second;
            it_list_i=TermIndex->erase(it_list_i);
        }else{
            it_list_i++;
        }
    }

}

template <class T>
void IndexTemplate<T>::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<T> query, map<int, string> &name,map<int,score_ratio> &sco_vec,vector<double> &idf_vec)
{
    double up_fre = 0;
    double up_sig = 0;
    int id1, id2, id3,id4;
    map<T, double> up_sim;
    double score = 0;
    map<T,double> TermFreq;
    map<int, double>::iterator it_res;
    map<int, NodeInfo *>::iterator it_tmp_node;//每个query中每个id获取全部tf时的迭代器
//    ofstream out_res("InvertedIndex_Result.txt", ofstream::app);



    if(level==0)
    {
        for (int i = 0; i < query.size(); i++)
        {
            if(TermIndex==NULL) break;
            typename map<T, ProgramList*>::iterator it = (*TermIndex).find(query[i]);
            map<int,NodeInfo*>::iterator it_node;
            if (it != (*TermIndex).end())
            {
                it->second->mutex.Lock();
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
                            score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idf_vec,sco_vec,it_node->second->id);
                            TermFreq.clear();
                            Sum += 1;
                            Result[it_node->second->id] = score;
                        }
                    }
                }
                it->second->mutex.Unlock();
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
                                    score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idf_vec,sco_vec,searchPointer->id);
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
                            int leng=it_exist->second->nodeMap->size();//长度不一定了，如果有提前结束的链，就先把null放进去
                            if (i < leng)
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

                                if(tmp_pointer!=NULL)
                                {
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
                                                                 query,idf_vec,sco_vec,id1);
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
                                    }else {
                                        pointer_que.push(NULL);
                                    }
                                }else{
                                    pointer_que.push(NULL);
                                }


                            }


                            if (i < leng)//迟早要删，先写后面的
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

                                if(tmp_pointer!=NULL)
                                {
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
                                                                 query,idf_vec,sco_vec,id2);
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
                                    } else{
                                        pointer_que.push(NULL);
                                    }
                                }else {
                                    pointer_que.push(NULL);
                                }

                            }


                            if (i < leng)
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

                                if(tmp_pointer!=NULL)
                                {
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
                                                                 query,idf_vec,sco_vec,id3);
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
                                    } else{
                                        pointer_que.push(NULL);
                                    }
                                } else{
                                    pointer_que.push(NULL);
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
                                            typename map<T, ProgramList *>::iterator it_str = (*TermIndex).find(
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
                                                             query,idf_vec,sco_vec,id4);
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
                        if (computeScore(up_fre, up_sig, up_sim, 0, query,idf_vec,sco_vec,0) < MinScore)
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
//        out_res << "size" << Sum << endl;
    }
}


#endif //HASH_0E_INDEXTEMPLATE_H
