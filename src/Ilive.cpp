//
// Created by watson on 15/07/17.
//

#include "Ilive.h"


using namespace std;

Ilive::Ilive()
{
    AudioCount = 0;
    InfoTable = new map<int, AudioInfo>;
    TermIndex = new map<string, std::list<Fre> >;
}

void Ilive::addAudio(AudioInfo &tmp_info)
{

    map<int,AudioInfo>::iterator it_fre=(*InfoTable).find(tmp_info.id);
    if(it_fre==(*InfoTable).end())
    {
        (*InfoTable)[tmp_info.id] = tmp_info;
        AudioCount++;
        map<string, int>::iterator it;
        for (it = tmp_info.TagsNum.begin(); it != tmp_info.TagsNum.end(); it++)
        {
            term_add(it->first, tmp_info.id, tmp_info.time);
        }
    }
    else
    {
        it_fre->second.update(tmp_info);
        map<string, int>::iterator it;
        for (it = tmp_info.TagsNum.begin(); it != tmp_info.TagsNum.end(); it++)
        {
            term_add(it->first, tmp_info.id, tmp_info.time);
        }
    }

    //(*InfoTable) = (*InfoTable).insert(make_pair<int, AudioInfo>);

//    if (AudioCount == IndexUnit)
//    {
//        Ilive_sort();
//    }

}

void Ilive::term_add(string term, int id, double fresh)
{
    map<string,std::list<Fre> >::iterator it = (*TermIndex).find(term);
    if ((it == (*TermIndex).end()))
    {

        list<Fre> tmp;
        tmp.push_back(Fre(id, fresh));
        (TermMutex)[term] = CMutex();
        (*TermIndex)[term] = tmp;

    }
    else
    {
        (TermMutex)[term].Lock();
        it->second.push_front(Fre(id, fresh));//此处是放在头部的
        (TermMutex)[term].Unlock();
    }
}

//void Ilive::Ilive_sort()
//{
//    map<string, std::list<Fre> >::iterator it;
//    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
//    {
//        sort(it->second.begin(), it->second.end());
//    }
//
//}



void Ilive::term_remove(string term, int id)
{
    list<Fre>::iterator it;
    for (it=(*TermIndex)[term].begin();it!=(*TermIndex)[term].end();)
    {
        if(it->id==id)
        {
            it=(*TermIndex)[term].erase(it);
        }
        else
        {
            it++;
        }
    }

    if((*TermIndex)[term].size()==0)
    {
        (*TermIndex).erase(term);
    }
}

void Ilive::output()
{
    ofstream out("IliveResult.txt");
    cout << "Ilive_count:" << AudioCount << "\n";
    map<string, std::list<Fre> >::iterator it;
    for (it = (*TermIndex).begin(); it != (*TermIndex).end(); it++)
    {
        out << it->first << ':' << '\t';
        list<Fre>::iterator it_fre;
        for (it_fre = it->second.begin(); it_fre != it->second.end(); it_fre++)
        {
            out << it_fre->id << ',' << it_fre->fresh << '\t';
        }
        out << '\n';
    }
    out.close();
}

void Ilive::search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum,const vector<string> &query, map<int, string> &name)
{
    try
    {
        for (int i = 0; i < query.size(); i++)
        {

            map<string, std::list<Fre> >::iterator it = (*TermIndex).find(query[i]);
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
                std::list<Fre>::iterator it_li=(*TermIndex)[query[i]].begin();
                for (int j = 0; j < (*TermIndex)[query[i]].size(); j++,it_li++)
                {

                    int id1 = it_li->id;
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
                    else
                    {
                        (*TermIndex)[query[i]].erase(it_li);
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

void Ilive::clear()
{
    InfoTable = new map<int, AudioInfo>;
    AudioCount = 0;
    TermIndex = new map<string, std::list<Fre> >;
    TermMutex.clear();
}