
#include "IndexManager.h"

void IndexManager::output()//输出每个Index的Info
{
    ofstream writefile("Components of Indexes.txt",ofstream::app);
    map<int, InvertedIndex*>::iterator it_index;
    writefile<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<endl;
    for (it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        cout << "I" << it_index->second->level << "_AudioCount: " << it_index->second->AudioCount <<"\tTermCount: "<<it_index->second->TermCount<< "\n";
        writefile<< "I" << it_index->second->level << "_AudioCount: " << it_index->second->AudioCount <<"\tTermCount: "<<it_index->second->TermCount<< "\n";
    }
    writefile.close();
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

    //判断Index0是否正在被清空或者修改Info
    myself->clearInvertedIndex.Lock();//在清理整个InvertedIndex，查询时要求包括镜像在内除所有InvertedIndex（这个map）
//    myself->clearI0.Lock();
    myself->Indexes[0]->I0MutexInfo.Lock();
    myself->Indexes[0]->search(Result, MinScore, AnswerNum, Sum, query, name);
    myself->Indexes[0]->I0MutexInfo.Unlock();
//    myself->clearI0.Unlock();

    map<int, InvertedIndex*>::iterator it_index;


    // 都不允许变化，防止出现查询时删除这种现象
    for (it_index = myself->Indexes.begin(); it_index != myself->Indexes.end(); it_index++)
    {
        if(it_index->first==0) continue;
        (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
    }

//    myself->clearMirror.Lock();
    vector<ForMirror<InvertedIndex>*>::iterator it_tmp_mirror;
    for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end();)
    {
//        (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改（防止出现镜像刚刚被创造出来，就有人去查询
        map<int,InvertedIndex*>::iterator it_mirror_index;
        for (it_mirror_index=((*it_tmp_mirror)->mirrorIndexMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIndexMap)->end();it_mirror_index++)
        {
            (it_mirror_index->second)->search(Result, MinScore, AnswerNum, Sum, query, name);
        }

        it_tmp_mirror++;//写在这里的原因是因为防止忘记迭代器删除这个坑
//        (*it_tmp_mirror)->mutex.Unlock();
    }


//    map<int, map<string, NodeInfo *> >::iterator it_live;
    for(map<int,double>::iterator it=Result.begin();it!=Result.end();it++)
    {
        if(myself->livePointer.find(it->first)!=myself->livePointer.end())
        {
            live_an++;
        }
    };
//    myself->clearMirror.Unlock();
    myself->clearInvertedIndex.Unlock();

    vector<pair<int, double> > tmp(Result.begin(), Result.end());
    ResVector=tmp;//这里八成或许可以优化，但是估计影响不大
    sort(ResVector.begin(), ResVector.end(), CompDedcendVal());

}

void *addAudioALLThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyAll *ones=(FamilyAll*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    IndexManager *myself=ones->me;
    map<string,double> &TagsNum=*(ones->tags);
    vector<ForMirror<InvertedIndex>* > &mirrorList=myself->mirrorList;//用于放置镜像Index的容器

    double begin,end;
    begin=getTime();
    if (tmp_info.final>=0)
    {
        myself->liveIdMutex[tmp_info.id].Lock();
        myself->mutexLive.Lock();//有意义，勿删，内部会有一个对此的判断（需要一点时间），如果没人使用，说明可以query，而且不需要阻塞
        myself->mutexLive.Unlock();
        myself->Indexes[0]->addAudioLive(tmp_info, TagsNum, myself->livePointer, myself->mutexLive);
        myself->liveIdMutex[tmp_info.id].Unlock();
    }
    else if(tmp_info.final==-1)
    {
        myself->Indexes[0]->addAudio(tmp_info,TagsNum);
    }
    end=getTime();
    AddAduioTime+=end-begin;
    myself->I0Num++;
    myself->I0TermNum+=tmp_info.Termcount;
    myself->TotalTermSum+=tmp_info.Termcount;


    if (myself->I0Num >= IndexAudioSumUnit||myself->I0TermNum>=IndexTermSumUnit)
    {
        myself->clearInvertedIndex.Lock();
//        myself->clearI0.Lock();//复制I0的过程开始了

        begin=getTime();
        myself->Indexes[0]->I0_sort();
        end=getTime();
        I0SortTime+=end-begin;

        begin=getTime();
        InvertedIndex *Index_tmp;
        (myself->liveIdMutex)[tmp_info.id].Lock();
        myself->Indexes[0]->level++;
        Index_tmp=new InvertedIndex(*myself->Indexes[0]);//这一份将会成为镜像备份，真正的Index0将会前去归并
        (myself->liveIdMutex)[tmp_info.id].Unlock();


        map<int,InvertedIndex*> *mirrorIndex=new map<int,InvertedIndex*>;//相当于备份参与归并的Index到另一个map中
        ForMirror<InvertedIndex>  *for_mirror=new ForMirror<InvertedIndex> (mirrorIndex);
        mirrorList.push_back(for_mirror);



//        for_mirror->mutex.Lock();
        (*mirrorIndex)[0]=Index_tmp;//镜像置入，新Index0创建，可以add，旧Index0可以开始merge
//        for_mirror->mutex.Unlock();
        Index_tmp=myself->Indexes[0];
        myself->Indexes[0]=new InvertedIndex;
        myself->I0Num = 0;
        myself->I0TermNum=0;
//        myself->clearI0.Unlock();
        myself->clearInvertedIndex.Unlock();


        int l=1;
        end=getTime();
        DuplicateTime+=end-begin;

        map<int, InvertedIndex*>::iterator it_index;
        while (1) {
            it_index = myself->Indexes.find(l);//判断是否存在l，如果存在直接合并，level+1
            if (it_index != myself->Indexes.end())
            {

                dense_hash_map<string,ProgramList*,my_hash<string> > &tmp_list=*(myself->Indexes[l]->TermIndex);
                begin=getTime();
                InvertedIndex *other_tmp=new InvertedIndex(*(myself->Indexes[l]));
                myself->clearInvertedIndex.Lock();
//                for_mirror->mutex.Lock();
                (*mirrorIndex)[l]=other_tmp;
//                for_mirror->mutex.Unlock();
                myself->clearInvertedIndex.Unlock();
//                myself->clearI0.Unlock();
                end=getTime();
                DuplicateTime+=end-begin;

                begin=getTime();
                myself->clearInvertedIndex.Lock();
                (*Index_tmp).MergerIndex(*(myself->Indexes[l]));


                delete myself->Indexes[l];
                end=getTime();
                pair<int,double> p(l,end-begin);
                time_of_index_merge.insert(p);
                MergeTime+=end-begin;
                MergeTimes++;


                myself->Indexes.erase(l);
                myself->clearInvertedIndex.Unlock();

                l += 1;
            }
            else
            {
                myself->clearInvertedIndex.Lock();
                myself->Indexes[l] = Index_tmp;
                myself->clearInvertedIndex.Unlock();


                myself->clearInvertedIndex.Lock();
//                myself->clearMirror.Lock();
//                for_mirror->mutex.Lock();
                map<int,InvertedIndex*>::iterator it_index;
                for (it_index=(*mirrorIndex).begin(); it_index != (*mirrorIndex).end(); it_index++)
                {
                    delete it_index->second;
                }
                delete mirrorIndex;
                for_mirror->mirrorIndexMap=NULL;
                myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),for_mirror),myself->mirrorList.end());
                delete for_mirror;
//                myself->clearMirror.Unlock();

                myself->clearInvertedIndex.Unlock();
                break;
            }
        }
    }
}

void IndexManager::InitialIdf()
{
    if(IdfTable.size()==0)
    {
        IdfTable.set_empty_key("");
    }
    double begin,end;
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey="";
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
        if (QuestionMark.compare(term_tmp) && DoubleQuestionMark.compare(term_tmp) && SpaceKey.compare(term_tmp)&&BlankKey.compare(term_tmp))
        {
            (IdfTable)[term_tmp] = atof(idf.c_str());
        }
        IdfNum++;
    }
    end=getTime();
    cout<<"Idf is okay."<<endl;
    cout<<"Time is "<<end-begin<<"s"<<endl;
}

void IndexManager::buildIndex(int audio_sum)
{
    int who =0;//0代表本进程，-1代表子进程（用不太出来），一定要bash跑，不然这玩意测的是虚拟机的内存
    struct rusage usage;
    int TagsSum,TermSum;
    double begin, end;
    begin = getTime();
    ifstream info_in("info_live.txt");
    if (!info_in) exit(7);
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey="";
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
        TagsSum = atoi(TagsSum_tmp.c_str());
        TermSum=0;
        map<string, double> TagsNum_tmp;
        for (int j = 0; j < atoi(TermSum_tmp.c_str()); j++)
        {
            getline(info_in, term_tmp);
            getline(info_in, num_tmp);
            if (QuestionMark.compare(term_tmp) && DoubleQuestionMark.compare(term_tmp) && SpaceKey.compare(term_tmp)&&BlankKey.compare(term_tmp))
            {
                (TagsNum_tmp)[term_tmp] = atof_1e(num_tmp.c_str());
                TermSum++;
            }
            else
            {
                TagsSum -= 2;//示例性的减2，没有实际意义
            }
        }
        char tmp[20];
        strcpy(tmp, id_tmp.c_str());
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, atof(score_tmp.c_str()), \
		  TagsSum, atof(time_tmp.c_str()),atoi(FinalFlag_tmp.c_str()),TermSum);
        pthread_t id;
        FamilyAll fam(this,&tmp_info,&TagsNum_tmp);
        pthread_create(&id,NULL,addAudioALLThread,(void*)&fam);
        pthread_join(id,NULL);


    }
    end = getTime();

    output();
    cout<<endl;
    getrusage(who,&usage);
    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tSumTime: "<<end-begin\
    <<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    ofstream writefile2("memory_of_index.txt",ofstream::app);
    writefile2<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    writefile2.close();
    writefile.close();
    info_in.close();
    cout<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tSumTime: "<<end-begin\
    <<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    cout<<endl;
}

string IndexManager::handleQuery(string query_str)
{
    double begin, end;
    string s;
    string stopwords = "，,";
    vector<string> query;
    vector<pair<int, double> > Result;
    map<int, string> name;

    SplitString(query_str, query, stopwords);

    FamilyQuery fam(this,&query,&name,&Result);
    pthread_t id;
    pthread_create(&id,NULL,searchThread,(void*)&fam);
    pthread_join(id,NULL);

    string str_back;
    end = getTime();
    if (Result.size() == 0)
    {
        return "NULL\n";
    }
    else
    {
        for (int i = 0; i < Result.size(); i++)
        {
            str_back += "id： " + Itos(Result[i].first) + "\n"+"title： " + name[Result[i].first] +
                        "\nlink：http://www.ximalaya.com/sound/" + Itos(Result[i].first) + "\nscore: " + Dtos(Result[i].second) + "\n\n";
        }
    }
    str_back += "Time: "+ Dtos(end - begin) + "s" +"\n";
    query.clear();
    Result.clear();
    name.clear();
    return str_back;
}

void IndexManager::updateScore(int id,int score)
{
    map<int,InvertedIndex*>::iterator it_Index;
    for(it_Index=Indexes.begin();it_Index!=Indexes.end();it_Index++)
    {
        if(it_Index->second->level!=0)
        {
            if(it_Index->second->search(id))
            {
                it_Index->second->update(id,score);
            }
        }
        else
        {
            if(it_Index->second->search(id))
            {
                it_Index->second->I0MutexInfo.Lock();
                (*it_Index->second->InfoTable)[id].score=score;
                it_Index->second->I0MutexInfo.Unlock();
            }
        }
    }
}