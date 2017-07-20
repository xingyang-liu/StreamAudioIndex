//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "Ii.h"


Ii::Ii()
{
    AudioCount = 0;
    level = 0;
    TermIndex = NULL;
    TermMutex=NULL;
    InfoTable = NULL;
}

void Ii::addAudio(AudioInfo &tmp_info,map<string,int> &TagsNum)
{
    if(TermIndex==NULL)
    {
//        TermIndexFre = new map<string, std::list<int> >;
//        TermIndexSig = new map<string, std::list<int> >;
//        TermIndexSim = new map<string, std::list<TermFreq> >;
        TermIndex =new map<string,ListInfo>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<string,CMutex>;
    }

    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    map<string, int>::iterator it;
    for (it = TagsNum.begin(); it != TagsNum.end(); it++)
    {
        term_add(it->first, tmp_info.id, TagsNum[it->first]);
    }
    //(*InfoTable) = (*InfoTable).insert(make_pair<int, AudioInfo>);

//    if (level==0&&AudioCount == IndexUnit)
//    {
//        I0_sort();
//    }
}

void Ii::term_add(string term, int id, int tf)
{
    map<string, ListInfo >::iterator it = (*TermIndex).find(term);
    if ((it == (*TermIndex).end()))
    {
        (*TermMutex)[term] = CMutex();
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]=ListInfo();
        (*TermIndex)[term].add_id(tf,id);
        (*TermMutex)[term].Unlock();
    }
    else
    {
        (*TermMutex)[term].Lock();
        (*TermIndex)[term].add_id(tf,id);
        (*TermMutex)[term].Unlock();
    }
}

void Ii::addAudioLive(AudioInfo &tmp_info,map<string,int> &TagsNum,map<int, map<string, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive)
{
    if(TermIndex==NULL)
    {
        TermIndex =new map<string,ListInfo>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<string,CMutex>;
    }
    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    map<string, int>::iterator it;
    for (it = TagsNum.begin(); it != TagsNum.end(); it++)
    {
        term_addLive(it->first, tmp_info.id, TagsNum[it->first],livePointer,tmp_info.final,mutexLive);
    }

//    map<string,ListInfo> &tmp1=*TermIndex;

    map<string,NodeInfo*>::iterator it_node=livePointer[tmp_info.id].begin();
    NodeInfo *tmp;
    if(tmp_info.final>0) {
        for (; it_node != livePointer[tmp_info.id].end(); it_node++) {
            if (it_node->second->flag == -1) {
                (*TermMutex)[it_node->first].Lock();
                (*TermIndex)[it_node->first]=ListInfo();
                tmp=(*TermIndex)[it_node->first].add_id(it_node->second->tf,it_node->second->id);
                it_node->second->flag=0;
                it_node->second=tmp;
                (*TermMutex)[it_node->first].Unlock();
            }else if(it_node->second->flag==-2333)
            {
                it_node->second->flag=-1;
            }
        }
    }else if(tmp_info.final==0)
    {
        for (; it_node != livePointer[tmp_info.id].end(); it_node++) {
            it_node->second->flag=0;
        }
    }
}

void Ii::term_addLive(string term, int id, int tf, map<int, map<string, NodeInfo*> > &livePointer,int final,CMutex &mutexLive)
{
    map<string, ListInfo >::iterator it = (*TermIndex).find(term);
    NodeInfo *tmp;
    if ((it == (*TermIndex).end()))
    {

        (*TermMutex)[term] = CMutex();
        (*TermMutex)[term].Lock();
        (*TermIndex)[term]=ListInfo();
        if(livePointer[id].find(term)==livePointer[id].end())
        {
            tmp=(*TermIndex)[term].add_id(tf,id);
        }
        else
        {
            tmp=(*TermIndex)[term].add_id(tf+(*(livePointer[id][term])).tf,id);
        }
        (*TermMutex)[term].Unlock();
    }
    else
    {
        (*TermMutex)[term].Lock();
        if(livePointer[id].find(term)==livePointer[id].end())
        {
            tmp=(*TermIndex)[term].add_id(tf,id);
        }
        else
        {
            tmp=(*TermIndex)[term].add_id(tf+(*(livePointer[id][term])).tf,id);
        }
        (*TermMutex)[term].Unlock();
    }

    if(final >0)
    {
        if(livePointer[id].find(term)!=livePointer[id].end()) livePointer[id][term]->flag=-2333;
        livePointer[id][term]=tmp;
    }
    else
    {
        if(livePointer[id].find(term)!=livePointer[id].end()) livePointer[id][term]->flag=0;
        mutexLive.Lock();
        livePointer.erase(id);
        mutexLive.Unlock();

    }
}

void Ii::I0_sort()
{
    vector<int> id_list;
    map<int,NodeInfo*>::iterator it_id;
    NodeInfo *pointer;
    pthread_t pid1,pid2,pid3;

    map<string,ListInfo>::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {

        for(it_id=((it->second).nodeMap)->begin();it_id!=((it->second).nodeMap)->end();it_id++)
        {
            id_list.push_back(it_id->first);
        }
        if(id_list.size()!=0)
        {
            FamilyI0Sort Fam1(this,id_list,1,&it);
            pthread_create(&pid1,NULL,I0SortThread,(void*)&Fam1);
            FamilyI0Sort Fam2(this,id_list,2,&it);
            pthread_create(&pid2,NULL,I0SortThread,(void*)&Fam2);
            FamilyI0Sort Fam3(this,id_list,3,&it);
            pthread_create(&pid3,NULL,I0SortThread,(void*)&Fam3);

            pthread_join(pid1,NULL);
            pthread_join(pid2,NULL);
            pthread_join(pid3,NULL);

            id_list.clear();
        }
    }

}

void Ii::output()
{
    cout << "I" << level << "_count:" << AudioCount << "\n";
}

int Ii::get_count() { return AudioCount; }

void Ii::MergerIndex(Ii &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
    int length;
    level++;
//    AudioCount += other.AudioCount;

    //merger info_table
    pthread_t pid[3];
    int ret;

    set<int>::iterator it_set;
//    map<int,AudioInfo> &tmp=(*InfoTable);
    for (it_set=RemovedId.begin();it_set!=RemovedId.end();it_set++)
    {

        (*InfoTable).erase(*it_set);
    }


    for (it_set=other.RemovedId.begin();it_set!=other.RemovedId.end();it_set++)
    {

        (*other.InfoTable).erase(*it_set);
        RemovedId.insert(*it_set);
    }

    map<string,ListInfo> &tmp1=*TermIndex;
    map<string,ListInfo> &tmp2=*other.TermIndex;

    map<int,AudioInfo>::iterator it_info;
    for(it_info=other.InfoTable->begin();it_info!=other.InfoTable->end();it_info++)
    {
        if((*InfoTable).find(it_info->first)==(*InfoTable).end())
        {
            (*InfoTable)[it_info->first]=it_info->second;
        }
    }


    map<string,ListInfo>::iterator it_list_i;
    map<string,ListInfo>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;
    map<int,NodeInfo*> ::iterator it_node_tmp;

    for(it_list_j=other.TermIndex->begin();it_list_j!=other.TermIndex->end();it_list_j++) {
        it_list_i = TermIndex->find(it_list_j->first);
        if (it_list_i ==TermIndex->end()) {
            (*TermIndex)[it_list_j->first] = ListInfo();
            for (it_node_tmp=((*other.TermIndex)[it_list_j->first].nodeMap)->begin();\
            it_node_tmp!=((*other.TermIndex)[it_list_j->first].nodeMap)->end();it_node_tmp++)
            {
                (*TermIndex)[it_list_j->first].add_id(it_node_tmp->second->tf,it_node_tmp->second->id);
            }
        }else{
            for (it_node_tmp=((*other.TermIndex)[it_list_j->first].nodeMap)->begin();\
            it_node_tmp!=((*other.TermIndex)[it_list_j->first].nodeMap)->end();it_node_tmp++)
            {
                (*TermIndex)[it_list_j->first].id2pointer(it_node_tmp->second->tf,it_node_tmp->second->id);
            }
        }
    }





//    map<int,AudioInfo> &tmp=*InfoTable;



    FamilyMerger one(this,&other);
    ret=pthread_create(&pid[0],NULL,IiMergerThreadFre,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }



    ret=pthread_create(&pid[2],NULL,IiMergerThreadSig,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    ret=pthread_create(&pid[1],NULL,IiMergerThreadTermFreq,(void*)&one);
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
        for(it_node_i=it_list_i->second.nodeMap->begin();it_node_i!=it_list_i->second.nodeMap->end();it_node_i++)
        {
            if(it_node_i->second->flag==0)
            {
                delete it_node_i->second;
                it_list_i->second.nodeMap->erase(it_node_i);
            }
        }
    }
    


    RemovedId.clear();
//
//
//
//
//    //重排序
//
//    I0_sort();


}

void Ii::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name)
{
    double up_fre = 0;
    double up_sig = 0;
    int id1, id2, id3;
    map<string, int> up_sim;
    double score = 0;
    map<string,int> TagsNum;
    map<int, double>::iterator it_res;
    map<int, NodeInfo *>::iterator it_tmp_node;//每个query中每个id获取全部tf时的迭代器
    ofstream out_res("Ii_Result.txt", ofstream::app);

    if(level==0)
    {
        for (int i = 0; i < query.size(); i++)
        {
            if(TermIndex==NULL) break;
            map<string, ListInfo>::iterator it = (*TermIndex).find(query[i]);
            map<int,NodeInfo*>::iterator it_node;
            if (it != (*TermIndex).end())
            {
                for(it_node=it->second.nodeMap->begin();it_node!=it->second.nodeMap->end();it_node++)
                {
                    map<int, double>::iterator it_res = Result.find(it_node->second->id);
                    if (it_res == Result.end())
                    {
                        AudioInfo &info_tmp = (*InfoTable)[it_node->second->id];
                        for (int k = 0; k < query.size(); k++)
                        {
                            map<string, ListInfo>::iterator it_str = (*TermIndex).find(query[i]);
                            if (it_str != TermIndex->end())
                            {
                                map<int,NodeInfo*> &tmp=*it_str->second.nodeMap;
                                NodeInfo *&tmp2=it_node->second;

                                it_tmp_node = it_str->second.nodeMap->find(it_node->second->id);
                                if (it_tmp_node != it_str->second.nodeMap->end())
                                {
                                    TagsNum[query[k]] = it_tmp_node->second->tf;
                                }
                            }

                        }
                        score = computeScore(info_tmp.time, info_tmp.score, TagsNum, info_tmp.TagsSum, query);
                        TagsNum.clear();
                        Sum += 1;
                        Result[it_node->second->id] = score;
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

                map<string, ListInfo >::iterator it = (*TermIndex).find(query[i]);
                if (it != (*TermIndex).end())
                {
                    if(it->second.max_fresh!=NULL)
                    {
                        NodeInfo *searchPointer=(it->second.max_fresh);
                        map<int,NodeInfo*> &tmp2=*it->second.nodeMap;

                        for (; searchPointer!=NULL; searchPointer=searchPointer->next_fresh)
                        {
                            it_res = Result.find(searchPointer->id);
                            if (it_res == Result.end())
                            {
                                map<int,AudioInfo> &tmp3=*InfoTable;

                                AudioInfo &info_tmp = (*InfoTable)[searchPointer->id];

                                for (int k=0;k<query.size();k++)
                                {
                                    map<string, ListInfo >::iterator it_str = (*TermIndex).find(query[k]);
                                    if(it_str!=TermIndex->end())
                                    {
                                        it_tmp_node=it_str->second.nodeMap->find(searchPointer->id);
                                        if(it_tmp_node!=it_str->second.nodeMap->end())
                                        {
                                            TagsNum[query[k]]=it_tmp_node->second->tf;
                                        }
                                    }

                                }
                                score = computeScore(info_tmp.time, info_tmp.score, TagsNum, info_tmp.TagsSum, query);
                                TagsNum.clear();
                                Sum += 1;
                                Result[searchPointer->id] = score;
                                if (Sum >= AnsNum)
                                    break;
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

                queue<NodeInfo*> pointer_que;
                NodeInfo* tmp_pointer;

                try
                {
                    for (int j = 0; j < query.size(); j++)
                    {
                        map<string, ListInfo >::iterator it_exist = (*TermIndex).find(query[j]);
                        ListInfo &other_exi=it_exist->second;
                        if (it_exist != (*TermIndex).end())
                        {
                            if (i < it_exist->second.nodeMap->size())
                            {
                                //fre模块
                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second.max_fresh;
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

                                    for (int k=0;k<query.size();k++)//日后最好判断一下一个东西是否已经判断，用一个flag即可（好像也不太对）
                                    {
                                        map<string, ListInfo >::iterator it_str = (*TermIndex).find(query[k]);
                                        if(it_str!=TermIndex->end())
                                        {
                                            map<int,NodeInfo*>::iterator it_tmp_node=it_str->second.nodeMap->find(id1);
                                            if(it_tmp_node!=it_str->second.nodeMap->end())
                                            {
                                                TagsNum[query[k]]=it_tmp_node->second->tf;
                                            }
                                        }

                                    }
                                    score = computeScore(info_tmp.time, info_tmp.score, TagsNum, info_tmp.TagsSum,
                                                         query);
                                    TagsNum.clear();
                                    if (score > MinScore)
                                    {
                                        Result[id1] = score;
                                        MinScore = score;
                                        Sum += 1;
                                    }
                                    if (up_fre < info_tmp.time)
                                    {
                                        up_fre = info_tmp.time;
                                    }

                                    if(tmp_pointer->next_fresh!=NULL)
                                    {
                                        pointer_que.push(tmp_pointer->next_fresh);
                                    }
                                }

                            }


                            if (i < it_exist->second.nodeMap->size())//迟早要删，先写后面的
                            {

                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second.max_sig;
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

                                    for (int k=0;k<query.size();k++)
                                    {
                                        map<string, ListInfo >::iterator it_str = (*TermIndex).find(query[k]);
                                        if(it_str!=TermIndex->end())
                                        {
                                            map<int,NodeInfo*>::iterator it_tmp_node=it_str->second.nodeMap->find(id2);
                                            if(it_tmp_node!=it_str->second.nodeMap->end())
                                            {
                                                TagsNum[query[k]]=it_tmp_node->second->tf;
                                            }
                                        }

                                    }
                                    score = computeScore(info_tmp.time, info_tmp.score, TagsNum, info_tmp.TagsSum,
                                                         query);
                                    TagsNum.clear();
                                    if (score > MinScore)
                                    {
                                        Result[id2] = score;
                                        MinScore = score;
                                        Sum += 1;
                                    }
                                    if (up_sig < info_tmp.score)
                                    {
                                        up_sig = info_tmp.score;
                                    }

                                    if(tmp_pointer->next_sig!=NULL)
                                    {
                                        pointer_que.push(tmp_pointer->next_sig);
                                    }
                                }
                            }


                            if (i < it_exist->second.nodeMap->size())
                            {
                                if(i==0)
                                {
                                    tmp_pointer=it_exist->second.max_termFreq;
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
                                    if(i==0)
                                    {
                                        tmp_pointer=it_exist->second.max_termFreq;
                                    }
                                    else
                                    {
                                        tmp_pointer=pointer_que.front();
                                        pointer_que.pop();
                                    }

                                    for (int k=0;k<query.size();k++)
                                    {
                                        map<string, ListInfo >::iterator it_str = (*TermIndex).find(query[k]);
                                        if(it_str!=TermIndex->end())
                                        {
                                            map<int,NodeInfo*>::iterator it_tmp_node=it_str->second.nodeMap->find(id3);
                                            if(it_tmp_node!=it_str->second.nodeMap->end())
                                            {
                                                TagsNum[query[k]]=it_tmp_node->second->tf;
                                            }
                                        }

                                    }


                                    AudioInfo &info_tmp = (*InfoTable)[id3];
                                    score = computeScore(info_tmp.time, info_tmp.score, TagsNum, info_tmp.TagsSum,
                                                         query);
                                    if (score > MinScore)
                                    {
                                        Result[id3] = score;
                                        MinScore = score;
                                        Sum += 1;
                                    }

                                    if (up_sim[query[j]] < tmp_pointer->tf)
                                    {
                                        up_sim[query[j]] = tmp_pointer->tf;
                                    }

                                    if(tmp_pointer->next_termFreq!=NULL)
                                    {
                                        pointer_que.push(tmp_pointer->next_termFreq);
                                    }
                                }
                            }
                            else
                            {
                                up_sim[query[j]] = 0;
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
//                    cout << "computeScore() with up has something wrong." << endl;
                    }
                }
                catch (...)
                {
//                cout << "Search with up has something wrong." << endl;
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

//void *MergerIndexThreadSim(void *fam)//现在只用于Sim
//{
//    FamilyIi * ones=(FamilyIi *)fam;
//    Ii *myself=ones->me;
//    Ii *other=ones->him;
//
//
//    map<string, std::list<TermFreq> >::iterator it_i=myself->TermIndexSim;
//    map<string, std::list<TermFreq> >::iterator it_j;
//
//
//    typename map<string, std::list<T> >::iterator it_T;
//    for (it_T = (*(other->getPosting(argTmp))).begin(); it_T != (*(other->getPosting(argTmp))).end(); it_T++)
//    {
//
//
//        int length = (*(myself->getPosting(argTmp)))[it_T->first].size();
//        (*(myself->getPosting(argTmp)))[it_T->first].resize(length + it_T->second.size());
//        copy(it_T->second.begin(), it_T->second.end(), (*(myself->getPosting(argTmp)))[it_T->first].begin() + length);
//        inplace_merge(((*(myself->getPosting(argTmp)))[it_T->first]).begin(), \
//		(*(myself->getPosting(argTmp)))[it_T->first].begin() + length,\
//		 ((*(myself->getPosting(argTmp)))[it_T->first]).end());
//
//        list<TermFreq>::iterator it_tf;
//        for (it_tf=(*(myself->getPosting(argTmp))).begin();it_tf!=(*(myself->getPosting(argTmp)));it_tf)
//        (*(myself->getPosting(argTmp)))
//    }
//}
//
//
//void *MergerIndexThreadFre(void *fam)
//{
//    FamilyIi * ones=(FamilyIi *)fam;
//    Ii *myself=ones->me;
//    Ii *other=ones->him;
//
//    std::list<int>::iterator it_i;
//    std::list<int>::iterator it_j;
//    map<string, AudioInfo >::iterator it_T;
//    for (it_T=other->TermIndex->begin();it_T!=other->TermIndex->end();it_T++)
//    {
//        it_j=it_T->second.begin();
//        it_i=myself->TermIndexFre[it_T->first].begin();
//        while(it_i!=myself->TermIndexFre[it_T->first].end()&&it_j!=it_T->second.end())
//        {
//            if((myself->InfoTable)[*it_i>*it_j)
//        }
//
//    }
//
//    for (it_T = (*(other->TermIndexFre)).begin(); it_T != (*(other->TermIndexFre)).end(); it_T++)
//    {
//        int length = (*(myself->TermIndexFre))[it_T->first].size();
//        (*(myself->TermIndexFre))[it_T->first].resize(length + it_T->second.size());
//        copy(it_T->second.begin(), it_T->second.end(), (*(myself->TermIndexFre))[it_T->first].begin() + length);
//        inplace_merge(((*(myself->TermIndexFre))[it_T->first]).begin(), \
//		(*(myself->TermIndexFre))[it_T->first].begin() + length,\
//		 ((*(myself->TermIndexFre))[it_T->first]).end(),CmpForFre(it->first));
//    }
//}

void *I0SortThread(void *fam)
{
    FamilyI0Sort &Fam=*(FamilyI0Sort *)fam;
    Ii *myself=Fam.me;
    vector<int> &id_list=Fam.id_list;
    int flag=Fam.flag;
    map<string,ListInfo>::iterator &it=*Fam.it_list;



    if (flag==1)
    {
        sort(id_list.begin(),id_list.end(),CmpForFre(it->first,myself));
        it->second.max_fresh=(*((it->second).nodeMap))[id_list[0]];
        NodeInfo *pointer=it->second.max_fresh;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {
                
                (*pointer).next_fresh=(*(it->second).nodeMap)[id_list[i]];
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
        sort(id_list.begin(),id_list.end(),CmpForSig(it->first,myself));
        it->second.max_sig=(*(it->second).nodeMap)[id_list[0]];
        NodeInfo *pointer=it->second.max_sig;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {

                (*pointer).next_sig=(*(it->second).nodeMap)[id_list[i]];
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
        sort(id_list.begin(),id_list.end(),CmpForSim(it->first,myself));
        it->second.max_termFreq=(*((it->second).nodeMap))[id_list[0]];
        NodeInfo *pointer=it->second.max_termFreq;
        if(id_list.size()>1)
        {
            for (int i=1;i<id_list.size();i++)
            {

                (*pointer).next_termFreq=(*(it->second).nodeMap)[id_list[i]];
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


void *IiMergerThreadFre(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    Ii *myself=ones->me;
    Ii *other=ones->him;

    map<string,ListInfo>::iterator it_list_i;
    map<string,ListInfo>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    int count=0;
    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {

        pointer_j=it_list_j->second.max_fresh;

        map<string,ListInfo> &tmp=*(myself->TermIndex);
        map<string,ListInfo> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second.max_fresh;

        //提前看一眼pointer_j是不是要清空
        if (it_list_j->second.max_fresh->flag != 0) {
            pointer_j = it_list_j->second.max_fresh;
        } else {
            pointer_j = it_list_j->second.max_fresh;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_fresh;
            }
        }

        if(it_list_i->second.max_fresh==NULL)
        {
           if(pointer_j!=NULL) {
               (*myself->TermIndex)[it_list_j->first].max_fresh = (*myself->TermIndex)[it_list_j->first].id2pointer(
                       pointer_j->tf, pointer_j->id);
               pointer_j = pointer_j->next_fresh;

               tmp_node=(*myself->TermIndex)[it_list_j->first].max_fresh;

               while(pointer_j!=NULL)
               {
                   tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                   tmp_node=tmp_node;
                   pointer_j=pointer_j->next_fresh;
                   while (pointer_j != NULL && pointer_j->flag == 0) {
                       pointer_j = pointer_j->next_fresh;
                   }
               }
               tmp_node=NULL;
           }
           else
           {
               (*myself->TermIndex)[it_list_j->first].max_fresh=NULL;
           }
        }
        else if(pointer_j!=NULL){
            //看一眼pointer_i是不是也要被清空
            if (it_list_i->second.max_fresh->flag != 0) {
                pointer_i = it_list_i->second.max_fresh;
            } else {
                pointer_i = it_list_i->second.max_fresh;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_fresh;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first].max_fresh=NULL;
            }
            else
            {
                NodeInfo &tmp4=*it_list_j->second.max_fresh;


                if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                {
                    (*myself->TermIndex)[it_list_j->first].max_fresh=pointer_i;
                    pointer_i=pointer_i->next_fresh;
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first].max_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_fresh;
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first].max_fresh;

                while(pointer_i!=NULL&&pointer_j!=NULL)
                {
                    if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                    {
                        tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);//同理
                        tmp_node=tmp_node->next_fresh;
                        pointer_i=pointer_i->next_fresh;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_fresh;
                        }
                    }
                    else
                    {
                        tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_fresh;
                        pointer_j=pointer_j->next_fresh;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_fresh;
                        }
                    }
                }

                while(pointer_i!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);//同理
                    tmp_node=tmp_node->next_fresh;
                    pointer_i=pointer_i->next_fresh;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_fresh;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_fresh;
                    pointer_j=pointer_j->next_fresh;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_fresh;
                    }
                }

                tmp_node->next_fresh=NULL;
            }
        }
    }

}

void *IiMergerThreadSig(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    Ii *myself=ones->me;
    Ii *other=ones->him;

    map<string,ListInfo>::iterator it_list_i;
    map<string,ListInfo>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {

        pointer_j=it_list_j->second.max_sig;

        map<string,ListInfo> &tmp=*(myself->TermIndex);
        map<string,ListInfo> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second.max_sig;

        //提前看一眼pointer_j是不是要清空
        if (it_list_j->second.max_sig->flag != 0) {
            pointer_j = it_list_j->second.max_sig;
        } else {
            pointer_j = it_list_j->second.max_sig;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_sig;
            }
        }

        if(it_list_i->second.max_sig==NULL)
        {
            if(pointer_j!=NULL) {
                (*myself->TermIndex)[it_list_j->first].max_sig = (*myself->TermIndex)[it_list_j->first].id2pointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_sig;

                tmp_node=(*myself->TermIndex)[it_list_j->first].max_sig;

                while(pointer_j!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node;
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }
                tmp_node=NULL;
            }
            else
            {
                (*myself->TermIndex)[it_list_j->first].max_sig=NULL;
            }
        }
        else if(pointer_j!=NULL){
            //看一眼pointer_i是不是也要被清空
            if (it_list_i->second.max_sig->flag != 0) {
                pointer_i = it_list_i->second.max_sig;
            } else {
                pointer_i = it_list_i->second.max_sig;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_sig;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first].max_sig=NULL;
            }
            else
            {
                NodeInfo &tmp4=*it_list_j->second.max_sig;


                if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                {
                    (*myself->TermIndex)[it_list_j->first].max_sig=pointer_i;
                    pointer_i=pointer_i->next_sig;
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first].max_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_sig;
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first].max_sig;

                while(pointer_i!=NULL&&pointer_j!=NULL)
                {
                    if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                    {
                        tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);//同理
                        tmp_node=tmp_node->next_sig;
                        pointer_i=pointer_i->next_sig;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_sig;
                        }
                    }
                    else
                    {
                        tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_sig;
                        pointer_j=pointer_j->next_sig;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_sig;
                        }
                    }
                }

                while(pointer_i!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);//同理
                    tmp_node=tmp_node->next_sig;
                    pointer_i=pointer_i->next_sig;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_sig;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }

                tmp_node->next_sig=NULL;
            }
        }
    }

}

void *IiMergerThreadTermFreq(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    Ii *myself=ones->me;
    Ii *other=ones->him;

    map<string,ListInfo>::iterator it_list_i;
    map<string,ListInfo>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {


        pointer_j=it_list_j->second.max_termFreq;

        map<string,ListInfo> &tmp=*(myself->TermIndex);
        map<string,ListInfo> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second.max_termFreq;

        //提前看一眼pointer_j是不是要清空
        if (it_list_j->second.max_termFreq->flag != 0) {
            pointer_j = it_list_j->second.max_termFreq;
        } else {
            pointer_j = it_list_j->second.max_termFreq;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_termFreq;
            }
        }

        if(it_list_i->second.max_termFreq==NULL)
        {
            if(pointer_j!=NULL) {
                (*myself->TermIndex)[it_list_j->first].max_termFreq = (*myself->TermIndex)[it_list_j->first].id2pointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_termFreq;

                tmp_node=(*myself->TermIndex)[it_list_j->first].max_termFreq;

                while(pointer_j!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node;
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }
                tmp_node=NULL;
            }
            else
            {
                (*myself->TermIndex)[it_list_j->first].max_termFreq=NULL;
            }
        }
        else if(pointer_j!=NULL) {
            //看一眼pointer_i是不是也要被清空
            if (it_list_i->second.max_termFreq->flag != 0) {
                pointer_i = it_list_i->second.max_termFreq;
            } else {
                pointer_i = it_list_i->second.max_termFreq;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_termFreq;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first].max_termFreq=NULL;
            }
            else
            {
                NodeInfo &tmp4=*it_list_j->second.max_termFreq;

                if(pointer_i->tf>pointer_j->tf)
                {
                    (*myself->TermIndex)[it_list_j->first].max_termFreq=pointer_i;
                    pointer_i=pointer_i->next_termFreq;
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first].max_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_termFreq;
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first].max_termFreq;

                while(pointer_i!=NULL&pointer_j!=NULL)
                {
                    if(pointer_i->tf>pointer_j->tf)
                    {
                        tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);
                        tmp_node=tmp_node->next_termFreq;
                        pointer_i=pointer_i->next_termFreq;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_termFreq;
                        }
                    }
                    else
                    {
                        tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_termFreq;
                        pointer_j=pointer_j->next_termFreq;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_termFreq;
                        }
                    }
                }

                while(pointer_i!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_i=pointer_i->next_termFreq;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_termFreq;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first].id2pointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }

                tmp_node->next_termFreq=NULL;
            }
        }
    }

}