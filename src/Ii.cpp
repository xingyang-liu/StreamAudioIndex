//
// Created by 兴阳 刘 on 2017/7/11.
//

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
    TermIndexFre = Index0.TermIndex;
    InfoTable = Index0.InfoTable;
    TermIndexSig = new map<string, vector<Sig> >;
    TermIndexSim = new map<string, vector<TermFreq> >;
    map<string, vector<Fre> >::iterator it_fre;
    for (it_fre = (*TermIndexFre).begin(); it_fre != (*TermIndexFre).end(); it_fre++)
    {
        try
        {
            vector<Sig> SigVec;
            vector<TermFreq> SimVec;
            for (int i = 0; i < it_fre->second.size(); i++)
            {
                id_tmp = it_fre->second[i].id;
                Sig sig_(id_tmp, (*InfoTable)[id_tmp].score);
                TermFreq sim_(id_tmp, (*InfoTable)[id_tmp].TagsNum[it_fre->first]);
                SigVec.push_back(sig_);
                SimVec.push_back(sim_);
            }
            sort(SigVec.begin(), SigVec.end());
            sort(SimVec.begin(), SimVec.end());
            (*TermIndexSig)[it_fre->first] = SigVec;
            (*TermIndexSim)[it_fre->first] = SimVec;
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

//    map<string, vector<Fre> >::iterator it;
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
//    map<string, vector<Sig> >::iterator it_sig;
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
//    map<string, vector<TermFreq> >::iterator it_sim;
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
//    map<string, vector<Fre> >::iterator it_fre;
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
//    map<string, vector<Sig> >::iterator it_sig;
//    for (it_sig = (*(other.TermIndexSig)).begin(); it_sig != (*(other.TermIndexSig)).end(); it_sig++)
//    {
//        /*
//        vector<Sig> tmp_sig;
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
//    map<string, vector<TermFreq> >::iterator it_sim;
//    for (it_sim = (*(other.TermIndexSim)).begin(); it_sim != (*(other.TermIndexSim)).end(); it_sim++)
//    {
//        int length = (*TermIndexSim)[it_sim->first].size();
//        (*TermIndexSim)[it_sim->first].resize(length + it_sim->second.size());
//        copy(it_sim->second.begin(), it_sim->second.end(), (*TermIndexSim)[it_sim->first].begin() + length);
//        inplace_merge(((*TermIndexSim)[it_sim->first]).begin(), (*TermIndexSim)[it_sim->first].begin() + length, ((*TermIndexSim)[it_sim->first]).end());
//    }
//}




void Ii::MergerIndex(Ii &other)//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的
{
    int length;
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
    FamilyIi one(this,&other);
    ret=pthread_create(&pid[0],NULL,MergerIndexThread<Fre>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    ret=pthread_create(&pid[1],NULL,MergerIndexThread<Sig>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }

    ret=pthread_create(&pid[2],NULL,MergerIndexThread<TermFreq>,(void*)&one);
    if (ret!=0) {
        cout << "Error" << endl;
        exit(2);
    }


    map<int, AudioInfo>::iterator it;
    for (it = (*(other.InfoTable)).begin(); it != (*(other.InfoTable)).end(); it++)
    {
        (*InfoTable)[it->first] = it->second;
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
    int id1, id2, id3;
    map<string, int> up_sim;
    double score = 0;
    ofstream out_res("Ii.txt", ofstream::app);


    if (Sum < AnsNum)
    {
        for (int i = 0; i < query.size(); i++)
        {
            map<string, vector<Fre> >::iterator it = (*TermIndexFre).find(query[i]);
            if (it != (*TermIndexFre).end())
            {
                for (int j = 0; j < it->second.size(); j++)
                {
                    //cout << it->first << endl;
                    map<int, double>::iterator it_res = Result.find(it->second[j].id);

                    if (it_res == Result.end())
                    {

                        AudioInfo &info_tmp = (*InfoTable)[it->second[j].id];
                        score = computeScore(info_tmp.time, info_tmp.score, info_tmp.TagsNum, info_tmp.TagsSum, query);
                        Sum += 1;
                        Result[it->second[j].id] = score;
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

            try
            {
                for (int j = 0; j < query.size(); j++)
                {
                    map<string, vector<Fre> >::iterator it_exist = (*TermIndexFre).find(query[j]);
                    if (it_exist != (*TermIndexFre).end())
                    {
                        if (i < (*TermIndexFre).size())
                        {
                            id1 = (*TermIndexFre)[query[j]][i].id;
                            map<int, double>::iterator it_res = Result.find(id1);
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
                            id2 = (*TermIndexSig)[query[j]][i].get_id();
                            map<int, double>::iterator it_res = Result.find(id2);
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
                            id3 = (*TermIndexSim)[query[j]][i].get_id();
                            map<int, double>::iterator it_res = Result.find(id3);
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
    T argTmp;

    typename map<string, vector<T> >::iterator it_T;
    for (it_T = (*(other->getPosting(argTmp))).begin(); it_T != (*(other->getPosting(argTmp))).end(); it_T++)
    {
        int length = (*(myself->getPosting(argTmp)))[it_T->first].size();
        (*(myself->getPosting(argTmp)))[it_T->first].resize(length + it_T->second.size());
        copy(it_T->second.begin(), it_T->second.end(), (*(myself->getPosting(argTmp)))[it_T->first].begin() + length);
        inplace_merge(((*(myself->getPosting(argTmp)))[it_T->first]).begin(), \
		(*(myself->getPosting(argTmp)))[it_T->first].begin() + length,\
		 ((*(myself->getPosting(argTmp)))[it_T->first]).end());
    }
}