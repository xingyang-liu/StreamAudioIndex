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

    map<int, Ii*>::iterator it_index;
    for (it_index = otherIndex.begin(); it_index != otherIndex.end(); it_index++)
    {
        (*it_index->second).output();
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
        myself->clearI0.Lock();
        myself->otherIndex[0]->I0MutexInfo.Lock();
        myself->otherIndex[0]->search(Result, MinScore, AnswerNum, Sum, query, name);
        myself->otherIndex[0]->I0MutexInfo.Unlock();
        myself->clearI0.Unlock();

        map<int, Ii*>::iterator it_index;
        myself->clearIi.Lock();//在清理整个Ii//这里包括镜像在内所有Ii都不允许变化
        for (it_index = myself->otherIndex.begin(); it_index != myself->otherIndex.end(); it_index++)
        {
            if(it_index->first==0) continue;
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
        }

        vector<ForMirror*>::iterator it_tmp_mirror;
        for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
        {
            (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改
            if(!(*it_tmp_mirror)->flag)
            {
                myself->mirrorList.erase(it_tmp_mirror);
            }
            else
            {

                map<int,Ii*>::iterator it_mirror_index;
                for (it_mirror_index=((*it_tmp_mirror)->mirrorIndexMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIndexMap)->end();it_mirror_index++)
                {
                    (it_mirror_index->second)->search(Result, MinScore, AnswerNum, Sum, query, name);
                }

                it_tmp_mirror++;
            }
            (*it_tmp_mirror)->mutex.Unlock();

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
    map<string,int> &TagsNum=*(ones->tags);
    vector<ForMirror* > &mirrorList=myself->mirrorList;


    if (tmp_info.final>=0)
    {
        myself->liveIdMutex[tmp_info.id].Lock();
        myself->mutexLive.Lock();//有意义，勿删
        myself->mutexLive.Unlock();
        myself->otherIndex[0]->addAudioLive(tmp_info,TagsNum,myself->livePointer,myself->mutexLive);
        myself->liveIdMutex[tmp_info.id].Unlock();
        if(tmp_info.final==0)
        {
            map<int,Ii*>::iterator it_Index;
            for(it_Index=myself->otherIndex.begin();it_Index!=myself->otherIndex.end();it_Index++)
            {
                if(it_Index->second->search(tmp_info.id)&&it_Index->second->level!=0)
                {
                    it_Index->second->RemovedId.insert(tmp_info.id);
                }
            }
        }
    }
    else if(tmp_info.final==-1)
    {
        myself->otherIndex[0]->addAudio(tmp_info,TagsNum);
    }
    myself->I0Num++;


    if (myself->I0Num >= IndexUnit)
    {
        myself->otherIndex[0]->I0_sort();
        Ii *Index_tmp;

        myself->clearI0.Lock();//复制I0的过程开始了
        Index_tmp=new Ii(*myself->otherIndex[0]);
        Index_tmp->level+=1;

        map<int,Ii*> *mirrorIndex=new map<int,Ii*>;//相当于独立出参与归并的Index到另一个map中
        ForMirror *for_mirror=new ForMirror(mirrorIndex);
        mirrorList.push_back(for_mirror);
        (*mirrorIndex)[0]=myself->otherIndex[0];

        myself->otherIndex[0]=new Ii;

        myself->I0Num = 0;
        myself->clearI0.Unlock();

        int l=1;

        map<int, Ii*>::iterator it_index;
        while (1) {
            it_index = myself->otherIndex.find(l);

            if (it_index != myself->otherIndex.end())
            {
                map<int,Ii*> &tmp=myself->otherIndex;
                (*Index_tmp).MergerIndex(*(myself->otherIndex[l]));
                for_mirror->mutex.Lock();
                (*mirrorIndex)[l]=myself->otherIndex[l];
                for_mirror->mutex.Unlock();

                myself->otherIndex.erase(l);
                l += 1;
            }
            else
            {
                myself->clearIi.Lock();
                map<int,Ii*>::iterator it_index;
                for (it_index=(*mirrorIndex).begin(); it_index != (*mirrorIndex).end(); it_index++)
                {
                    delete it_index->second;
                }
                myself->otherIndex[l] = Index_tmp;
                delete mirrorIndex;
                mirrorIndex=NULL;
                for_mirror->flag=false;
                myself->clearIi.Unlock();


                break;
            }
        }
    }
}