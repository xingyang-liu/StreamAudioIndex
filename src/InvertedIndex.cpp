//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "InvertedIndex.h"


InvertedIndex::InvertedIndex()
{
    AudioCount = 0;
    level = 0;
    TermIndex = NULL;
    TermMutex=NULL;
    InfoTable = NULL;
}

void InvertedIndex::addAudio(AudioInfo &tmp_info,map<string,int> &TermFreq)
{
    if(TermIndex==NULL)
    {
//        TermIndexFre = new map<string, std::list<int> >;
//        TermIndexSig = new map<string, std::list<int> >;
//        TermIndexSim = new map<string, std::list<TermFreq> >;
        TermIndex =new map<string,ProgramList*>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<string,CMutex>;
    }

    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    map<string, int>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        node_add(it->first, tmp_info.id, TermFreq[it->first]);
    }
    //(*InfoTable) = (*InfoTable).insert(make_pair<int, AudioInfo>);

//    if (level==0&&AudioCount == IndexUnit)
//    {
//        I0_sort();
//    }
}

void InvertedIndex::node_add(string term, int id, int tf)
{
    map<string, ProgramList* >::iterator it = (*TermIndex).find(term);
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

void InvertedIndex::addAudioLive(AudioInfo &tmp_info,map<string,int> &TermFreq,map<int, map<string, NodeInfo *> > &livePointer\
    ,CMutex &mutexLive)
{
    if(TermIndex==NULL)
    {
        TermIndex = new map<string,ProgramList*>;
        InfoTable = new map<int, AudioInfo>;
        TermMutex = new map<string,CMutex>;
    }
    I0MutexInfo.Lock();
    (*InfoTable)[tmp_info.id] = tmp_info;
    I0MutexInfo.Unlock();
    AudioCount++;

    map<string, int>::iterator it;
    for (it = TermFreq.begin(); it != TermFreq.end(); it++)
    {
        node_addLive(it->first, tmp_info.id, TermFreq[it->first],livePointer,tmp_info.final,mutexLive);
    }


    map<string,NodeInfo*>::iterator it_node=livePointer[tmp_info.id].begin();
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
        for (; it_node != livePointer[tmp_info.id].end(); it_node++) {
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

            livePointer[it_node->second->id].erase(it_node->first);

        }
        mutexLive.Lock();
        livePointer.erase(tmp_info.id);
        mutexLive.Unlock();
    }
}

void InvertedIndex::node_addLive(string term, int id, int tf, map<int, map<string, NodeInfo*> > &livePointer,int final,CMutex &mutexLive)
{
    map<string, ProgramList* >::iterator it = (*TermIndex).find(term);
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

double InvertedIndex::computeScore(const double &time, const double &score, map<string, int> &TermFreq, const int &tagsSum,
                    const vector<string> &query)
{
    double fre = pow(2, time - getTime());
    double sig = log(score / 10000 + 1);
    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        map<string, int>::iterator it = TermFreq.find(query[i]);
        if (it != TermFreq.end())
        {
            try
            {
                if (tagsSum != 0)
                {
                    sim += TermFreq[query[i]] / tagsSum*IdfTable[query[i]];
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


void InvertedIndex::I0_sort()
{
    vector<int> id_list;
    map<int,NodeInfo*>::iterator it_id;
    NodeInfo *pointer;
    pthread_t pid1,pid2,pid3;
    map<string,ProgramList*> &other=*TermIndex;
    map<string,ProgramList*>::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {

        for(it_id=((it->second)->nodeMap)->begin();it_id!=((it->second)->nodeMap)->end();it_id++)
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

void InvertedIndex::output()
{
    cout << "I" << level << "_count:" << AudioCount << "\n";
}

int InvertedIndex::get_count() { return AudioCount; }

void InvertedIndex::MergerIndex(InvertedIndex &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
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

    map<int,AudioInfo>::iterator it_info;
    for(it_info=other.InfoTable->begin();it_info!=other.InfoTable->end();it_info++)
    {
        if((*InfoTable).find(it_info->first)==(*InfoTable).end())
        {
            (*InfoTable)[it_info->first]=it_info->second;
        }
    }


    map<string,ProgramList*>::iterator it_list_i;
    map<string,ProgramList*>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;
    map<int,NodeInfo*> ::iterator it_node_tmp;

    for(it_list_j=other.TermIndex->begin();it_list_j!=other.TermIndex->end();it_list_j++) {
        it_list_i = TermIndex->find(it_list_j->first);
        map<string,ProgramList*> &otherone=(*other.TermIndex);
        map<string,ProgramList*> &oneone=(*TermIndex);
        string str=it_list_j->first;


        if (it_list_i ==TermIndex->end()) {
            (*TermIndex)[it_list_j->first] = new ProgramList;

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



    FamilyMerger one(this,&other);
    ret=pthread_create(&pid[0],NULL,InvertedIndexMergerThreadFre,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }



    ret=pthread_create(&pid[2],NULL,InvertedIndexMergerThreadSig,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    ret=pthread_create(&pid[1],NULL,InvertedIndexMergerThreadTermFreq,(void*)&one);
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
        for(it_node_i=it_list_i->second->nodeMap->begin();it_node_i!=it_list_i->second->nodeMap->end();it_node_i++)
        {
            if(it_node_i->second->flag==0)
            {
                delete it_node_i->second;
                it_list_i->second->nodeMap->erase(it_node_i);
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

void InvertedIndex::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name)
{
    double up_fre = 0;
    double up_sig = 0;
    int id1, id2, id3;
    map<string, int> up_sim;
    double score = 0;
    map<string,int> TermFreq;
    map<int, double>::iterator it_res;
    map<int, NodeInfo *>::iterator it_tmp_node;//每个query中每个id获取全部tf时的迭代器
    ofstream out_res("InvertedIndex_Result.txt", ofstream::app);

    if(level==0)
    {
        for (int i = 0; i < query.size(); i++)
        {
            if(TermIndex==NULL) break;
            map<string, ProgramList*>::iterator it = (*TermIndex).find(query[i]);
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
                                map<string, ProgramList*>::iterator it_str = (*TermIndex).find(query[i]);
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

                map<string, ProgramList* >::iterator it = (*TermIndex).find(query[i]);
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
                                        map<string, ProgramList *>::iterator it_str = (*TermIndex).find(query[k]);
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
                        map<string, ProgramList* >::iterator it_exist = (*TermIndex).find(query[j]);
          
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
                                            map<string, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
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

                                    if(tmp_pointer->next_fresh!=NULL)
                                    {
                                        pointer_que.push(tmp_pointer->next_fresh);
                                    }
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
                                            map<string, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
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

                                    if(tmp_pointer->next_sig!=NULL)
                                    {
                                        pointer_que.push(tmp_pointer->next_sig);
                                    }
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
                                            map<string, ProgramList* >::iterator it_str = (*TermIndex).find(query[k]);
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


void *I0SortThread(void *fam)
{
    FamilyI0Sort &Fam=*(FamilyI0Sort *)fam;
    InvertedIndex *myself=Fam.me;
    vector<int> &id_list=Fam.id_list;
    int flag=Fam.flag;
    map<string,ProgramList*>::iterator &it=*Fam.it_list;



    if (flag==1)
    {
        sort(id_list.begin(),id_list.end(),CmpForFre(it->first,myself));
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
        sort(id_list.begin(),id_list.end(),CmpForSig(it->first,myself));
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
        sort(id_list.begin(),id_list.end(),CmpForSim(it->first,myself));
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


void *InvertedIndexMergerThreadFre(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    InvertedIndex *myself=ones->me;
    InvertedIndex *other=ones->him;

    map<string,ProgramList*>::iterator it_list_i;
    map<string,ProgramList*>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    int count=0;
    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {

        pointer_j=it_list_j->second->max_fresh;

        map<string,ProgramList*> &tmp=*(myself->TermIndex);
        map<string,ProgramList*> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second->max_fresh;

        //判断pointer_j是不是由于flag可能需要清空
        if (it_list_j->second->max_fresh->flag != 0) {
            pointer_j = it_list_j->second->max_fresh;
        } else {
            pointer_j = it_list_j->second->max_fresh;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_fresh;
            }
        }

        if(it_list_i->second->max_fresh==NULL)//判断InvertedIndex中是否原来有项，如果没有，则把Ij中的所有program按id复制过来（即不考虑复制指针）
        {
           if(pointer_j!=NULL) {
               (*myself->TermIndex)[it_list_j->first]->max_fresh = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                       pointer_j->tf, pointer_j->id);
               pointer_j = pointer_j->next_fresh;

               tmp_node=(*myself->TermIndex)[it_list_j->first]->max_fresh;

               while(pointer_j!=NULL)
               {
                   tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                   tmp_node=tmp_node->next_fresh;
                   pointer_j=pointer_j->next_fresh;
                   while (pointer_j != NULL && pointer_j->flag == 0) {
                       pointer_j = pointer_j->next_fresh;
                   }
               }
               tmp_node->next_fresh=NULL;
           }
           else
           {
               (*myself->TermIndex)[it_list_j->first]->max_fresh=NULL;
           }
        }
        else if(pointer_j!=NULL){//当InvertedIndex中有项时，考虑Ij是不是有项，如果没有就根据flag理一下InvertedIndex的顺序，有的话就先看一下I是不是由于flag需要清空，不要就正式开始归并，要的话就还是
            //判断pointer_i是不是由于flag可能也需要清空
            if (it_list_i->second->max_fresh->flag != 0) {
                pointer_i = it_list_i->second->max_fresh;
            } else {
                pointer_i = it_list_i->second->max_fresh;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_fresh;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_fresh = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_fresh;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_fresh;//注意这里pointeri或者j需要后移一位，避免max_fresh自循环

                while(pointer_j!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_fresh;
                    pointer_j=pointer_j->next_fresh;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_fresh;
                    }
                }
                tmp_node->next_fresh=NULL;
            }
            else
            {

                if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                {
                    (*myself->TermIndex)[it_list_j->first]->max_fresh=pointer_i;
                    pointer_i=pointer_i->next_fresh;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_fresh;
                    }
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first]->max_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_fresh;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_i->next_fresh;
                    }
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_fresh;

                while(pointer_i!=NULL&&pointer_j!=NULL)
                {
                    if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                    {
                        tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                        tmp_node=tmp_node->next_fresh;
                        pointer_i=pointer_i->next_fresh;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_fresh;
                        }
                    }
                    else
                    {
                        tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_fresh;
                        pointer_j=pointer_j->next_fresh;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_fresh;
                        }
                    }
                }

                while(pointer_i!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_fresh;
                    pointer_i=pointer_i->next_fresh;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_fresh;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_fresh;
                    pointer_j=pointer_j->next_fresh;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_fresh;
                    }
                }

                tmp_node->next_fresh=NULL;
            }
        }else{
            if (it_list_i->second->max_fresh->flag != 0) {
                pointer_i = it_list_i->second->max_fresh;
            } else {
                pointer_i = it_list_i->second->max_fresh;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_fresh;
                }
            }



            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_fresh=NULL;
            }
            else {
                it_list_i->second->max_fresh=pointer_i;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_fresh;

                pointer_i=pointer_i->next_fresh;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_fresh;
                }
                while(pointer_i!=NULL)
                {
                    tmp_node->next_fresh=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_fresh;
                    pointer_i=pointer_i->next_fresh;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_fresh;
                    }
                }
                tmp_node->next_fresh=NULL;

            }
        }
    }

}

void *InvertedIndexMergerThreadSig(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    InvertedIndex *myself=ones->me;
    InvertedIndex *other=ones->him;

    map<string,ProgramList*>::iterator it_list_i;
    map<string,ProgramList*>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {

        pointer_j=it_list_j->second->max_sig;

        map<string,ProgramList*> &tmp=*(myself->TermIndex);
        map<string,ProgramList*> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second->max_sig;

        //判断pointer_j是不是由于flag可能需要清空
        if (it_list_j->second->max_sig->flag != 0) {
            pointer_j = it_list_j->second->max_sig;
        } else {
            pointer_j = it_list_j->second->max_sig;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_sig;
            }
        }

        if(it_list_i->second->max_sig==NULL)//判断InvertedIndex中是否原来有项，如果没有，则把Ij中的所有program按id复制过来（即不考虑复制指针）
        {
            if(pointer_j!=NULL) {
                (*myself->TermIndex)[it_list_j->first]->max_sig = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_sig;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_sig;

                while(pointer_j!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }
                tmp_node->next_sig=NULL;
            }
            else
            {
                (*myself->TermIndex)[it_list_j->first]->max_sig=NULL;
            }
        }
        else if(pointer_j!=NULL){//当InvertedIndex中有项时，考虑Ij是不是有项，如果没有就根据flag理一下InvertedIndex的顺序，有的话就先看一下I是不是由于flag需要清空，不要就正式开始归并，要的话就还是
            //判断pointer_i是不是由于flag可能也需要清空
            if (it_list_i->second->max_sig->flag != 0) {
                pointer_i = it_list_i->second->max_sig;
            } else {
                pointer_i = it_list_i->second->max_sig;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_sig;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_sig = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_sig;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_sig;//注意这里pointeri或者j需要后移一位，避免max_sig自循环

                while(pointer_j!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }
                tmp_node->next_sig=NULL;
            }
            else
            {

                if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                {
                    (*myself->TermIndex)[it_list_j->first]->max_sig=pointer_i;
                    pointer_i=pointer_i->next_sig;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_sig;
                    }
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first]->max_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_sig;

                while(pointer_i!=NULL&&pointer_j!=NULL)
                {
                    if((*myself->InfoTable)[pointer_i->id].time>(*myself->InfoTable)[pointer_j->id].time)
                    {
                        tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                        tmp_node=tmp_node->next_sig;
                        pointer_i=pointer_i->next_sig;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_sig;
                        }
                    }
                    else
                    {
                        tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_sig;
                        pointer_j=pointer_j->next_sig;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_sig;
                        }
                    }
                }

                while(pointer_i!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_i=pointer_i->next_sig;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_sig;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_j=pointer_j->next_sig;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_sig;
                    }
                }

                tmp_node->next_sig=NULL;
            }
        }else{
            if (it_list_i->second->max_sig->flag != 0) {
                pointer_i = it_list_i->second->max_sig;
            } else {
                pointer_i = it_list_i->second->max_sig;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_sig;
                }
            }



            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_sig=NULL;
            }
            else {
                it_list_i->second->max_sig=pointer_i;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_sig;

                pointer_i=pointer_i->next_sig;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_sig;
                }
                while(pointer_i!=NULL)
                {
                    tmp_node->next_sig=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_sig;
                    pointer_i=pointer_i->next_sig;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_sig;
                    }
                }
                tmp_node->next_sig=NULL;

            }
        }
    }
}

void *InvertedIndexMergerThreadTermFreq(void *fam)
{
    FamilyMerger * ones=(FamilyMerger *)fam;
    InvertedIndex *myself=ones->me;
    InvertedIndex *other=ones->him;

    map<string,ProgramList*>::iterator it_list_i;
    map<string,ProgramList*>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;


    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {

        pointer_j=it_list_j->second->max_termFreq;

        map<string,ProgramList*> &tmp=*(myself->TermIndex);
        map<string,ProgramList*> &tmp1=*(other->TermIndex);
        string str=it_list_j->first;

        it_list_i=myself->TermIndex->find(it_list_j->first);

        NodeInfo &tmp3=*it_list_i->second->max_termFreq;

        //判断pointer_j是不是由于flag可能需要清空
        if (it_list_j->second->max_termFreq->flag != 0) {
            pointer_j = it_list_j->second->max_termFreq;
        } else {
            pointer_j = it_list_j->second->max_termFreq;
            while (pointer_j != NULL && pointer_j->flag == 0) {
                pointer_j = pointer_j->next_termFreq;
            }
        }

        if(it_list_i->second->max_termFreq==NULL)//判断InvertedIndex中是否原来有项，如果没有，则把Ij中的所有program按id复制过来（即不考虑复制指针）
        {
            if(pointer_j!=NULL) {
                (*myself->TermIndex)[it_list_j->first]->max_termFreq = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_termFreq;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_termFreq;

                while(pointer_j!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }
                tmp_node->next_termFreq=NULL;
            }
            else
            {
                (*myself->TermIndex)[it_list_j->first]->max_termFreq=NULL;
            }
        }
        else if(pointer_j!=NULL){//当InvertedIndex中有项时，考虑Ij是不是有项，如果没有就根据flag理一下InvertedIndex的顺序，有的话就先看一下I是不是由于flag需要清空，不要就正式开始归并，要的话就还是
            //判断pointer_i是不是由于flag可能也需要清空
            if (it_list_i->second->max_termFreq->flag != 0) {
                pointer_i = it_list_i->second->max_termFreq;
            } else {
                pointer_i = it_list_i->second->max_termFreq;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_termFreq;
                }
            }

            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_termFreq = (*myself->TermIndex)[it_list_j->first]->getNodePointer(
                        pointer_j->tf, pointer_j->id);
                pointer_j = pointer_j->next_termFreq;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_termFreq;//注意这里pointeri或者j需要后移一位，避免max_termFreq自循环

                while(pointer_j!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }
                tmp_node->next_termFreq=NULL;
            }
            else
            {

                if(pointer_i->tf>pointer_j->tf)
                {
                    (*myself->TermIndex)[it_list_j->first]->max_termFreq=pointer_i;
                    pointer_i=pointer_i->next_termFreq;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_termFreq;
                    }
                }
                else
                {
                    (*myself->TermIndex)[it_list_j->first]->max_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_termFreq;

                while(pointer_i!=NULL&&pointer_j!=NULL)
                {
                    if(pointer_i->tf>pointer_j->tf)
                    {
                        tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                        tmp_node=tmp_node->next_termFreq;
                        pointer_i=pointer_i->next_termFreq;
                        while (pointer_i != NULL && pointer_i->flag == 0) {
                            pointer_i = pointer_i->next_termFreq;
                        }
                    }
                    else
                    {
                        tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                        tmp_node=tmp_node->next_termFreq;
                        pointer_j=pointer_j->next_termFreq;
                        while (pointer_j != NULL && pointer_j->flag == 0) {
                            pointer_j = pointer_j->next_termFreq;
                        }
                    }
                }


                while(pointer_i!=NULL)
                {
                    if(tmp_node==(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id))
                    {
                        cout<<"wo"<<endl;
                    }else{
                        tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    }
                    tmp_node=tmp_node->next_termFreq;
                    pointer_i=pointer_i->next_termFreq;

                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_termFreq;
                    }
                }

                while(pointer_j!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_j->tf,pointer_j->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_j=pointer_j->next_termFreq;
                    while (pointer_j != NULL && pointer_j->flag == 0) {
                        pointer_j = pointer_j->next_termFreq;
                    }
                }

                tmp_node->next_termFreq=NULL;
            }
        }else{
            if (it_list_i->second->max_termFreq->flag != 0) {
                pointer_i = it_list_i->second->max_termFreq;
            } else {
                pointer_i = it_list_i->second->max_termFreq;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_termFreq;
                }
            }



            if(pointer_i==NULL)
            {
                (*myself->TermIndex)[it_list_j->first]->max_termFreq=NULL;
            }
            else {
                it_list_i->second->max_termFreq=pointer_i;

                tmp_node=(*myself->TermIndex)[it_list_j->first]->max_termFreq;

                pointer_i=pointer_i->next_termFreq;
                while (pointer_i != NULL && pointer_i->flag == 0) {
                    pointer_i = pointer_i->next_sig;
                }
                while(pointer_i!=NULL)
                {
                    tmp_node->next_termFreq=(*myself->TermIndex)[it_list_j->first]->getNodePointer(pointer_i->tf,pointer_i->id);
                    tmp_node=tmp_node->next_termFreq;
                    pointer_i=pointer_i->next_termFreq;
                    while (pointer_i != NULL && pointer_i->flag == 0) {
                        pointer_i = pointer_i->next_termFreq;
                    }
                }
                tmp_node->next_termFreq=NULL;

            }
        }
    }
}