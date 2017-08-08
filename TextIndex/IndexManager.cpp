//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "IndexManager.h"

//void IndexManager::addAudio(AudioInfo &audio_info)
//{
//    if (I0Num < IndexUnit)
//    {
//        Index0.addAudio(audio_info);
//        I0Num++;
//    }
//
//    if (I0Num >= IndexUnit)
//    {
//        InvertedIndex *Index1_tmp;
//        Index1_tmp = new InvertedIndex(1, Index0);
//        //(*Index1_tmp).test();
//        int l = 1;
//        while (1)
//        {
//            map<int, InvertedIndex*>::iterator it_index = Indexes.find(l);
//            if (it_index != Indexes.end())
//            {
//                (*Index1_tmp).MergerIndex(*(Indexes[l]));
//                delete Indexes[l-1];
//                l += 1;
//                Indexes.erase(l - 1);
//            }
//            else
//            {
//                Indexes[l] = Index1_tmp;
//
//                break;
//
//            }
//        }
//        Index0.clear();
//        I0Num = 0;
//    }
//}

void IndexManager::output()
{

    map<int, InvertedIndex*>::iterator it_index;
    for (it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        (*it_index->second).output();
    }
}

void *searchThread(void *family)
{
    FamilyQuery *fam=(FamilyQuery *)family;
    IndexManager *myself=fam->me;
    vector<string> &query=*(fam->que);
    vector<pair<int,double> > &ResVector=*(fam->ResVec);
    map<int,string> &name=*(fam->na);


    map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;
    try
    {
        myself->clearI0.Lock();
        myself->Indexes[0]->I0MutexInfo.Lock();
        myself->Indexes[0]->search(Result, MinScore, AnswerNum, Sum, query, name);
        myself->Indexes[0]->I0MutexInfo.Unlock();
        myself->clearI0.Unlock();

        map<int, InvertedIndex*>::iterator it_index;
        myself->clearInvertedIndex.Lock();//在清理整个InvertedIndex//这里包括镜像在内所有InvertedIndex都不允许变化
        for (it_index = myself->Indexes.begin(); it_index != myself->Indexes.end(); it_index++)
        {
            if(it_index->first==0) continue;
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
        }

        vector<ForMirror<InvertedIndex>*>::iterator it_tmp_mirror;
        for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
        {
            (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改

            map<int,InvertedIndex*>::iterator it_mirror_index;
            for (it_mirror_index=((*it_tmp_mirror)->mirrorIndexMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIndexMap)->end();it_mirror_index++)
            {
                (it_mirror_index->second)->search(Result, MinScore, AnswerNum, Sum, query, name);
            }

            it_tmp_mirror++;

            (*it_tmp_mirror)->mutex.Unlock();

        }
        myself->clearInvertedIndex.Unlock();

        vector<pair<int, double> > tmp(Result.begin(), Result.end());
        ResVector=tmp;
        sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
    }
    catch (...)
    {
        cout << "Search has somthing wrong." << endl;
    }
}

void *addAudioALLThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyAll *ones=(FamilyAll*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    IndexManager *myself=ones->me;
    map<string,double> &TagsNum=*(ones->tags);
    vector<ForMirror<InvertedIndex>* > &mirrorList=myself->mirrorList;


    if (tmp_info.final>=0)
    {
        myself->liveIdMutex[tmp_info.id].Lock();
        myself->mutexLive.Lock();//有意义，勿删
        myself->mutexLive.Unlock();
        myself->Indexes[0]->addAudioLive(tmp_info, TagsNum, myself->livePointer, myself->mutexLive);
        myself->liveIdMutex[tmp_info.id].Unlock();

    }
    else if(tmp_info.final==-1)
    {
        myself->Indexes[0]->addAudio(tmp_info,TagsNum);
    }
    myself->I0Num++;


    if (myself->I0Num >= IndexUnit)
    {
        myself->clearI0.Lock();//复制I0的过程开始了
        myself->Indexes[0]->I0_sort();


        InvertedIndex *Index_tmp;
        (myself->liveIdMutex)[tmp_info.id].Lock();
        Index_tmp=new InvertedIndex(*myself->Indexes[0]);
        (myself->liveIdMutex)[tmp_info.id].Unlock();
        Index_tmp->level+=1;
//        map<string,ProgramList*> &tmp_list=*(Index_tmp->TermIndex);

        map<int,InvertedIndex*> *mirrorIndex=new map<int,InvertedIndex*>;//相当于独立出参与归并的Index到另一个map中
        ForMirror<InvertedIndex>  *for_mirror=new ForMirror<InvertedIndex> (mirrorIndex);
        mirrorList.push_back(for_mirror);

        (*mirrorIndex)[0]=Index_tmp;
//        InvertedIndex &other=(*(*mirrorIndex)[0]);
        Index_tmp=myself->Indexes[0];
        Index_tmp->level+=1;

        myself->Indexes[0]=new InvertedIndex;
        myself->I0Num = 0;
        myself->clearI0.Unlock();
        int l=1;

        map<int, InvertedIndex*>::iterator it_index;
        while (1) {
            it_index = myself->Indexes.find(l);


            if (it_index != myself->Indexes.end())
            {
//                map<int,InvertedIndex*> &tmp=myself->Indexes;
//                map<string,ProgramList*> &tmp_list=*(myself->Indexes[l]->TermIndex);
                map<string,ProgramList*> &tmp_list=*(myself->Indexes[l]->TermIndex);
                InvertedIndex *other_tmp=new InvertedIndex(*(myself->Indexes[l]));
                (*mirrorIndex)[l]=other_tmp;//可能需要加互斥锁
                (*Index_tmp).MergerIndex(*(myself->Indexes[l]));
                delete myself->Indexes[l];

                myself->clearInvertedIndex.Lock();
                myself->Indexes.erase(l);
                myself->clearInvertedIndex.Unlock();

                l += 1;
            }
            else
            {
                myself->clearInvertedIndex.Lock();
                myself->Indexes[l] = Index_tmp;
                myself->clearInvertedIndex.Unlock();

                myself->clearMirror.Lock();
                map<int,InvertedIndex*>::iterator it_index;
                for (it_index=(*mirrorIndex).begin(); it_index != (*mirrorIndex).end(); it_index++)
                {
                    delete it_index->second;
                }
                delete mirrorIndex;
                for_mirror->mirrorIndexMap=NULL;
                myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),for_mirror),myself->mirrorList.end());
                delete for_mirror;
                myself->clearMirror.Unlock();

                break;
            }
        }
    }
}

void IndexManager::InitialIdf()
{
    double begin,end;
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    ifstream in("idf.txt");
    if (!in)
        exit(7);
    else {
        begin=getTime();
        cout << "Begin idf." << endl;
    }
    IdfNum=0;
    string term_tmp;
    string idf;
    while (!in.eof())
    {
        getline(in, term_tmp);
        getline(in, idf);
        if (QuestionMark.compare(term_tmp) && DoubleQuestionMark.compare(term_tmp) && SpaceKey.compare(term_tmp))
        {
            (IdfTable)[term_tmp] = atof(idf.c_str());
            //cout << term_tmp << ' ' << num_tmp << endl;
        }
        IdfNum++;
    }
    end=getTime();
    cout<<"Idf is okay."<<endl;
    cout<<"Time is "<<end-begin<<"s"<<endl;
}

void IndexManager::buildIndex(int audio_sum)
{
    double begin, end;
    begin = getTime();
    ifstream info_in("info_live.txt");
    if (!info_in) exit(7);
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string LikeCount_tmp, CommentCount_tmp, PlayCount_tmp, TagsSum_tmp, score_tmp, time_tmp, title_tmp,\
 		term_tmp, id_tmp, num_tmp, TermSum_tmp,FinalFlag_tmp;
    for (int i = 0; i < audio_sum; i++)
    {

        if(!getline(info_in, id_tmp)) break;
        getline(info_in, title_tmp);
        getline(info_in, LikeCount_tmp);
        getline(info_in, CommentCount_tmp);
        getline(info_in, PlayCount_tmp);
        getline(info_in, score_tmp);
        getline(info_in, TagsSum_tmp);
        getline(info_in, time_tmp);
        getline(info_in, TermSum_tmp);
        getline(info_in,FinalFlag_tmp);
        //cout << id_tmp << title_tmp << LikeCount_tmp << CommentCount_tmp << PlayCount_tmp << score_tmp << TagsSum_tmp << time_tmp << endl;
        int TagsSum = atoi(TagsSum_tmp.c_str());
        map<string, double> TagsNum_tmp;
        for (int j = 0; j < atoi(TermSum_tmp.c_str()); j++)
        {
            getline(info_in, term_tmp);
            getline(info_in, num_tmp);
            if (QuestionMark.compare(term_tmp) && DoubleQuestionMark.compare(term_tmp) && SpaceKey.compare(term_tmp))
            {
                (TagsNum_tmp)[term_tmp] = atof_1e(num_tmp.c_str());
                //cout << term_tmp << ' ' << num_tmp << endl;
            }
            else
            {
                TagsSum -= 2;
                //cout << "delete " << term_tmp << endl;
            }
        }
        char tmp[20];
        strcpy(tmp, id_tmp.c_str());
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, atof(score_tmp.c_str()), \
		  TagsSum, atof(time_tmp.c_str()),atoi(FinalFlag_tmp.c_str()));
        pthread_t id;
        FamilyAll fam(this,&tmp_info,&TagsNum_tmp);
        pthread_create(&id,NULL,addAudioALLThread,(void*)&fam);
        pthread_join(id,NULL);


    }
    end = getTime();

    output();
    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<end-begin\
    <<" Times: "<<1<<setprecision(8)<<" Average: "<<end-begin<<endl;
    cout << end - begin << "s" << endl;
    writefile.close();
    info_in.close();
}

string IndexManager::handleQuery(string query_str)
{

    string s;

    double begin, end;
    string stopwords = "，";
    vector<string> query;
    vector<pair<int, double> > Result;
    map<int, string> name;
//    ofstream out_res("Log.txt", ofstream::app);

    SplitString(query_str, query, stopwords);

    FamilyQuery fam(this,&query,&name,&Result);
    pthread_t id;
    pthread_create(&id,NULL,searchThread,(void*)&fam);
    pthread_join(id,NULL);

//    Result = Index.search(query, name);
//    cout << name.size() << endl;

//    out_res << query_str << endl;

    string str_back;
    end = getTime();
    if (Result.size() == 0)
    {
//        cout << "NULL" << endl;
        return "NULL\n";
//        return query_str;
    }
    else
    {
        for (int i = 0; i < Result.size(); i++)
        {
            str_back += "id： " + Itos(Result[i].first) + "\n"+"title： " + name[Result[i].first] +
                        "\nlink：http://www.ximalaya.com/sound/" + Itos(Result[i].first) + "\nscore: " + Dtos(Result[i].second) + "\n\n";
//            cout << Result[i].first << "-" << name[Result[i].first] << "\:" << Result[i].second << "\n";
//            out_res << Result[i].first << "-" << name[Result[i].first] << ":" << Result[i].second << "\n";
        }
    }
    str_back += "Time: "+ Dtos(end - begin) + "s" +"\n";
//		send(serConn, str_back.c_str(), strlen(str_back.c_str()) +5, 0);
    //cout << "Time: " << end - begin << "s" << endl;
//	out_res << "Time: " << end - begin << "s" << endl;
    query.clear();
    Result.clear();
    name.clear();
//	out_res.close();

//	closesocket(serConn);   //关闭
//	WSACleanup();           //释放资源的操作
//	cout << "连接断开" << endl;
    return str_back;
}