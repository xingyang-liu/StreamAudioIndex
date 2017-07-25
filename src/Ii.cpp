
#include "Ii.h"

Ii::Ii()
{
    AudioCount = 0;
    level = 0;
    id_tmp = 0;
    TermIndexFre = NULL;
    TermIndexSig = NULL;
    TermIndexSim = NULL;
    InfoTable = NULL;
}

Ii::Ii(int level0, I0 &Index0)
{
    level = level0;
    AudioCount = Index0.AudioCount;

    InfoTable = new map<int,AudioInfo>;
    TermIndexSig = new map<string, vector<int> >;
    TermIndexSim = new map<string, vector<int> >;
    TermIndexFre = new map<string,vector<int>>;

    map<int, AudioInfo>::iterator it;
//    map<int,AudioInfo> &it_audio=*other.InfoTable;
    for (it = (*(Index0.InfoTable)).begin(); it != (*(Index0.InfoTable)).end(); it++)
    {
        (*InfoTable)[it->first] = it->second;
    }

    map<string, vector<int> >::iterator it_fre;
    for (it_fre = (*Index0.TermIndex).begin(); it_fre != (*Index0.TermIndex).end(); it_fre++)
    {
        try
        {
            vector<int> SigVec;
            vector<int> SimVec;
            vector<int> FreVec;
            for (int i = 0; i < it_fre->second.size(); i++)
            {
                id_tmp = it_fre->second[i];
                SigVec.push_back(id_tmp);
                SimVec.push_back(id_tmp);
                FreVec.push_back(id_tmp);
            }
            sort(SigVec.begin(), SigVec.end(),CmpForSig<Ii>(it_fre->first,this));
            sort(SimVec.begin(), SimVec.end(),CmpForSim<Ii>(it_fre->first,this));
            sort(FreVec.begin(),FreVec.end(),CmpForSim<Ii>(it_fre->first,this));
            (*TermIndexSig)[it_fre->first] = SigVec;
            (*TermIndexSim)[it_fre->first] = SimVec;
            (*TermIndexFre)[it_fre->first] =FreVec;
        }
        catch (...)
        {
//            cout << "Something wrong with I0toI1." << endl;
        }
    }
}

void Ii::output()
{
//    string FreName = "Index/I" + Itos(level) + "__postinglist_fre.txt";
//    string SigName = "Index/I" + Itos(level) + "__postinglist_sig.txt";
//    string SimName = "Index/I" + Itos(level) + "__postinglist_sim.txt";
//    ofstream out_fre(FreName.c_str());//不存在就创建是默认的
//    ofstream out_sig(SigName.c_str());
//    ofstream out_sim(SimName.c_str());
    cout << "I" << level << "_count:" << AudioCount << "\n";

//    map<string, vector<int> >::iterator it;
//    for (it = (*TermIndexFre).begin(); it != (*TermIndexFre).end(); it++)
//    {
//        out_fre << it->first << '\:' << '\t';
//        for (int i = 0; i < it->second.size(); i++)
//        {
//            out_fre << it->second[i].id << ',' << it->second[i].fresh << '\t';
//        }
//        out_fre << '\n';
//    }
//    out_fre.close();
//
//    map<string, vector<int> >::iterator it_sig;
//    for (it_sig = (*TermIndexSig).begin(); it_sig != (*TermIndexSig).end(); it_sig++)
//    {
//        out_sig << it_sig->first << '\:' << '\t';
//        for (int i = 0; i < it_sig->second.size(); i++)
//        {
//            out_sig << it_sig->second[i].get_id() << ',' << it_sig->second[i].get_score() << '\t';
//        }
//        out_sig << '\n';
//    }
//    out_sig.close();
//
//    map<string, vector<int> >::iterator it_sim;
//    for (it_sim = (*TermIndexSim).begin(); it_sim != (*TermIndexSim).end(); it_sim++)
//    {
//        out_sim << it_sim->first << '\:' << '\t';
//        for (int i = 0; i < it_sim->second.size(); i++)
//        {
//            out_sim << it_sim->second[i].get_id() << ',' << it_sim->second[i].get_tf() << '\t';
//        }
//        out_sim << '\n';
//    }
//    out_sim.close();
}

int Ii::get_count() { return AudioCount; }

//void Ii::test() { cout << (*TermIndexFre).size() << endl; cout << (*TermIndexSig).size() << endl; }
//
//void Ii::*MergerIndexFre(void *arg)
//{
//    Ii other=*(Ii *) arg;
//
//    map<string, vector<int> >::iterator it_fre;
//    for (it_fre = (*(other.TermIndexFre)).begin(); it_fre != (*(other.TermIndexFre)).end(); it_fre++)
//    {
//        int length = (*TermIndexFre)[it_fre->first].size();
//        (*TermIndexFre)[it_fre->first].resize(length + it_fre->second.size());
//        copy(it_fre->second.begin(), it_fre->second.end(), (*TermIndexFre)[it_fre->first].begin() + length);
//        inplace_merge(((*TermIndexFre)[it_fre->first]).begin(), (*TermIndexFre)[it_fre->first].begin() + length, ((*TermIndexFre)[it_fre->first]).end());
//    }
//}
//
//
//void Ii::*MergerIndexSig(void *arg)
//{
//    Ii other=*(Ii *) arg;
//    map<string, vector<int> >::iterator it_sig;
//    for (it_sig = (*(other.TermIndexSig)).begin(); it_sig != (*(other.TermIndexSig)).end(); it_sig++)
//    {
//        /*
//        vector<int> tmp_sig;
//        tmp_sig.swap((*TermIndexSig)[it_fre->first]);
//        (*TermIndexSig)[it_fre->first].resize((*TermIndexSig)[it_fre->first].size() + it_sig->second.size());//这里要求是排好序的vector
//        merge(tmp_sig.begin(), tmp_sig.end(), it_sig->second.begin(), it_sig->second.end(), (*TermIndexSig)[it_fre->first].begin());*/
//        //可以考虑用那个专属归并
//        int length = (*TermIndexSig)[it_sig->first].size();
//        (*TermIndexSig)[it_sig->first].resize(length + it_sig->second.size());
//        copy(it_sig->second.begin(), it_sig->second.end(), (*TermIndexSig)[it_sig->first].begin() + length);
//        inplace_merge(((*TermIndexSig)[it_sig->first]).begin(), (*TermIndexSig)[it_sig->first].begin() + length, ((*TermIndexSig)[it_sig->first]).end());
//    }
//}
//
//void Ii::*MergerIndexSim(void *arg)
//{
//    Ii other=*(Ii *) arg;
//    map<string, vector<int> >::iterator it_sim;
//    for (it_sim = (*(other.TermIndexSim)).begin(); it_sim != (*(other.TermIndexSim)).end(); it_sim++)
//    {
//        int length = (*TermIndexSim)[it_sim->first].size();
//        (*TermIndexSim)[it_sim->first].resize(length + it_sim->second.size());
//        copy(it_sim->second.begin(), it_sim->second.end(), (*TermIndexSim)[it_sim->first].begin() + length);
//        inplace_merge(((*TermIndexSim)[it_sim->first]).begin(), (*TermIndexSim)[it_sim->first].begin() + length, ((*TermIndexSim)[it_sim->first]).end());
//    }
//}

void Ii::Ii_sort()
{
    map<string, vector<int> >::iterator it1;
    for (it1 = (*TermIndexFre).begin(); it1 != (*TermIndexFre).end(); it1++)
    {
        sort(it1->second.begin(), it1->second.end(),CmpForSig<Ii>(it1->first,this));
    }

    map<string, vector<int> >::iterator it2;
    for (it2 = (*TermIndexSig).begin(); it2 != (*TermIndexSig).end(); it2++)
    {
        sort(it2->second.begin(), it2->second.end(),CmpForFre<Ii>(it2->first,this));
    }

    map<string, vector<int> >::iterator it3;
    for (it3 = (*TermIndexSim).begin(); it3 != (*TermIndexSim).end(); it3++)
    {
        sort(it3->second.begin(), it3->second.end(),CmpForSim<Ii>(it3->first,this));
    }

}

void Ii::MergerIndex(Ii &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
    level++;
    AudioCount += other.AudioCount;
    //merger info_table

    pthread_t pid[3];
    int ret;
//
//    ret=pthread_create(&pid[i],NULL,MergerIndexFre,(void*)&other);
//    if (ret!=0) {
//        cout << "Error" << endl;
//        exit(2);
//    }
//
//    ret=pthread_create(&pid[i],NULL,MergerIndexSig,(void*)&other);
//    if (ret!=0) {
//        cout << "Error" << endl;
//        exit(2);
//    }
//
    Ii_sort();//里面可以写多线程，这里也可以写多线程
    other.Ii_sort();


    map<int, AudioInfo>::iterator it;
    map<int,AudioInfo> &it_audio=*other.InfoTable;
    for (it = (*(other.InfoTable)).begin(); it != (*(other.InfoTable)).end(); it++)
    {
        (*InfoTable)[it->first] = it->second;
    }

    bufferMutex.Lock();
    for (int i=0;i<updateBuffer.size();i++)
    {
        updateBuffer.pop();
    }
    bufferMutex.Unlock();


    FamilyIi one1(this,&other,1);
    ret=pthread_create(&pid[0],NULL,MergerIndexThread<int>,(void*)&one1);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    FamilyIi one2(this,&other,2);
    ret=pthread_create(&pid[1],NULL,MergerIndexThread<int>,(void*)&one2);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    FamilyIi one3(this,&other,3);
    ret=pthread_create(&pid[2],NULL,MergerIndexThread<int>,(void*)&one3);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }




//
    for (int i=0;i<3;i++)
    {
        pthread_join(pid[i],NULL);
    }

    //merger index_fre


    //merger index_sig


    //merger index_sim

}

void Ii::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name)
{
    double up_fre = 0;
    double up_sig = 0;
    int id1, id2, id3,id4;
    map<string, double> up_sim;
    map<int, double>::iterator it_res;
    double score = 0;
    ofstream out_res("Ii.txt", ofstream::app);


    if (Sum < AnsNum)
    {
        for (int i = 0; i < query.size(); i++)
        {
            map<string, vector<int> >::iterator it = (*TermIndexFre).find(query[i]);
            if (it != (*TermIndexFre).end())
            {
                for (int j = 0; j < it->second.size(); j++)
                {
                    //cout << it->first << endl;
                    it_res = Result.find(it->second[j]);

                    if (it_res == Result.end())
                    {

                        AudioInfo &info_tmp = (*InfoTable)[it->second[j]];
                        score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum, query);
                        Sum += 1;
                        Result[it->second[j]] = score;
                        if (Sum >= AnsNum)
                            break;
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
        for (int i = 0; i < (*TermIndexFre).size(); i++)
        {
            if (TerFlag)
                break;
            up_fre = 0;
            up_sig = 0;

            for (int j = 0; j < query.size(); j++)
            {
                up_sim[query[j]] = 0;
            }

            priority_queue<Sig> copyBuffer(updateBuffer);

            try
            {
                for (int j = 0; j < query.size(); j++)
                {
                    map<string, vector<int> >::iterator it_exist = (*TermIndexFre).find(query[j]);
                    if (it_exist != (*TermIndexFre).end())
                    {
                        if (i < (*TermIndexFre).size())
                        {
                            id1 = (*TermIndexFre)[query[j]][i];
                            it_res = Result.find(id1);
                            if (it_res != Result.end())
                            {
                                AudioInfo &info_tmp = (*InfoTable)[id1];
                                score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum,
                                                     query);
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
                            }
                        }

                        if (i < (*TermIndexSig).size())
                        {
                            id2 = (*TermIndexSig)[query[j]][i];
                            it_res = Result.find(id2);
                            if (it_res != Result.end())
                            {
                                AudioInfo &info_tmp = (*InfoTable)[id2];
                                score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum,
                                                     query);
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
                            }
                        }

                        if (i < (*TermIndexSim).size())
                        {
                            id3 = (*TermIndexSim)[query[j]][i];
                            it_res = Result.find(id3);
                            if (it_res != Result.end())
                            {
                                AudioInfo &info_tmp = (*InfoTable)[id3];
                                score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum,
                                                     query);
                                if (score > MinScore)
                                {
                                    Result[id3] = score;
                                    MinScore = score;
                                    Sum += 1;
                                }
                                if (up_sim[query[j]] < info_tmp.TagsNum[query[j]])
                                {
                                    up_sim[query[j]] = info_tmp.TagsNum[query[j]];
                                }
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
                                    score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum,
                                                         query);
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

template <class T>
void *MergerIndexThread(void *fam)
{
    FamilyIi * ones=(FamilyIi *)fam;
    Ii *myself=ones->me;
    Ii *other=ones->him;
    int argTmp=ones->identi;


    typename map<string, vector<T> >::iterator it_T;
    for (it_T = (*(other->getPosting(argTmp))).begin(); it_T != (*(other->getPosting(argTmp))).end(); it_T++)
    {
        int length = (*(myself->getPosting(argTmp)))[it_T->first].size();
        (*(myself->getPosting(argTmp)))[it_T->first].resize(length + it_T->second.size());
        copy(it_T->second.begin(), it_T->second.end(), (*(myself->getPosting(argTmp)))[it_T->first].begin() + length);
        if(argTmp==1)
        {
            inplace_merge(((*(myself->getPosting(argTmp)))[it_T->first]).begin(), \
		(*(myself->getPosting(argTmp)))[it_T->first].begin() + length,\
		 ((*(myself->getPosting(argTmp)))[it_T->first]).end(),CmpForFre<Ii>(it_T->first,myself));
        }
        else if (argTmp==2)
        {
            inplace_merge(((*(myself->getPosting(argTmp)))[it_T->first]).begin(), \
		(*(myself->getPosting(argTmp)))[it_T->first].begin() + length,\
		 ((*(myself->getPosting(argTmp)))[it_T->first]).end(),CmpForSig<Ii>(it_T->first,myself));

        }
        else if(argTmp==3)
        {
            inplace_merge(((*(myself->getPosting(argTmp)))[it_T->first]).begin(), \
		(*(myself->getPosting(argTmp)))[it_T->first].begin() + length,\
		 ((*(myself->getPosting(argTmp)))[it_T->first]).end(),CmpForSim<Ii>(it_T->first,myself));

        }

    }
}