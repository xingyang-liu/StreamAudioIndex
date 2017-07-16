//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "IndexAll.h"

//void IndexAll::addAudio(AudioInfo &audio_info)
//{
//    if (I0Num < IndexUnit)
//    {
//        Index0.addAudio(audio_info);
//        I0Num++;
//    }
//
//    if (I0Num >= IndexUnit)
//    {
//        Ii *Index1_tmp;
//        Index1_tmp = new Ii(1, Index0);
//        //(*Index1_tmp).test();
//        int l = 1;
//        while (1)
//        {
//            map<int, Ii*>::iterator it_index = otherIndex.find(l);
//            if (it_index != otherIndex.end())
//            {
//                (*Index1_tmp).MergerIndex(*(otherIndex[l]));
//                delete otherIndex[l-1];
//                l += 1;
//                otherIndex.erase(l - 1);
//            }
//            else
//            {
//                otherIndex[l] = Index1_tmp;
//
//                break;
//
//            }
//        }
//        Index0.clear();
//        I0Num = 0;
//    }
//}

void IndexAll::output()
{
    IndexLive.output();
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

void *searchThread(void *family)
{
    FamilyQuery *fam=(FamilyQuery *)family;
    IndexAll *myself=fam->me;
    vector<string> &query=*(fam->que);
    vector<pair<int,double> > &ResVector=*(fam->ResVec);
    map<int,string> &name=*(fam->na);


    map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;
    try
    {
        myself->IndexLive.MutexInfo.Lock();
        myself->IndexLive.search(Result, MinScore, AnswerNum, Sum, query, name);
        myself->IndexLive.MutexInfo.Unlock();
        myself->clearI0.Lock();
        myself->Index0.MutexInfo.Lock();
        myself->Index0.search(Result, MinScore, AnswerNum, Sum, query, name);
        myself->Index0.MutexInfo.Unlock();
        myself->clearI0.Unlock();

        map<int, Ii*>::iterator it_index;
        myself->clearIi.Lock();//可能在清除指针，所以堵塞一下
        for (it_index = myself->otherIndex.begin(); it_index != myself->otherIndex.end(); it_index++)
        {
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
        }
        myself->clearIi.Unlock();
        vector<pair<int, double> > tmp(Result.begin(), Result.end());
        ResVector=tmp;
        sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
    }
    catch (...)
    {
//        cout << "Search has somthing wrong." << endl;
    }
}

void *addAudioALLThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyAll *ones=(FamilyAll*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    IndexAll *myself=ones->me;

    if (tmp_info.final>0)
    {
        myself->IndexLive.addAudio(tmp_info);
    }
    else if(tmp_info.final==0)
    {
        map<int,AudioInfo>::iterator it_audio=(*myself->IndexLive.InfoTable).find(tmp_info.id);

        map<string, int>::iterator it;
        for (it = (it_audio->second).TagsNum.begin(); it != (it_audio->second).TagsNum.end(); it++)
        {
            (myself->IndexLive.TermMutex)[it->first].Lock();
            (myself->IndexLive).term_remove(it->first, tmp_info.id);
            (myself->IndexLive.TermMutex)[it->first].Unlock();

        }
        tmp_info.update(it_audio->second);
        (*myself->IndexLive.InfoTable).erase(tmp_info.id);
        (myself->IndexLive.AudioCount)--;
        tmp_info.final=-1;
    }


    if(tmp_info.final==-1)
    {
        //类似互斥锁一样的东西
        myself->maxTime=tmp_info.time;
        myself->Index0.MutexInfo.Lock();
        (*(myself->Index0.InfoTable))[tmp_info.id] = tmp_info;
        myself->Index0.MutexInfo.Unlock();
        myself->Index0.AudioCount++;
        map<string, int>::iterator it;
        for (it = tmp_info.TagsNum.begin(); it != tmp_info.TagsNum.end(); it++)
        {
            myself->Index0.term_add(it->first, tmp_info.id, tmp_info.time);
        }
        myself->I0Num++;
        //可以查询
        myself->maxTime=9999999999;

        if (myself->I0Num >= IndexUnit)
        {
            myself->Index0.I0_sort();
            Ii *Index1_tmp;
            Index1_tmp = new Ii(1, myself->Index0);//有毒
            int l = 1;
            map<int, Ii*>::iterator it_index;
            vector<int> del_list;
            while (1)
            {
                it_index = myself->otherIndex.find(l);
                if (it_index != myself->otherIndex.end())
                {
                    (*Index1_tmp).MergerIndex(*(myself->otherIndex[l]));
                    del_list.push_back(l);
                    l += 1;
                }
                else
                {
                    myself->clearIi.Lock();
                    for (int i=0;i<del_list.size();i++)
                    {
                        delete myself->otherIndex[del_list[i]];//有毒
                        myself->otherIndex.erase(del_list[i]);
                    }
                    myself->otherIndex[l] = Index1_tmp;
                    myself->clearIi.Unlock();
                    break;

                }
            }
            myself->clearI0.Lock();
            myself->Index0.clear();
            myself->clearI0.Unlock();
            myself->I0Num = 0;
        }
    }

}