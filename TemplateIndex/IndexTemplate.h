//
// Created by billy on 17-7-23.
//

#ifndef HASH_0E_INDEXTEMPLATE_H
#define HASH_0E_INDEXTEMPLATE_H

#include "../BasicStructure/ProgramList.h"

#include "../utils/utils.h"
#include "Sig.h"
#include "../PhonomeIndex/phonome.h"
#include "Cmp.h"
#include "../Merge/InvertedIndexMergerThreadFre.h"
#include "../Merge/InvertedIndexMergerThreadSig.h"
#include "../Merge/InvertedIndexMergerThreadSim.h"
#include "../I0Sort/I0SortThreadFre.h"
#include "../I0Sort/I0SortThreadSig.h"
#include "../I0Sort/I0SortThreadSim.h"

template <class T>
class IndexTemplate {
public:
    int audioCount, level,termCount,mergeCount;
    btree::btree_map<T,ProgramList*> *TermIndex;//为了空间
    btree::btree_map<int, AudioInfo> *InfoTable;
    btree::btree_map<T, CMutex> *TermMutex;//only for I0
    CMutex I0MutexInfo;
    CMutex termIndexMutex;//大致就是我进入这里查询的时候，TermIndex不能改变
    priority_queue<Sig> updateBuffer;
    CMutex bufferMutex;//由于I0不需要重新排序，所以它没有这个


    IndexTemplate()
    {
        audioCount = 0;
        level = 0;
        termCount=0;
        mergeCount=1;
        TermIndex = NULL;
        TermMutex=NULL;
        InfoTable = NULL;
    }

    IndexTemplate(int l):level(l){audioCount=0;termCount=0;mergeCount=1;}

    IndexTemplate(const IndexTemplate&other)
    {
        audioCount=other.audioCount;
        level=other.level;
        termCount=other.termCount;
        mergeCount=other.mergeCount;
        TermIndex = new btree::btree_map<T,ProgramList*>;
        ProgramList* tmp;
        for(auto itList=other.TermIndex->begin();itList!=other.TermIndex->end();itList++)
        {
            tmp= itList->second->clone();
            (*TermIndex)[itList->first]=tmp;
        }
        InfoTable =new btree::btree_map<int, AudioInfo> (*(other.InfoTable));
        TermMutex = new btree::btree_map<T,CMutex> (*(other.TermMutex));
    }

    bool search(int id)
    {
        if(InfoTable==NULL){
            return false;
        } else{
            btree::btree_map<int,AudioInfo>::iterator it_tmp=InfoTable->find(id);
            return it_tmp != InfoTable->end();
        }
    }

    void addAudio(AudioInfo &tmpInfo,btree::btree_map<T,double> &TermFreq);

    void addNode(T term, int id, double tf);

    void output()
    {
        cout << "I" << level << "_count:" << audioCount << "\n";
    }

    void addAudioLive(AudioInfo &tmpInfo,btree::btree_map<T,double> &TermFreq,btree::btree_map<int, \
    btree::btree_map<T, NodeInfo *> > &livePointer,CMutex &mutexLive);

    void addLiveNode(T term, int id, double tf, btree::btree_map<int, btree::btree_map<T, NodeInfo *> > &livePointer,
                     int final, CMutex &mutexLive);

    void I0_sort();

    void update(int id,double score);

    void insertAndRemove();

    void mergerIndex(IndexTemplate &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

    void search(btree::btree_map<int, double> &result, double &minScore, int &ansNum, int &sum, const vector<T> query,const vector<double> idf_vec , btree::btree_map<int, string> &name,btree::btree_map<int,score_ratio> &sco_vec);

    double computeScore(const double &time, const double &score, btree::btree_map<T, double> &TermFreq, const int &tagsSum,
                        const vector<T> &query,const vector<double> &idf_vec);
    double computeScore(const double &time, const double &score, btree::btree_map<T, double> &TermFreq, const int &tagsSum,
                                const vector<T> &query,const vector<double> &idf_vec,btree::btree_map<int,score_ratio> &sco_vec,int id);

    ~IndexTemplate()
    {
//        cout << "addr: " << this << "; program list addr: " << TermIndex << endl;
        if(TermIndex!=NULL)
        {
            //btree::btree_map<T,ProgramList*> &other=*TermIndex;
            //typename btree::btree_map<T,ProgramList*>::iterator it_list;
//            cout << "numof terms: " << TermIndex->size() << endl;
            for (auto it_list=TermIndex->begin();it_list!=TermIndex->end();it_list++)
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

    FamilyI0Sort(ProgramList* l, IndexTemplate<T>* m): list(l), me(m){}
};

template <class T>
void *Index0SortThreadFre(void *fam)
{
    auto sortList = new I0SortThreadFre<T>(fam);
    sortList->excecuteSort();
    delete sortList;
}

template <class T>
void *Index0SortThreadSig(void *fam)
{
    auto sortList = new I0SortThreadSig<T>(fam);
    sortList->excecuteSort();
    delete sortList;
}

template <class T>
void *Index0SortThreadSim(void *fam)
{
    auto sortList = new I0SortThreadSim<T>(fam);
    sortList->excecuteSort();
    delete sortList;
}

template <class T>
class ForMirror
{
public:
    btree::btree_map<int,T*>* mirrorIndexMap;

    ForMirror(){mirrorIndexMap=NULL;}

    ForMirror(btree::btree_map<int,T*>* mirror):mirrorIndexMap(mirror){}

    ~ForMirror()
    {
        if(mirrorIndexMap!=NULL)
        {
            typename btree::btree_map<int,T*>::iterator it_tmp;
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
    delete tt;
}

template <class T>
void *invertedIndexMergerThreadSig(void *fam)
{
    InvertedIndexMergerThreadSig<T> *tt = new InvertedIndexMergerThreadSig<T>(fam);
    tt->excecuteMerge();
    delete tt;
}

template <class T>
void *invertedIndexMergerThreadTermFreq(void *fam)
{
    InvertedIndexMergerThreadSim<T> *tt = new InvertedIndexMergerThreadSim<T>(fam);
    tt->excecuteMerge();
    delete tt;
}

template <class T>
void IndexTemplate<T>::update(int id,double score)
{
    I0MutexInfo.Lock();
    btree::btree_map<int, AudioInfo>::iterator it_info=InfoTable->find(id);
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
void IndexTemplate<T>::insertAndRemove()//在merger开始之前，buffer中所有的项
{
    NodeInfo* tmp_node;
    btree::btree_map<int,double> tmpBuffer;
    for(int i=0;i<updateBuffer.size();i++)
    {
        tmpBuffer[updateBuffer.top().id]=updateBuffer.top().score;
        updateBuffer.pop();
    }

    btree::btree_map<int,double>::iterator itBuffer;
    typename btree::btree_map<T,ProgramList*>::iterator itListI;
    for(itListI=TermIndex->begin();itListI!=TermIndex->end();itListI++)
    {
        for(itBuffer=tmpBuffer.begin();itBuffer!=tmpBuffer.end();itBuffer++)
        {
            if(itListI->second->nodeMap->find(itBuffer->first)!=itListI->second->nodeMap->end())
            {
                tmp_node=itListI->second->max_sig;
                if(itListI->second->max_sig->next_sig!=NULL)//如果只有一个node就没有任何操作
                {
                    if(itListI->second->max_sig->id==itBuffer->first)//如果max_sig正好是要改的值，那就先把它从链里去掉
                    {
                        itListI->second->max_sig=itListI->second->max_sig->next_sig;
                    }
                    else{//也是从链中挖去这个点
                        while(tmp_node->next_sig->next_sig!=NULL&&tmp_node->next_sig->id!=itBuffer->first)
                        {
                            tmp_node=tmp_node->next_sig;
                        }
                        tmp_node->next_sig=tmp_node->next_sig->next_sig;
                    }

                    //准备重新放进去
                    if(itBuffer->second>=(*InfoTable)[itListI->second->max_sig->id].score)
                    {
                        (*(itListI->second->nodeMap))[itBuffer->first]->next_sig=itListI->second->max_sig;
                        itListI->second->max_sig=(*(itListI->second->nodeMap))[itBuffer->first];
                    }
                    else{
                        tmp_node=itListI->second->max_sig;
                        while(tmp_node->next_sig!=NULL&&(!((((*InfoTable)[tmp_node->id].score)>=itBuffer->second)&&\
                        ((*InfoTable)[tmp_node->next_sig->id].score)<itBuffer->second)))
                        {
                            tmp_node=tmp_node->next_sig;
                        }
                        if(tmp_node->next_sig==NULL)
                        {
                            tmp_node->next_sig=(*(itListI->second->nodeMap))[itBuffer->first];
                            (*(itListI->second->nodeMap))[itBuffer->first]->next_sig=NULL;
                        } else{
                            (*(itListI->second->nodeMap))[itBuffer->first]->next_sig=tmp_node->next_sig;
                            tmp_node->next_sig=(*(itListI->second->nodeMap))[itBuffer->first];
                        }
                    }
                }
            }
        }
    }
}

template <class T>
void IndexTemplate<T>::addAudio(AudioInfo &tmpInfo,btree::btree_map<T,double> &TermFreq){
    if(TermIndex==NULL)
    {
        TermIndex =new btree::btree_map<T,ProgramList*>;
        InfoTable = new btree::btree_map<int, AudioInfo>;
        TermMutex = new btree::btree_map<T,CMutex>;
    }

    I0MutexInfo.Lock();
    (*InfoTable)[tmpInfo.id] = tmpInfo;
    I0MutexInfo.Unlock();
    audioCount++;

    typename btree::btree_map<T, double>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        addNode(it->first, tmpInfo.id, TermFreq[it->first]);
    }
}

template <class T>
void IndexTemplate<T>::addNode(T term, int id, double tf)
{
    typename btree::btree_map<T,ProgramList*>::iterator it = (*TermIndex).find(term);
    if ((it == (*TermIndex).end()))
    {
        (*TermMutex)[term] = CMutex();
        termIndexMutex.Lock();//感觉基本上修改全部的，都需要用这个互斥锁
        (*TermIndex)[term]=new ProgramList;
        proSum++;
        termIndexMutex.Unlock();
        (*TermMutex)[term].Lock();
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
void IndexTemplate<T>::addAudioLive(AudioInfo &tmpInfo,btree::btree_map<T,double> &TermFreq,btree::btree_map<int, \
btree::btree_map<T, NodeInfo *> > &livePointer,CMutex &mutexLive)
{
    if(TermIndex==NULL)
    {
        TermIndex = new btree::btree_map<T,ProgramList*>;
        InfoTable = new btree::btree_map<int, AudioInfo>;
        TermMutex = new btree::btree_map<T,CMutex>;
    }
    I0MutexInfo.Lock();
    (*InfoTable)[tmpInfo.id] = tmpInfo;
    I0MutexInfo.Unlock();
    audioCount++;

    typename btree::btree_map<T, double>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        addLiveNode(it->first, tmpInfo.id, TermFreq[it->first], livePointer, tmpInfo.final, mutexLive);
    }


    typename btree::btree_map<T,NodeInfo*>::iterator itNode=livePointer[tmpInfo.id].begin();
    NodeInfo *tmp;
    if(tmpInfo.final>0) {
        for (; itNode != livePointer[tmpInfo.id].end(); itNode++) {
            if (itNode->second->flag == -1) {

                if((*TermIndex).find(itNode->first)==TermIndex->end())
                {
                    (*TermMutex)[itNode->first] = CMutex();
                    termIndexMutex.Lock();//感觉基本上修改全部的，都需要用这个互斥锁
                    (*TermIndex)[itNode->first]=new ProgramList;
                    termIndexMutex.Unlock();
                    (*TermMutex)[itNode->first].Lock();
                    tmp=(*TermIndex)[itNode->first]->addNode(itNode->second->tf,itNode->second->id);
                    (*TermMutex)[itNode->first].Unlock();
                    itNode->second->flag=0;
                    itNode->second=tmp;
                    itNode->second->flag=-1;
                }else{
                    (*TermMutex)[itNode->first].Lock();
                    tmp=(*TermIndex)[itNode->first]->addNode(itNode->second->tf,itNode->second->id);
                    (*TermMutex)[itNode->first].Unlock();
                    itNode->second->flag=0;
                    itNode->second=tmp;
                    itNode->second->flag=-1;
                }
            }else if(itNode->second->flag==-2333)
            {
                itNode->second->flag=-1;
            }
        }
    }else if(tmpInfo.final==0)
    {
        for (; itNode != livePointer[tmpInfo.id].end();) {
            T str=itNode->first;

            if (itNode->second->flag == -1) {
                if((*TermIndex).find(itNode->first)==TermIndex->end())
                {
                    termIndexMutex.Lock();
                    (*TermIndex)[itNode->first]= new ProgramList;
                    termIndexMutex.Unlock();

                    (*TermMutex)[itNode->first].Lock();
                    tmp=(*TermIndex)[itNode->first]->addNode(itNode->second->tf,itNode->second->id);
                    itNode->second->flag=0;
                    itNode->second=tmp;
                    itNode->second->flag=-1;
                    (*TermMutex)[itNode->first].Unlock();
                }else{
                    (*TermMutex)[itNode->first].Lock();
                    tmp=(*TermIndex)[itNode->first]->addNode(itNode->second->tf,itNode->second->id);
                    itNode->second->flag=0;
                    itNode->second=tmp;
                    itNode->second->flag=-1;
                    (*TermMutex)[itNode->first].Unlock();
                }
            }

            itNode=livePointer[itNode->second->id].erase(itNode);
        }
        mutexLive.Lock();
        livePointer.erase(tmpInfo.id);
        mutexLive.Unlock();
    }
}

template <class T>
void IndexTemplate<T>::addLiveNode(T term, int id, double tf, btree::btree_map<int, btree::btree_map<T, NodeInfo *> >\
                            &livePointer, int final, CMutex &mutexLive)
{
    typename btree::btree_map<T,ProgramList*>::iterator it = (*TermIndex).find(term);
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
    pthread_t pid1,pid2,pid3;
    btree::btree_map<T,ProgramList*> &other=*TermIndex;
    typename btree::btree_map<T,ProgramList*>::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {
        FamilyI0Sort<T> Fam1(it->second, this);
        pthread_create(&pid1,NULL,Index0SortThreadFre<T>,(void*)&Fam1);//有待商榷
        FamilyI0Sort<T> Fam2(it->second, this);
        pthread_create(&pid2,NULL,Index0SortThreadSim<T>,(void*)&Fam2);
        FamilyI0Sort<T> Fam3(it->second, this);
        pthread_create(&pid3,NULL,Index0SortThreadSig<T>,(void*)&Fam3);

        pthread_join(pid1,NULL);
        pthread_join(pid2,NULL);
        pthread_join(pid3,NULL);
    }
}

template <class T>
void IndexTemplate<T>::mergerIndex(IndexTemplate<T> &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
    level++;
    pthread_t pid[3];
    int ret;
    insertAndRemove();
    other.insertAndRemove();

    //将merge过来的infotable中的不重复的info放入该infotable中
    btree::btree_map<int,AudioInfo>::iterator it_info;
    for(it_info=other.InfoTable->begin();it_info!=other.InfoTable->end();it_info++)
    {
        if((*InfoTable).find(it_info->first)==(*InfoTable).end())
        {
            (*InfoTable)[it_info->first]=it_info->second;
        }
    }

    typename btree::btree_map<T,ProgramList*>::iterator itListI;
    typename btree::btree_map<T,ProgramList*>::iterator itListJ;//otherIndex的programlist


    btree::btree_map<int,NodeInfo*> ::iterator itNodeI;
    btree::btree_map<int,NodeInfo*> ::iterator itNodeJ;

    //将otherindex的节点复制到Index中
    for(itListJ=other.TermIndex->begin();itListJ!=other.TermIndex->end();itListJ++) {
        itListI = TermIndex->find(itListJ->first);

        if (itListI ==TermIndex->end()) {
            (*TermIndex)[itListJ->first] = new ProgramList(*itListJ->second);
        }else{
            for (itNodeJ=((*other.TermIndex)[itListJ->first]->nodeMap)->begin();\
            itNodeJ!=((*other.TermIndex)[itListJ->first]->nodeMap)->end();itNodeJ++)
            {
                btree::btree_map<int,NodeInfo*>&thu=*(*TermIndex)[itListJ->first]->nodeMap;
                itNodeI=(*TermIndex)[itListJ->first]->nodeMap->find(itNodeJ->second->id);
                if(itNodeI==(*TermIndex)[itListJ->first]->nodeMap->end())
                {
                    (*(*TermIndex)[itListJ->first]->nodeMap)[itNodeJ->second->id]=itNodeJ->second;//如果other的termindex中存在myself中没有的id，就拿过来
                }
            }
        }
    }


    double begin,end;
    begin=getTime();
    FamilyMerger<T> one(this,&other);
    ret=pthread_create(&pid[0],NULL,invertedIndexMergerThreadFre<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    ret=pthread_create(&pid[1],NULL,invertedIndexMergerThreadSig<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    ret=pthread_create(&pid[2],NULL,invertedIndexMergerThreadTermFreq<T>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    for (int i=0;i<3;i++)
    {
        pthread_join(pid[i],NULL);
    }
    end=getTime();
    MergeSortTime+=end-begin;
    audioCount=(*InfoTable).size();
    termCount+=other.termCount;

    //归并idIndex

    for(itListJ=other.TermIndex->begin();itListJ!=other.TermIndex->end();itListJ++) {
        delete itListJ->second->nodeMap;
        itListJ->second->nodeMap=NULL;
    }

    for(itListI=TermIndex->begin();itListI!=TermIndex->end();itListI++) {
        if(itListI->second->nodeMap->size()==0)
        {
            delete itListI->second;
            TermIndex->erase(itListI);
        }
    }

}

template <class T>
void IndexTemplate<T>::search(btree::btree_map<int, double> &result, double &minScore, int &ansNum, int &sum, \
        const vector<T> query,const vector<double> idfVec , btree::btree_map<int, string> &name,btree::btree_map<int,score_ratio> &sco_vec)
{
    double upFre = 0;
    double upSig = 0;
    int id1, id2, id3,id4;
    btree::btree_map<T, double> upSim;
    double score = 0;
    btree::btree_map<T,double> TermFreq;
    btree::btree_map<int, double>::iterator itRes;
    btree::btree_map<int,NodeInfo*>::iterator itTmpNode;//每个query中每个id获取全部tf时的迭代器


    if(level==0)
    {
        for (int i = 0; i < query.size(); i++)
        {
            if(TermIndex==NULL) break;
            typename btree::btree_map<T,ProgramList*>::iterator it = (*TermIndex).find(query[i]);
            btree::btree_map<int,NodeInfo*>::iterator itNode;
            if (it != (*TermIndex).end())
            {
                it->second->mutex.Lock();
                for(itNode=it->second->nodeMap->begin();itNode!=it->second->nodeMap->end();itNode++)
                {
                    btree::btree_map<int, double>::iterator it_res = result.find(itNode->second->id);
                    if (it_res == result.end())
                    {
                        if(itNode->second->flag==-1)
                        {
                            AudioInfo &infoTmp = (*InfoTable)[itNode->second->id];
                            for (int k = 0; k < query.size(); k++)
                            {
                                typename btree::btree_map<T,ProgramList*>::iterator itStr = (*TermIndex).find(query[i]);
                                if (itStr != TermIndex->end())
                                {
                                    btree::btree_map<int,NodeInfo*> &tmp=*itStr->second->nodeMap;
                                    itTmpNode = itStr->second->nodeMap->find(itNode->second->id);
                                    if (itTmpNode != itStr->second->nodeMap->end())
                                    {
                                        TermFreq[query[k]] = itTmpNode->second->tf;
                                    }
                                }
                            }
                            score = computeScore(infoTmp.time, infoTmp.score, TermFreq, infoTmp.TagsSum, query,idfVec,sco_vec,itNode->second->id);
                            TermFreq.clear();
                            sum += 1;
                            result[itNode->second->id] = score;
                        }
                    }
                }
                it->second->mutex.Unlock();
            }
        }
        vector<pair<int, double> > resVector(result.begin(), result.end());
        sort(resVector.begin(), resVector.end(), CompDedcendVal());
        result.clear();
        if (sum >= ansNum)
        {
            for (int i = 0; i < ansNum; i++)
            {
                result[resVector[i].first] = resVector[i].second;
                btree::btree_map<int, AudioInfo>::iterator it_name = (*InfoTable).find(resVector[i].first);
                if (it_name != (*InfoTable).end())
                {
                    name[resVector[i].first] = (*InfoTable)[resVector[i].first].title;
                }
            }
            minScore = resVector[ansNum - 1].second;
            sum = ansNum;
        }
        else if (resVector.size()>0)
        {
            for (int i = 0; i < resVector.size(); i++)
            {
                result[resVector[i].first] = resVector[i].second;
                btree::btree_map<int, AudioInfo>::iterator itName = (*InfoTable).find(resVector[i].first);
                if (itName != (*InfoTable).end())
                {
                    name[resVector[i].first] = (*InfoTable)[resVector[i].first].title;
                }
            }
            minScore = resVector[resVector.size() - 1].second;
            sum = resVector.size();
        }
    }
    else
    {
        if (sum < ansNum)
        {
            for (int i = 0; i < query.size(); i++)
            {
                btree::btree_map<int,AudioInfo> &tmp1=*InfoTable;
                typename btree::btree_map<T,ProgramList*>::iterator it = (*TermIndex).find(query[i]);
                if (it != (*TermIndex).end())
                {
                    it->second->mutex.Lock();
                    if(it->second->max_fresh!=NULL)
                    {
                        NodeInfo *searchPointer=(it->second->max_fresh);
                        btree::btree_map<int,NodeInfo*> &tmp2=*it->second->nodeMap;

                        for (; searchPointer!=NULL; searchPointer=searchPointer->next_fresh)
                        {
                            itRes = result.find(searchPointer->id);
                            if (itRes == result.end())
                            {
                                if(searchPointer->flag==-1) {
                                    btree::btree_map<int, AudioInfo> &tmp3 = *InfoTable;

                                    AudioInfo &info_tmp = (*InfoTable)[searchPointer->id];

                                    for (int k = 0; k < query.size(); k++) {
                                        typename btree::btree_map<T,ProgramList*>::iterator it_str = (*TermIndex).find(query[k]);
                                        if (it_str != TermIndex->end()) {
                                            itTmpNode = it_str->second->nodeMap->find(searchPointer->id);
                                            if (itTmpNode != it_str->second->nodeMap->end()) {
                                                TermFreq[query[k]] = itTmpNode->second->tf;
                                            }
                                        }
                                    }
                                    score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idfVec,sco_vec,searchPointer->id);
                                    TermFreq.clear();
                                    sum += 1;
                                    result[searchPointer->id] = score;
                                    if (sum >= ansNum)
                                        break;
                                }
                            }
                        }
                    }
                    it->second->mutex.Unlock();
                }
            }

            vector<pair<int, double> > resVector(result.begin(), result.end());
            sort(resVector.begin(), resVector.end(), CompDedcendVal());
            result.clear();
            if (sum >= ansNum)
            {
                for (int i = 0; i < ansNum; i++)
                {
                    result[resVector[i].first] = resVector[i].second;
                    btree::btree_map<int, AudioInfo>::iterator it_name = (*InfoTable).find(resVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[resVector[i].first] = (*InfoTable)[resVector[i].first].title;
                    }
                }
                minScore = resVector[ansNum - 1].second;
                sum = ansNum;
            }
            else if (resVector.size()>0)
            {
                for (int i = 0; i < resVector.size(); i++)
                {
                    result[resVector[i].first] = resVector[i].second;
                    btree::btree_map<int, AudioInfo>::iterator it_name = (*InfoTable).find(resVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[resVector[i].first] = (*InfoTable)[resVector[i].first].title;
                    }
                }

                minScore = resVector[resVector.size() - 1].second;
                sum = resVector.size();
            }
        }

        if (sum >= ansNum)
        {
            bool TerFlag = false;
            queue<NodeInfo*> pointerQue;
            for (int i = 0; i < (*TermIndex).size(); i++)
            {
                if (TerFlag)
                    break;
                upFre = 0;
                upSig = 0;

                for (int j = 0; j < query.size(); j++)
                {
                    upSim[query[j]] = 0;
                }

                NodeInfo* tmp_pointer;
                priority_queue<Sig> copyBuffer(updateBuffer);

                try
                {
                    for (int j = 0; j < query.size(); j++)
                    {
                        typename btree::btree_map<T,ProgramList*>::iterator itExist = (*TermIndex).find(query[j]);

                        if (itExist != (*TermIndex).end())
                        {
                            itExist->second->mutex.Lock();
                            int leng=itExist->second->nodeMap->size();//长度不一定了，如果有提前结束的链，就先把null放进去
                            if (i < leng)
                            {
                                //fre模块
                                if(i==0)
                                {
                                    tmp_pointer=itExist->second->max_fresh;
                                }
                                else
                                {
                                    tmp_pointer=pointerQue.front();
                                    pointerQue.pop();
                                }

                                if(tmp_pointer!=NULL)
                                {
                                    id1 = (*tmp_pointer).id;
                                    itRes = result.find(id1);
                                    if (itRes == result.end())
                                    {
                                        AudioInfo &info_tmp = (*InfoTable)[id1];
                                        if (tmp_pointer->flag==-1)
                                        {


                                            for (int k=0;k<query.size();k++)//因为我已经判断Result，然后
                                            {
                                                typename btree::btree_map<T,ProgramList*>::iterator it_str = (*TermIndex).find(query[k]);
                                                if(it_str!=TermIndex->end())
                                                {
                                                    btree::btree_map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id1);
                                                    if(it_tmp_node!=it_str->second->nodeMap->end())
                                                    {
                                                        TermFreq[query[k]]=it_tmp_node->second->tf;
                                                    }
                                                }

                                            }
                                            score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idfVec,sco_vec,id1);
                                            TermFreq.clear();
                                            if (score > minScore)
                                            {
                                                result[id1] = score;
                                                minScore = score;
                                                sum += 1;
                                            }
                                        }
                                        if (upFre < info_tmp.time)
                                        {
                                            upFre = info_tmp.time;
                                        }
                                    }
                                    if(tmp_pointer->next_fresh!=NULL)
                                    {
                                        pointerQue.push(tmp_pointer->next_fresh);
                                    }else {
                                        pointerQue.push(NULL);
                                    }
                                }else{
                                    pointerQue.push(NULL);
                                }
                            }

                            if (i < leng)//迟早要删，先写后面的
                            {
                                if(i==0)
                                {
                                    tmp_pointer=itExist->second->max_sig;
                                }
                                else
                                {
                                    tmp_pointer=pointerQue.front();
                                    pointerQue.pop();
                                }

                                if(tmp_pointer!=NULL)
                                {
                                    id2 = (*tmp_pointer).id;
                                    itRes = result.find(id2);
                                    if (itRes == result.end())
                                    {
                                        AudioInfo &info_tmp = (*InfoTable)[id2];
                                        if (tmp_pointer->flag==-1)
                                        {


                                            for (int k=0;k<query.size();k++)
                                            {
                                                typename btree::btree_map<T,ProgramList*>::iterator it_str = (*TermIndex).find(query[k]);
                                                if(it_str!=TermIndex->end())
                                                {
                                                    btree::btree_map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id2);
                                                    if(it_tmp_node!=it_str->second->nodeMap->end())
                                                    {
                                                        TermFreq[query[k]]=it_tmp_node->second->tf;
                                                    }
                                                }
                                            }
                                            score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idfVec,sco_vec,id2);
                                            TermFreq.clear();
                                            if (score > minScore)
                                            {
                                                result[id2] = score;
                                                minScore = score;
                                                sum += 1;
                                            }
                                        }
                                        if (upSig < info_tmp.score)
                                        {
                                            upSig = info_tmp.score;
                                        }
                                    }
                                    if(tmp_pointer->next_sig!=NULL)
                                    {
                                        pointerQue.push(tmp_pointer->next_sig);
                                    } else{
                                        pointerQue.push(NULL);
                                    }
                                }else {
                                    pointerQue.push(NULL);
                                }
                            }

                            if (i < leng)
                            {
                                if(i==0)
                                {
                                    tmp_pointer=itExist->second->max_termFreq;
                                }
                                else
                                {
                                    tmp_pointer=pointerQue.front();
                                    pointerQue.pop();
                                }

                                if(tmp_pointer!=NULL)
                                {
                                    id3 = (*tmp_pointer).id;
                                    itRes = result.find(id3);
                                    if (itRes == result.end())
                                    {
                                        if (tmp_pointer->flag==-1)
                                        {
                                            AudioInfo &info_tmp = (*InfoTable)[id3];
                                            for (int k=0;k<query.size();k++)
                                            {
                                                typename btree::btree_map<T,ProgramList*>::iterator it_str = (*TermIndex).find(query[k]);
                                                if(it_str!=TermIndex->end())
                                                {
                                                    btree::btree_map<int,NodeInfo*>::iterator it_tmp_node=it_str->second->nodeMap->find(id3);
                                                    if(it_tmp_node!=it_str->second->nodeMap->end())
                                                    {
                                                        TermFreq[query[k]]=it_tmp_node->second->tf;
                                                    }
                                                }
                                            }
                                            score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idfVec,sco_vec,id3);
                                            if (score > minScore)
                                            {
                                                result[id3] = score;
                                                minScore = score;
                                                sum += 1;
                                            }
                                        }
                                        if (upSim[query[j]] < tmp_pointer->tf)
                                        {
                                            upSim[query[j]] = tmp_pointer->tf;
                                        }

                                    }
                                    if(tmp_pointer->next_termFreq!=NULL)
                                    {
                                        pointerQue.push(tmp_pointer->next_termFreq);
                                    } else{
                                        pointerQue.push(NULL);
                                    }
                                } else{
                                    pointerQue.push(NULL);
                                }
                            }
                            else
                            {
                                upSim[query[j]] = 0;
                            }

                            if(copyBuffer.size()>0) {
                                while (upSig < copyBuffer.top().get_score()) {
                                    id4 = (copyBuffer.top().get_id());
                                    copyBuffer.pop();
                                    itRes = result.find(id4);
                                    if (itRes == result.end()) {
                                        AudioInfo &info_tmp = (*InfoTable)[id4];
                                        for (int k = 0; k < query.size(); k++) {
                                            typename btree::btree_map<T,ProgramList*>::iterator it_str = (*TermIndex).find(
                                                    query[k]);
                                            if (it_str != TermIndex->end()) {
                                                btree::btree_map<int,NodeInfo*>::iterator it_tmp_node = it_str->second->nodeMap->find(
                                                        id4);
                                                if (it_tmp_node != it_str->second->nodeMap->end()) {
                                                    TermFreq[query[k]] = it_tmp_node->second->tf;
                                                }
                                            }

                                        }
                                        score = computeScore(info_tmp.time, info_tmp.score, TermFreq, info_tmp.TagsSum, query,idfVec,sco_vec,id4);
                                        TermFreq.clear();
                                        if (score > minScore) {
                                            result[id4] = score;
                                            minScore = score;
                                            sum += 1;
                                        }
                                    }
                                }
                            }
                            itExist->second->mutex.Unlock();
                        }
                    }

                    try//你也可以放到与i的循环同一级，我觉得问题不大
                    {
                        if (computeScore(upFre, upSig, upSim, 0, query,idfVec) < minScore)
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
            vector<pair<int, double> > ResVector(result.begin(), result.end());
            sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
            result.clear();
            if (sum >= ansNum)
            {
                for (int i = 0; i < ansNum; i++)
                {
                    result[ResVector[i].first] = ResVector[i].second;
                    btree::btree_map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }
                minScore = ResVector[ansNum - 1].second;
                sum = ansNum;
            }
            else
            {
                for (int i = 0; i < ResVector.size(); i++)
                {
                    result[ResVector[i].first] = ResVector[i].second;
                    btree::btree_map<int, AudioInfo>::iterator it_name = (*InfoTable).find(ResVector[i].first);
                    if (it_name != (*InfoTable).end())
                    {
                        name[ResVector[i].first] = (*InfoTable)[ResVector[i].first].title;
                    }
                }
                minScore = ResVector[-1].second;
                sum = ResVector.size();
            }

        }
    }
}

template <class T>
double IndexTemplate<T>::computeScore(const double &time, const double &score, btree::btree_map<T, double> &TermFreq, const int &tagsSum,
                                   const vector<T> &query, const vector<double> &idf_vec)
{
    double fre = (pow(2, (time - getTime())));
    if(fre>=1)
    {
        fre=1;
    }else if(fre<0)
    {
        fre=0;
    }

    double sig = log(score / 1000000 + 1);
    if(sig>1)
    {
        sig=1;
    }else if(sig>0)
    {
        sig = sqrt(sig);
    }else
    {
        sig=0;
    }

    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        auto it = TermFreq.find(query[i]);
        if (it != TermFreq.end())
        {
            if (tagsSum != 0)
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
            else
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
        }
    }
    if(sim>1)
    {
        sim=1;
    }else
    {
        sim=sqrt(sim);
    }
    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
    return ScoreAll;
}

template <class T>
double IndexTemplate<T>::computeScore(const double &time, const double &score, btree::btree_map<T, double> &TermFreq, const int &tagsSum,
                                   const vector<T> &query, const vector<double> &idf_vec,btree::btree_map<int,score_ratio> &sco_vec,int id)
{
    double fre = (pow(2, (time - getTime())));
    if(fre>=1)
    {
        fre=1;
    }else if(fre<0)
    {
        fre=0;
    }

    double sig = log(score / 1000000 + 1);
    if(sig>1)
    {
        sig=1;
    }else if(sig>0)
    {
        sig = sqrt(sig);
    }else
    {
        sig=0;
    }

    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        auto it = TermFreq.find(query[i]);
        if (it != TermFreq.end())
        {
            if (tagsSum != 0)
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
            else
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
        }
    }
    if(sim>1)
    {
        sim=1;
    }else
    {
        sim=sqrt(sim);
    }
    sco_vec[id]=score_ratio(fre,sig,sim);
    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
    return ScoreAll;
}

#endif //HASH_0E_INDEXTEMPLATE_H
