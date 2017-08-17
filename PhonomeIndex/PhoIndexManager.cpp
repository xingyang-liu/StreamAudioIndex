//
// Created by billy on 17-7-30.
//

#include <fcntl.h>
#include <dirent.h>
#include "PhoIndexManager.h"

void PhoIndexManager::output()
{
    ofstream writefile("Components_of_Indexes_Phonome.txt",ofstream::app);
    map<int, PhonomeIndex*>::iterator it_index;
    writefile<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<endl;
    for (it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        if(cout_flag)
        {
            cout << "I" << it_index->second->level << "_AudioCount: " << it_index->second->AudioCount <<"\tTermCount: "\
            <<it_index->second->TermCount<<"\tMergeCount: "<<it_index->second->MergeCount<< "\n";
        }
        writefile<< "I" << it_index->second->level << "_AudioCount: " << it_index->second->AudioCount <<"\tTermCount: "\
            <<it_index->second->TermCount<<"\tMergeCount: "<<it_index->second->MergeCount<< "\n";
    }
    writefile.close();
}

void PhoIndexManager::buildIndex(int audio_sum)
{
    int who =0;//0代表本进程，-1代表子进程（用不太出来），一定要bash跑，不然这玩意测的是虚拟机的内存
    struct rusage usage;
    int TagsSum,TermSum;
    double begin, end;
    begin = getTime();
    ifstream info_in("info_phonome2.txt");
    string path = "/home/watson/C++/info/";
    if (!info_in) exit(7);
//    string DoubleQuestionMark = "??";
//    string QuestionMark = "?";
//    string SpaceKey = " ";
    string LikeCount_tmp, CommentCount_tmp, PlayCount_tmp, TagsSum_tmp, score_tmp, time_tmp, title_tmp,\
 		id_tmp, TermSum_tmp,FinalFlag_tmp;
//    Phoneme term_tmp;
    int num_tmp;
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
        TagsSum = 0;
        TermSum=0;//为了统计term的数量，方便提前停止
        map<Phoneme, double> TagsNum_tmp;
        float buf[13];
        ssize_t n;
        string filename = path + id_tmp + ".mfcc";
        int fd = open(filename.c_str(), O_RDONLY);
        if (fd == 0) {
//            cout << "No such files!" << endl;
            continue;
        }
        while ((n=read(fd, (void *)buf, 13*4))>0) {
            Phoneme term_tmp = Phoneme(buf);
            map<Phoneme, double>::iterator insertor;
            insertor = TagsNum_tmp.find(term_tmp);
            if ((n = read(fd, &num_tmp, sizeof(num_tmp)))){
                if (insertor != TagsNum_tmp.end()) {
                    TagsNum_tmp[insertor->first] = num_tmp;
                    TagsSum += num_tmp;
//                    cout << "here\n";
                }
                else {
//                    IdfTablePho[term_tmp] = log(500);
                    TagsNum_tmp[term_tmp] = num_tmp;
                    TagsSum += num_tmp;
                }
            }
            TermSum++;
                
        }
//        char tmp[20];
//        strcpy(tmp, id_tmp.c_str());
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, atof(score_tmp.c_str()), \
		  TagsSum, atof(time_tmp.c_str()), atoi(FinalFlag_tmp.c_str()),TermSum);
        pthread_t id;
        FamilyPho fam(this,&tmp_info,&TagsNum_tmp);
        pthread_create(&id,NULL,addAudioPhoThread,(void*)&fam);
        pthread_join(id,NULL);


    }
    end = getTime();


    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tSumTime: "<<end-begin\
    <<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    ofstream writefile2("memory_of_index.txt",ofstream::app);
    writefile2<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    writefile2.close();
    writefile.close();
    info_in.close();
    if(cout_flag)
    {
        output();
        cout<<endl;
        getrusage(who,&usage);
        cout<<"Sum: "<<AudioSum<<"\tTotalTermSum: "<<TotalTermSum<<"\tIndexTermUnit: "<<IndexTermSumUnit<<"\tSumTime: "<<end-begin\
        <<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
        cout<<endl;
    }

}

void *addAudioPhoThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyPho *ones=(FamilyPho*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    PhoIndexManager *myself=ones->me;
    map<Phoneme,double> &TagsNum=*(ones->tags);
    vector<ForMirror<PhonomeIndex>* > &mirrorList=myself->mirrorList;

    double begin,end;
    begin=getTime();
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
    end=getTime();
    AddAduioTime+=end-begin;
    myself->I0Num++;
    myself->I0TermNum+=tmp_info.Termcount;
    myself->TotalTermSum+=tmp_info.Termcount;


    if (myself->I0Num >= IndexAudioSumUnit||myself->I0TermNum>=IndexTermSumUnit)
    {
//        myself->clearI0.Lock();//复制I0的过程开始了
//        ProgramList* test = myself->Indexes[0]->TermIndex->begin()->second;
        myself->clearInvertedIndex.Lock();
        begin=getTime();
        myself->Indexes[0]->I0_sort();
        end=getTime();
        I0SortTime+=end-begin;

        begin=getTime();
        PhonomeIndex *Index_tmp;
        (myself->liveIdMutex)[tmp_info.id].Lock();
        myself->Indexes[0]->level++;
        Index_tmp=new PhonomeIndex(*myself->Indexes[0]);
        (myself->liveIdMutex)[tmp_info.id].Unlock();
//        Index_tmp->level+=1;
//        map<string,ProgramList*> &tmp_list=*(Index_tmp->TermIndex);

        auto *mirrorIndex=new map<int,PhonomeIndex*>;//相当于独立出参与归并的Index到另一个map中
        auto *for_mirror=new ForMirror<PhonomeIndex> (mirrorIndex);
        mirrorList.push_back(for_mirror);


        (*mirrorIndex)[0]=Index_tmp;
//        InvertedIndex &other=(*(*mirrorIndex)[0]);
        Index_tmp=myself->Indexes[0];


        myself->Indexes[0]=new PhonomeIndex;
        myself->I0Num = 0;
        myself->I0TermNum=0;
//        myself->clearI0.Unlock();
//        myself->clearI0.Unlock();
        myself->clearInvertedIndex.Unlock();

        int l=1;
        end=getTime();
        DuplicateTime+=end-begin;

        map<int, PhonomeIndex*>::iterator it_index;
        while (1) {
            it_index = myself->Indexes.find(l);

            if (it_index != myself->Indexes.end())
            {
                map<int,PhonomeIndex*> &tmp=myself->Indexes;
                map<Phoneme,ProgramList*> &tmp_list1=*(Index_tmp->TermIndex);
                map<Phoneme,ProgramList*> &tmp_list=*(myself->Indexes[l]->TermIndex);
                PhonomeIndex *other_tmp=new PhonomeIndex(*(myself->Indexes[l]));
                myself->clearInvertedIndex.Lock();
                (*mirrorIndex)[l]=other_tmp;//可能需要加互斥锁
                myself->clearInvertedIndex.Unlock();
                end=getTime();
                DuplicateTime+=end-begin;

                if(myself->Indexes[l]->MergeCount+Index_tmp->MergeCount>ratio-1)
                {
                    begin=getTime();
                    myself->clearInvertedIndex.Lock();
                    Index_tmp->level++;
                    Index_tmp->MergeCount=1;
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
                    begin=getTime();
                    myself->clearInvertedIndex.Lock();
                    Index_tmp->MergeCount+=myself->Indexes[l]->MergeCount;
                    (*Index_tmp).MergerIndex(*(myself->Indexes[l]));
                    delete myself->Indexes[l];
                    end=getTime();
                    pair<int,double> p(l,end-begin);
                    time_of_index_merge.insert(p);
                    MergeTime+=end-begin;
                    MergeTimes++;
                    (myself->Indexes)[l]=Index_tmp;
                    map<int,PhonomeIndex*>::iterator it_index;
                    for (it_index=(*mirrorIndex).begin(); it_index != (*mirrorIndex).end(); it_index++)
                    {
                        delete it_index->second;
                    }
                    delete mirrorIndex;
                    for_mirror->mirrorIndexMap=NULL;
                    myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),for_mirror),myself->mirrorList.end());
                    delete for_mirror;
                    myself->clearInvertedIndex.Unlock();
                    break;
                }
            }
            else
            {
                myself->clearInvertedIndex.Lock();
                myself->Indexes[l] = Index_tmp;
                Index_tmp->MergeCount=1;
                myself->clearInvertedIndex.Unlock();


                myself->clearInvertedIndex.Lock();
//                myself->clearMirror.Lock();
//                for_mirror->mutex.Lock();
                map<int,PhonomeIndex*>::iterator it_index;
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

string PhoIndexManager::handleQuery(vector<SimilarPhoneme> phones) {
    vector<Phoneme> query;


    for (auto &phone : phones) {
        auto findPhone = IdfTablePho.find(phone);
        if (findPhone == IdfTablePho.end()) {
            for (findPhone = IdfTablePho.begin(); findPhone != IdfTablePho.end() ; findPhone++) {
                if (findPhone->first == phone) {
                    Phoneme tmp = Phoneme(findPhone->first);
                    query.push_back(tmp);
                    break;
                }
            }
            continue;
        }
        Phoneme tmp = Phoneme(findPhone->first);
        query.push_back(tmp);
    }
    if (query.size() == 0)
        return "No Results Found\n";
    return handleQuery(query);
}

string PhoIndexManager::handleQuery(vector<Phoneme> query)
{

    Phoneme s;

    double begin, end;
    vector<pair<int, double> > Result;
    map<int, string> name;
    map<int,score_ratio> sco_vec;

    FamilyPhoQuery fam(this,&query,&name,&Result,&sco_vec);
    pthread_t id;
    pthread_create(&id,NULL,searchPhoThread,(void*)&fam);
    pthread_join(id,NULL);

    string str_back;
    end = getTime();
    if (Result.size() == 0)
    {
//        cout << "NULL" << endl;
        return "No Results Found\n";
//        return query_str;
    }
    else
    {
        for (int i = 0; i < Result.size(); i++)
        {
            str_back += "id： " + Itos(Result[i].first) + "\n"+"title： " + name[Result[i].first] +
                        "\nlink：http://www.ximalaya.com/sound/" + Itos(Result[i].first) + "\nscore:\t" + Dtos(Result[i].second) +"\tfre:\t"+\
                        Dtos(sco_vec[Result[i].first].fre)+"\tsig:\t"+Dtos(sco_vec[Result[i].first].sig)+"\tsim:\t"+Dtos(sco_vec[Result[i].first].sim)+ "\n\n";
        }
    }
    str_back += "Time: "+ Dtos(end - begin) +"\n";

    query.clear();
    Result.clear();
    name.clear();

    return str_back;
}

void *searchPhoThread(void *family)
{
    FamilyPhoQuery *fam=(FamilyPhoQuery *)family;
    PhoIndexManager *myself=fam->me;
    vector<Phoneme> &query=*(fam->que);
    vector<pair<int,double> > &ResVector=*(fam->ResVec);
    map<int,string> &name=*(fam->na);
    map<int,score_ratio> &sco_vec=*(fam->sco_ve);


    map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;

    vector<double> idf_vec;
    map<SimilarPhoneme, double>::iterator it_idf;
    for (const auto &i : query)
    {
        it_idf=IdfTablePho.find(SimilarPhoneme(i));
        if(it_idf!=IdfTablePho.end())
        {
            idf_vec.push_back(IdfTablePho[SimilarPhoneme(i)]);
        }else{
            idf_vec.push_back(0);
        }
    }


    myself->clearInvertedIndex.Lock();
//    myself->clearI0.Lock();
    myself->Indexes[0]->I0MutexInfo.Lock();
    myself->Indexes[0]->search(Result, MinScore, AnswerNum, Sum, query, name,sco_vec,idf_vec);
    myself->Indexes[0]->I0MutexInfo.Unlock();
//    myself->clearI0.Unlock();

    map<int, PhonomeIndex*>::iterator it_index;
    //这里包括镜像在内所有PhonomeIndex都不允许变化
    for (it_index = myself->Indexes.begin(); it_index != myself->Indexes.end(); it_index++)
    {
        if(it_index->first==0) continue;
        (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name,sco_vec,idf_vec);
    }

    vector<ForMirror<PhonomeIndex>*>::iterator it_tmp_mirror;
    for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
    {
//        (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改

        map<int,PhonomeIndex*>::iterator it_mirror_index;
        for (it_mirror_index=((*it_tmp_mirror)->mirrorIndexMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIndexMap)->end();it_mirror_index++)
        {
            (it_mirror_index->second)->search(Result, MinScore, AnswerNum, Sum, query, name,sco_vec,idf_vec);
        }

        it_tmp_mirror++;

//        (*it_tmp_mirror)->mutex.Unlock();

    }
    myself->clearInvertedIndex.Unlock();

    vector<pair<int, double> > tmp(Result.begin(), Result.end());
    ResVector=tmp;
    sort(ResVector.begin(), ResVector.end(), CompDedcendVal());


}

void PhoIndexManager::InitialIdf() {
    double begin,end;
    IdfNum=0;
    double idf;
    int fd = open("idf.mfcc", O_RDONLY);
    if (!fd)
        exit(7);
    else {
        begin=getTime();
        if(cout_flag)
        {
            cout << "Begin idf." << endl;
        }
    }
    ssize_t n;
    float buf[13];
    while ((n=read(fd, (void *)buf, 13*4))>0) {
        SimilarPhoneme term_tmp = SimilarPhoneme(buf);
        if ((n=read(fd, &idf, sizeof(idf))) > 0){
            IdfTablePho[term_tmp] = idf;
        }
        IdfNum++;
    }
    end=getTime();
    if(cout_flag)
    {
        cout<<"Idf is okay."<<endl;
        cout<<"Time is "<<end-begin<<"s"<<endl;
        cout<<"IdfNum is "<<IdfNum<<endl;
    }
//    cout<<"Idf is okay."<<endl;
//    cout<<"Time is "<<end-begin<<"s"<<endl;
}

void initialInfo(string path) {
    ////////////////读取所有文件//////////////////
    vector<string> files;
    DIR* dir;
    struct dirent* ptr;
    int count = 0;
    dir = opendir(path.c_str());
    while((ptr=readdir(dir)) != NULL) {
//        cout << ptr->d_name << endl;
        string tmp = ptr->d_name;
        if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, "..")) {
            files.push_back(tmp);
            ++count;
        }
    }
    closedir(dir);
    ////////////////寻找相同的phonome//////////////////
    map<SimilarPhoneme, count_node> phones;
    vector<string>::iterator file;
    int fcount = 0;
    for (file = files.begin(); file != files.end() ; ++file) {
        cout << ++fcount << ": " << *file << endl;
        float buf[13];
        ssize_t n;
        string filename = path + *file;
//        map<Phoneme, int> phones;
        int fd = open(filename.c_str(), O_RDONLY);
        while ((n=read(fd, (void *)buf, 13*4))>0) {
            SimilarPhoneme term_tmp = SimilarPhoneme(buf);
            map<SimilarPhoneme, count_node>::iterator it = phones.find(term_tmp);
            if (it == phones.end()) {
                phones[term_tmp].current = 1;
                phones[term_tmp].overall = 1;
//                cout << "no!\n";
            }
            else {
                it->second.current += 1;
                it->second.overall += 1;
//                if (it->first == term_tmp) cout << "yes!\n";
            }
        }
        //////////////////写入文件/////////////////////
        close(fd);
        filename = "/media/billy/New Volume/Billy/info/";
        filename += file->c_str();
        ofstream myFile (filename.c_str(), ios::out | ios::binary);
        map<SimilarPhoneme, count_node>::iterator test;
        for (test = phones.begin(); test != phones.end(); ++test) {
            if (test->second.current == 0) continue;
            char phonebuf[13*4];
            test->first.output(phonebuf);
            myFile.write(phonebuf, 13*4);
            myFile.write((char*)&(test->second.current), 4);
            test->second.current = 0;
            test->second.filesNum += 1;
        }
        myFile.close();
//        break;
    }
    unsigned long filesTotal = files.size();
    ofstream idfFile ("idf.mfcc", ios::out | ios::binary);
    ofstream out ("result.txt");
    map<SimilarPhoneme, count_node>::iterator test;
    for (test = phones.begin(); test != phones.end(); ++test) {
        char phonebuf[13*4];
        test->first.output(phonebuf);
        idfFile.write(phonebuf, 13*4);
        double idf = log10((double)filesTotal/(test->second.filesNum + 1));
        idfFile.write((char*)&idf, 8);
        out << Itos(test->second.filesNum) << " " << Itos(test->second.overall) << endl;
    }
    out.close();
    idfFile.close();
}