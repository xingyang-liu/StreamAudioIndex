//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "IndexAll.h"

void IndexAll::output()
{
    IndexLive.output();
    Index0->output();

    map<int, Ii*>::iterator it_index;
    for (it_index = otherIndex.begin(); it_index != otherIndex.end(); it_index++)
    {
        (*it_index->second).output();
    }
}

vector<pair<int, double> > IndexAll::search(const vector<string> &query, map<int, string> &name)
{
    map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;
    try
    {
        Index0->search(Result, MinScore, AnswerNum, Sum, query, name);

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

void *searchThread(void *Family)
{
    FamilyQuery *fam=(FamilyQuery *)Family;
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
        myself->Index0->MutexInfo.Lock();
        myself->Index0->search(Result, MinScore, AnswerNum, Sum, query, name);
        myself->Index0->MutexInfo.Unlock();
        myself->clearI0.Unlock();

        map<int, Ii*>::iterator it_index;
        myself->clearIi.Lock();//可能在清除指针，所以堵塞一下
        for (it_index = myself->otherIndex.begin(); it_index != myself->otherIndex.end(); it_index++)
        {
            (*it_index->second).termIndexMutex.Lock();
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
            (*it_index->second).termIndexMutex.Unlock();
        }
        myself->clearIi.Unlock();

        myself->clearMirror.Lock();
        vector<ForMirror*>::iterator it_tmp_mirror;
        for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
        {
            //当我进入某个镜像时，不允许该镜像修改
            (*it_tmp_mirror)->mirrorI0->search(Result, MinScore, AnswerNum, Sum, query, name);
            map<int,Ii*>::iterator it_mirror_index;
            for (it_mirror_index=((*it_tmp_mirror)->mirrorIiMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIiMap)->end();it_mirror_index++)
            {
                (it_mirror_index->second)->search(Result, MinScore, AnswerNum, Sum, query, name);
            }

            it_tmp_mirror++;
        }
        myself->clearMirror.Unlock();


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
        myself->IndexLive.MutexInfo.Lock();
        myself->IndexLive.addAudio(tmp_info);
        myself->IndexLive.MutexInfo.Unlock();
    }
    else if(tmp_info.final==0)
    {
        map<int,AudioInfo>::iterator it_audio=(*myself->IndexLive.InfoTable).find(tmp_info.id);

        map<string, double>::iterator it;
        for (it = (it_audio->second).TagsNum.begin(); it != (it_audio->second).TagsNum.end(); it++)
        {
            (myself->IndexLive.TermMutex)[it->first].Lock();
            (myself->IndexLive).term_remove(it->first, tmp_info.id);
            (myself->IndexLive.TermMutex)[it->first].Unlock();

        }
        tmp_info.update(it_audio->second);
        myself->IndexLive.MutexInfo.Lock();
        (*myself->IndexLive.InfoTable).erase(tmp_info.id);
        myself->IndexLive.MutexInfo.Unlock();
        (myself->IndexLive.AudioCount)--;
        tmp_info.final=-1;
    }


    if(tmp_info.final==-1)
    {
        //类似互斥锁一样的东西
        myself->Index0->MutexInfo.Lock();
        (*(myself->Index0->InfoTable))[tmp_info.id] = tmp_info;
        myself->Index0->MutexInfo.Unlock();
        myself->Index0->AudioCount++;
        myself->Index0->TermIndexMutex.Lock();
        map<string, double>::iterator it;
        for (it = tmp_info.TagsNum.begin(); it != tmp_info.TagsNum.end(); it++)
        {
            myself->Index0->term_add(it->first, tmp_info.id, tmp_info.time);
        }
        myself->I0Num++;
        myself->Index0->TermIndexMutex.Unlock();
        //可以查询


        if (myself->I0Num >= IndexUnit)
        {
            int other=IndexUnit;
            myself->clearI0.Lock();
            myself->Index0->I0_sort();

            I0 *mirror_I0=myself->Index0;
//            map<int,AudioInfo> &other=*mirror_I0->InfoTable;


            ForMirror *for_mirror=new ForMirror(mirror_I0);
            myself->mirrorList.push_back(for_mirror);



            Ii *Index1_tmp;
            Index1_tmp = new Ii(1, *(myself->Index0));//有毒
            myself->Index0=new I0;
            myself->clearI0.Unlock();

            int l = 1;
            map<int, Ii*>::iterator it_index;
//            vector<int> del_list;


            while (1)
            {
                it_index = myself->otherIndex.find(l);
                if (it_index != myself->otherIndex.end())
                {
                    (*Index1_tmp).MergerIndex(*(myself->otherIndex[l]));
                    if(for_mirror->mirrorIiMap==NULL)
                    {
                        for_mirror->mirrorIiMap=new map<int,Ii*>;
                    }
                    (*(for_mirror->mirrorIiMap))[l]=myself->otherIndex[l];
                    myself->clearIi.Lock();
//                    map<int,Ii*> &iother=*for_mirror->mirrorIiMap;
                    myself->otherIndex.erase(l);
                    myself->clearIi.Unlock();
                    l += 1;
                }
                else
                {
                    myself->clearIi.Lock();
                    myself->otherIndex[l] = Index1_tmp;
                    myself->clearIi.Unlock();

                    myself->clearMirror.Lock();

                    delete for_mirror->mirrorI0;
                    for_mirror->mirrorI0=NULL;


                    if(for_mirror->mirrorIiMap!=NULL)
                    {

                        map<int,Ii*>::iterator it_index;
                        for (it_index=for_mirror->mirrorIiMap->begin(); it_index != for_mirror->mirrorIiMap->end(); it_index++)
                        {
                            delete it_index->second;
                            it_index->second=NULL;
                        }
                        delete for_mirror->mirrorIiMap;
                        for_mirror->mirrorIiMap=NULL;
                    }

                    myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),for_mirror),myself->mirrorList.end());
                    delete for_mirror;
                    myself->clearMirror.Unlock();
                    break;

                }
            }
            myself->I0Num=0;

        }
    }

}

void IndexAll::updateScore(int id,double new_score)
{


    map<int,AudioInfo>::iterator it_info;
    it_info=(Index0->InfoTable)->find(id);
    if(it_info!=(Index0->InfoTable)->end())
    {
        (Index0)->updateScore(id,new_score);
    }

    map<int, Ii*>::iterator it_Index;
    for (it_Index=otherIndex.begin();it_Index!=otherIndex.end();it_Index++)
    {
        it_info=((it_Index->second)->InfoTable)->find(id);
        if(it_info!=((it_Index->second)->InfoTable)->end())
        {
            (it_Index->second->updateScore(id,new_score));

        }
    }
}