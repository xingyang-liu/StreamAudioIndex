//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "IndexAll.h"

void IndexAll::addAudio(int id, string title, int LikeCount, int CommentCount, int PlayCount, double score, map<string, int> TagsNum, int TagsSum, double time)
{
    if (I0Num < IndexUnit)
    {
        Index0.addAudio(id, title, LikeCount, CommentCount, PlayCount, score, TagsNum, TagsSum, time);
        I0Num++;
    }

    if (I0Num >= IndexUnit)
    {
        Ii *Index1_tmp;
        Index1_tmp = new Ii(1, Index0);
        //(*Index1_tmp).test();
        int l = 1;
        while (1)
        {
            map<int, Ii*>::iterator it_index = otherIndex.find(l);
            if (it_index != otherIndex.end())
            {
                (*Index1_tmp).MergerIndex(*(otherIndex[l]));
                l += 1;
                otherIndex.erase(l - 1);
            }
            else
            {
                otherIndex[l] = Index1_tmp;

                break;

            }
        }
        Index0.clear();
        I0Num = 0;
    }
}

void IndexAll::output()
{
    Index0.output();

    map<int, Ii*>::iterator it_index;
    for (it_index = otherIndex.begin(); it_index != otherIndex.end(); it_index++)
    {
        (*it_index->second).output();
    }
}

void IndexAll::get_count()
{
//    cout << "Index0 sum is " + Itos(Index0.get_count()) << endl;
}

vector<pair<int, double> > IndexAll::search(const vector<string> &query, map<int, string> &name)
{
    map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;
    try
    {
        Index0.search(Result, MinScore, AnswerNum, Sum, query, name);

        map<int, Ii*>::iterator it_index;
        for (it_index = otherIndex.begin(); it_index != otherIndex.end(); it_index++)
        {
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
        }
        vector<pair<int, double> > ResVector(Result.begin(), Result.end());
        sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
        return ResVector;
    }
    catch (...)
    {
//        cout << "Search has somthing wrong." << endl;
    }
}