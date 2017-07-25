//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "I0.h"

using namespace std;

I0::I0()
{
    AudioCount = 0;
    InfoTable = new map<int, AudioInfo>;
    TermIndex = new map<string, vector<int> >;
}

void I0::addAudio(AudioInfo &tmp_info)
{
    (*InfoTable)[tmp_info.id] = tmp_info;
    AudioCount++;
    TermIndexMutex.Lock();
    map<string, double>::iterator it;
    for (it = tmp_info.TagsNum.begin(); it != tmp_info.TagsNum.end(); it++)
    {
        term_add(it->first, tmp_info.id, tmp_info.time);
    }
    TermIndexMutex;
    //(*InfoTable) = (*InfoTable).insert(make_pair<int, AudioInfo>);

}

void I0::term_add(string term, int id, double fresh)
{
    map<string, vector<int> >::iterator it = (*TermIndex).find(term);
    if ((it == (*TermIndex).end()))
    {

        vector<int> tmp;
        tmp.insert(tmp.begin(),id);
        (TermMutex)[term] = CMutex();
        (*TermIndex)[term] = tmp;

    }
    else
    {
        (TermMutex)[term].Lock();
        (*TermIndex)[term].insert((*TermIndex)[term].begin(),id);//这里是倒序的时间，vector很尬，搜索呢个地方是倒着搜索的
        (TermMutex)[term].Unlock();
    }
}

void I0::I0_sort()
{
    map<string, vector<int> >::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {
        sort(it->second.begin(), it->second.end(),CmpForFre<I0>(it->first,this));
    }

}

void I0::output()
{
//    ofstream out("Index/I0_Result.txt");
    cout << "I0_count:" << AudioCount << "\n";
//    map<string, vector<Fre> >::iterator it;
//    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
//    {
//        out << it->first << ':' << '\t';
//        for (int i = 0; i < it->second.size(); i++)
//        {
//            out << it->second[i].id << ',' << it->second[i].fresh << '\t';
//        }
//        out << '\n';
//    }
//    out.close();
}

void I0::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum,const vector<string> &query, map<int, string> &name)
{
    try
    {
        for (int i = query.size()-1; i >=0 ; i--)
        {

            map<string, vector<int> >::iterator it = (*TermIndex).find(query[i]);
//            cout << get_count();
//            map<string,vector<Fre> >::iterator it_tmp;
//            for (it_tmp=(*TermIndex).begin();it_tmp!=(*TermIndex).end();it_tmp++)
//            {
//                string tmp = it_tmp->first;
//                cout<<tmp<<endl;
//                if (tmp == query[i]) cout << "find it!!" << endl;
//            }
            if (it != (*TermIndex).end())
            {
                TermMutex[query[i]].Lock();
                for (int j = 0; j < (*TermIndex)[query[i]].size(); j++)
                {
                    int id1 = (*TermIndex)[query[i]][j];
                    map<int, double>::iterator term_it = Result.find(id1);

                    if (term_it == Result.end())
                    {
                        double score = computeScore((*InfoTable)[id1].time, (*InfoTable)[id1].score,
                                                    (*InfoTable)[id1].TagsNum, (*InfoTable)[id1].TagsSum, query);
                        Sum++;
                        Result[id1] = score;
                        if (score < MinScore)
                        {
                            MinScore = score;
                        }
                    }
                }
                TermMutex[query[i]].Unlock();
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
    }
    catch (...)
    {
//        cout << "Something wrong with I0 handleQuery." << endl;
    }
}

void I0::clear()
{
    InfoTable = new map<int, AudioInfo>;
    AudioCount = 0;
    TermIndex = new map<string, vector<int> >;
    TermMutex.clear();
}