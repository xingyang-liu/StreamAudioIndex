//
// Created by billy on 17-7-30.
//

#include <fcntl.h>
#include <dirent.h>
#include "PhoIndexManager.h"

void PhoIndexManager::output()
{

    map<int, PhonomeIndex*>::iterator it_index;
    for (it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        (*it_index->second).output();
    }
}

void PhoIndexManager::buildIndex(int audio_sum)
{
    double begin, end;
    begin = getTime();
    ifstream info_in("info_phonome.txt");
    string path = "/media/billy/Braavos/Billy/info/";
    if (!info_in) exit(7);
//    string DoubleQuestionMark = "??";
//    string QuestionMark = "?";
//    string SpaceKey = " ";
    string LikeCount_tmp, CommentCount_tmp, PlayCount_tmp, TagsSum_tmp, score_tmp, time_tmp, title_tmp,\
 		id_tmp, TermSum_tmp,FinalFlag_tmp;
//    Phonome term_tmp;
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
        int TagsSum = 0;
        map<Phonome, double> TagsNum_tmp;
        float buf[13];
        ssize_t n;
        string filename = path + id_tmp + ".mfcc";
        int fd = open(filename.c_str(), O_RDONLY);
        if (!fd) continue;
        while ((n=read(fd, (void *)buf, 13*4))>0) {
            Phonome term_tmp = Phonome(buf);
            if ((n = read(fd, &num_tmp, sizeof(num_tmp)))){
                TagsNum_tmp[term_tmp] = num_tmp;
                TagsSum += 1;
            }
                
        }
//        char tmp[20];
//        strcpy(tmp, id_tmp.c_str());
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, atof(score_tmp.c_str()), \
		  TagsSum, atof(time_tmp.c_str()), atoi(FinalFlag_tmp.c_str()));
        pthread_t id;
        FamilyPho fam(this,&tmp_info,&TagsNum_tmp);
        pthread_create(&id,NULL,addAudioPhoThread,(void*)&fam);
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

void *addAudioPhoThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyPho *ones=(FamilyPho*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    PhoIndexManager *myself=ones->me;
    map<Phonome,double> &TagsNum=*(ones->tags);
    vector<ForMirror<PhonomeIndex>* > &mirrorList=myself->mirrorList;


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


        PhonomeIndex *Index_tmp;
        (myself->liveIdMutex)[tmp_info.id].Lock();
        Index_tmp=new PhonomeIndex(*myself->Indexes[0]);
        (myself->liveIdMutex)[tmp_info.id].Unlock();
        Index_tmp->level+=1;
//        map<string,ProgramList*> &tmp_list=*(Index_tmp->TermIndex);

        map<int,PhonomeIndex*> *mirrorIndex=new map<int,PhonomeIndex*>;//相当于独立出参与归并的Index到另一个map中
        ForMirror<PhonomeIndex>  *for_mirror=new ForMirror<PhonomeIndex> (mirrorIndex);
        mirrorList.push_back(for_mirror);

        (*mirrorIndex)[0]=Index_tmp;
//        InvertedIndex &other=(*(*mirrorIndex)[0]);
        Index_tmp=myself->Indexes[0];
        Index_tmp->level+=1;

        myself->Indexes[0]=new PhonomeIndex;
        myself->I0Num = 0;
        myself->clearI0.Unlock();
        int l=1;

        map<int, PhonomeIndex*>::iterator it_index;
        while (1) {
            it_index = myself->Indexes.find(l);


            if (it_index != myself->Indexes.end())
            {
                map<int,PhonomeIndex*> &tmp=myself->Indexes;
                map<Phonome,ProgramList*> &tmp_list1=*(Index_tmp->TermIndex);
                map<Phonome,ProgramList*> &tmp_list=*(myself->Indexes[l]->TermIndex);
                PhonomeIndex *other_tmp=new PhonomeIndex(*(myself->Indexes[l]));
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
                map<int,PhonomeIndex*>::iterator it_index;
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

string PhoIndexManager::handleQuery(vector<Phonome> query)
{

    Phonome s;

    double begin, end;
    string stopwords = "，";
    vector<pair<int, double> > Result;
    map<int, string> name;

    FamilyPhoQuery fam(this,&query,&name,&Result);
    pthread_t id;
    pthread_create(&id,NULL,searchPhoThread,(void*)&fam);
    pthread_join(id,NULL);

//    Result = Index.search(query, name);
//    cout << name.size() << endl;

//    out_res << query_str << endl;

    string str_back;
    end = getTime();
    if (Result.size() == 0)
    {
//        cout << "NULL" << endl;
        return NULL;
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

void *searchPhoThread(void *family)
{
    FamilyPhoQuery *fam=(FamilyPhoQuery *)family;
    PhoIndexManager *myself=fam->me;
    vector<Phonome> &query=*(fam->que);
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

        map<int, PhonomeIndex*>::iterator it_index;
        myself->clearInvertedIndex.Lock();//在清理整个PhonomeIndex//这里包括镜像在内所有PhonomeIndex都不允许变化
        for (it_index = myself->Indexes.begin(); it_index != myself->Indexes.end(); it_index++)
        {
            if(it_index->first==0) continue;
            (*it_index->second).search(Result, MinScore, AnswerNum, Sum, query, name);
        }

        vector<ForMirror<PhonomeIndex>*>::iterator it_tmp_mirror;
        for (it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
        {
            (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改

            map<int,PhonomeIndex*>::iterator it_mirror_index;
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
    vector<string>::iterator file;
    for (file = files.begin(); file != files.end() ; ++file) {
        cout << *file << endl;
        float buf[13];
        ssize_t n;
        string filename = path + *file;
        map<Phonome, int> phones;
        int fd = open(filename.c_str(), O_RDONLY);
        while ((n=read(fd, (void *)buf, 13*4))>0) {
            Phonome term_tmp = Phonome(buf);
            map<Phonome, int>::iterator it = phones.find(term_tmp);
            if (it == phones.end()) {
                phones[term_tmp] = 1;
//                cout << "no!\n";
            }
            else {
                it->second += 1;
//                if (it->first == term_tmp) cout << "yes!\n";
            }
        }
//        cout << *file << endl;
        close(fd);
        filename = "/media/billy/Braavos/Billy/info/";
        filename += file->c_str();
        ofstream myFile (filename.c_str(), ios::out | ios::binary);
        map<Phonome, int>::iterator test;
        for (test = phones.begin(); test != phones.end(); ++test) {
            char phonebuf[13*4];
            test->first.output(phonebuf);
            myFile.write(phonebuf, 13*4);
            myFile.write((char*)&(test->second), 4);
        }
        myFile.close();
//        break;
    }

}