////
//// Created by billy on 17-7-30.

#include <dirent.h>
#include "PhoIndexManager.h"

void PhoIndexManager::output()
{
    for (auto it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        (*it_index->second).output();
    }
}

void PhoIndexManager::buildIndex(int audio_sum)
{
    double begin, end;
    begin = getTime();
    ifstream info_in("../resouce/info_pho10000.txt");
    string path = "/media/billy/New Volume/Billy/info/";
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
        int TagsSum = 0, termSum = 0;
        btree::btree_map<int, double> TagsNum_tmp;
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
            auto insertor = IdfTable.find(term_tmp);
            if ((n = read(fd, &num_tmp, sizeof(num_tmp)))){
                if (insertor != IdfTable.end()) {
                    TagsNum_tmp[insertor->second.code] = num_tmp;
                    TagsSum += num_tmp;
                    termSum++;
                }
                else {
                    IdfTable[term_tmp] = IdfCode(1, IdfCodeNum);
                    TagsNum_tmp[IdfCodeNum] = num_tmp;
                    TagsSum += num_tmp;
                    IdfCodeNum++;
                    termSum++;
                }
            }

        }
//        char tmp[20];
//        strcpy(tmp, id_tmp.c_str());
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, atof(score_tmp.c_str()), \
		  TagsSum, atof(time_tmp.c_str()), atoi(FinalFlag_tmp.c_str()));
        pthread_t id;
        FamilyPho fam(this,&tmp_info,&TagsNum_tmp,termSum);
        pthread_create(&id,NULL,addAudioPhoThread,(void*)&fam);
        pthread_join(id,NULL);


    }
    end = getTime();

//    output();
    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<audioSum<<"\tTotalTermSum:"<<totalTermSum<<"\tIndexTermUnit: "<<indexTermSumUnit<<" SumTime: "<<end-begin\
    <<" Times: "<<1<<setprecision(8)<<" Average: "<<end-begin<<endl;
//    cout << end - begin << "s" << endl;
    writefile.close();
    info_in.close();
}

void PhoIndexManager::addAudio(AudioInfo &audio, vector<SimilarPhoneme> &content) {
    map<SimilarPhoneme, int> termMap;
    btree::btree_map<int, double> TagsNum_tmp;

    int tagsSum = 0;
    int termSum = 0;

    ////////////////计算term的数量//////////////////
    for (auto it_tmp = content.begin(); it_tmp != content.end() ; it_tmp++) {
        auto itTerm = termMap.find(*it_tmp);
        if (itTerm != termMap.end()) {
            itTerm->second++;
        }
        else {
            for (itTerm = termMap.begin(); itTerm != termMap.end() ; itTerm++) {
                if (itTerm->first == *it_tmp) {
                    itTerm->second++;
                    break;
                }
            }
            if (itTerm == termMap.end()) {
                termMap[*it_tmp] = 1;
            }
        }
        tagsSum++;
    }
    ///////////////更新IDF表/////////////////////
    for (auto itTerm = termMap.begin(); itTerm != termMap.end(); ++itTerm) {
        auto it_idf = IdfTable.find(itTerm->first);
        if (it_idf != IdfTable.end()) {
            it_idf->second.idf++;
            (TagsNum_tmp)[it_idf->second.code] = itTerm->second/(tagsSum+1.0);
        }
        else {
            IdfTable[it_idf->first] = IdfCode(1.0, IdfCodeNum);
            (TagsNum_tmp)[IdfCodeNum] = itTerm->second/(tagsSum+1.0);
            IdfCodeNum++;
        }
        termSum++;
    }
    pthread_t pid;
    FamilyPho fam(this,&audio,&TagsNum_tmp,termSum);
    pthread_create(&pid, NULL, addAudioPhoThread, (void *) &fam);
    pthread_join(pid,NULL);
}

void *addAudioPhoThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyPho *ones=(FamilyPho*) Family;
    AudioInfo &tmp_info=(*ones->audio);
    PhoIndexManager *myself=ones->me;
    btree::btree_map<int,double> &TagsNum=*(ones->tags);
    vector<ForMirror<InvertedIndex>* > &mirrorList=myself->mirrorList;
    int tCount=(ones->termCount);
    myself->AudioSum++;

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
    myself->I0TermNum+=tCount;
    myself->totalTermSum+=tCount;
    myself->Indexes[0]->termCount+=tCount;


    if (myself->I0Num >= indexAudioSumUnit||myself->I0TermNum>=indexTermSumUnit)
    {
        myself->clearInvertedIndex.Lock();//复制I0的过程开始了
        ProgramList* test = myself->Indexes[0]->TermIndex->begin()->second;
        myself->Indexes[0]->I0_sort();


        InvertedIndex *Index_tmp;
        (myself->liveIdMutex)[tmp_info.id].Lock();
        Index_tmp=new InvertedIndex(*myself->Indexes[0]);
        (myself->liveIdMutex)[tmp_info.id].Unlock();
        Index_tmp->level+=1;
//        map<string,ProgramList*> &tmp_list=*(Index_tmp->TermIndex);

        auto *mirrorIndex=new btree::btree_map<int,InvertedIndex*>;//相当于独立出参与归并的Index到另一个map中
        auto *for_mirror=new ForMirror<InvertedIndex> (mirrorIndex);
        mirrorList.push_back(for_mirror);

        (*mirrorIndex)[0]=Index_tmp;
//        InvertedIndex &other=(*(*mirrorIndex)[0]);
        Index_tmp=myself->Indexes[0];
        Index_tmp->level+=1;

        myself->Indexes[0]=new InvertedIndex;
        myself->I0Num = 0;
        myself->clearInvertedIndex.Unlock();
        int l=1;

        btree::btree_map<int, InvertedIndex*>::iterator itIndex;
        while (1) {
            itIndex = myself->Indexes.find(l);//判断是否存在l，如果存在直接合并，level+1
            if (itIndex != myself->Indexes.end())
            {

//                btree::btree_map<string,ProgramList*> &tmp_list=*(myself->Indexes[l]->TermIndex);
                begin=getTime();
//                cout << "start to new I1" << endl;
//                sleep(10);
                InvertedIndex *otherTmp=new InvertedIndex(*(myself->Indexes[l]));
//                cout << "finish creating I1: " << otherTmp << endl;
//                sleep(10);

                myself->clearInvertedIndex.Lock();
                (*mirrorIndex)[l]=otherTmp;
                myself->clearInvertedIndex.Unlock();

                end=getTime();
                duplicateTime+=end-begin;


                if(myself->Indexes[l]->mergeCount+Index_tmp->mergeCount>ratio-1)
                {
                    begin=getTime();
                    myself->clearInvertedIndex.Lock();
                    Index_tmp->level++;
                    Index_tmp->mergeCount=1;
                    (*Index_tmp).mergerIndex(*(myself->Indexes[l]));
                    delete myself->Indexes[l];
                    end=getTime();


                    pair<int,double> p(l,end-begin);
                    timeOfIndexMerge.insert(p);
                    mergeTime+=end-begin;
                    mergeTimes++;
                    myself->Indexes.erase(l);
                    myself->clearInvertedIndex.Unlock();
                    l += 1;
                }
                else
                {
                    begin=getTime();
                    myself->clearInvertedIndex.Lock();
                    Index_tmp->mergeCount+=myself->Indexes[l]->mergeCount;
                    (*Index_tmp).mergerIndex(*(myself->Indexes[l]));
                    delete myself->Indexes[l];
                    end=getTime();

                    pair<int,double> p(l,end-begin);
                    timeOfIndexMerge.insert(p);
                    mergeTime+=end-begin;
                    mergeTimes++;
                    (myself->Indexes)[l]=Index_tmp;

                    btree::btree_map<int,InvertedIndex*>::iterator itMirrorIndex;
                    for (itMirrorIndex=(*mirrorIndex).begin(); itMirrorIndex != (*mirrorIndex).end(); itMirrorIndex++)
                    {
                        delete itMirrorIndex->second;
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
                Index_tmp->mergeCount=1;
                myself->clearInvertedIndex.Unlock();


                myself->clearInvertedIndex.Lock();
                btree::btree_map<int,InvertedIndex*>::iterator it_index;
                for (it_index=(*mirrorIndex).begin(); it_index != (*mirrorIndex).end(); it_index++)
                {
//                    cout<<"Delete Mirror id: "<<it_index->first << "; addr: " << it_index->second <<endl;
                    delete it_index->second;
                }
//                if(mirrorIndex != forMirror->mirrorIndexMap)
//                    cout << "oh my god..............." << endl;
                delete mirrorIndex;
                for_mirror->mirrorIndexMap=NULL;
                myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),for_mirror),myself->mirrorList.end());
                delete for_mirror;
                myself->clearInvertedIndex.Unlock();
                break;
            }
        }
    }
}

void PhoIndexManager::handleQuery(vector<SimilarPhoneme> phones, vector<string> Result) {
    vector<int> queryCode;
    vector<double> queryIdf;

    for (auto &phone : phones) {
        auto findPhone = IdfTable.find(phone);
        if (findPhone == IdfTable.end()) {
            for (findPhone = IdfTable.begin(); findPhone != IdfTable.end() ; findPhone++) {
                if (findPhone->first == phone) {
                    int tmpCode = findPhone->second.code;
                    double tmpIdf = findPhone->second.idf;
                    queryCode.push_back(tmpCode);
                    queryIdf.push_back(log(audioSum/(tmpIdf+1.0)));
                    break;
                }
            }
            continue;
        }
        int tmpCode = findPhone->second.code;
        double tmpIdf = findPhone->second.idf;
        queryCode.push_back(tmpCode);
        queryIdf.push_back(tmpIdf);
    }
    if (queryCode.size() == 0)
        return;
    handleQuery(queryCode, queryIdf, Result);
    return;
}

void PhoIndexManager::handleQuery(vector<int> queryCode, vector<double> queryIdf, vector<string> Result)
{

    Phoneme s;

    double begin, end;
    vector<pair<int, double> > resultVec;
    btree::btree_map<int, string> name;
    btree::btree_map<int,score_ratio> scoVec;

    FamilyPhoQuery fam(this,&queryCode,&queryIdf,&name,&resultVec,&scoVec);
    pthread_t id;
    pthread_create(&id,NULL,searchPhoThread,(void*)&fam);
    pthread_join(id,NULL);

    const char* resultTemplate = " { \"id\" : 0, \"title\" : \" \" , \"link\" : \" \", \" score\": 0.0 } ";
    end = getTime();
    if (resultVec.size() == 0)
    {
//        cout << "NULL" << endl;
        return;
//        return query_str;
    }
    else
    {
        for (int i = 0; i < resultVec.size(); i++)
        {
            rapidjson::Document jsonBack;
            jsonBack.Parse(resultTemplate);
            rapidjson::Value &id = jsonBack["id"];
            id.SetInt(resultVec[i].first);
            rapidjson::Value &title = jsonBack["title"];
            title.SetString(name[resultVec[i].first].c_str(), name[resultVec[i].first].size(), jsonBack.GetAllocator());
            rapidjson::Value &link = jsonBack["link"];
            string strLink = "http://www.ximalaya.com/sound/" + Itos(resultVec[i].first);
            link.SetString(strLink.c_str(), strLink.size(), jsonBack.GetAllocator());
            rapidjson::Value &score = jsonBack["score"];
            score.SetDouble(resultVec[i].second);
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            jsonBack.Accept(writer);
            string strBack = buffer.GetString();
            Result.push_back(strBack);
        }
    }
    string strBack = "Time: "+ Dtos(end - begin) +"\n";
    Result.push_back(strBack);

    queryCode.clear();
    resultVec.clear();
    name.clear();

    return;
}

void *searchPhoThread(void *family)
{
    FamilyPhoQuery *fam=(FamilyPhoQuery *)family;
    PhoIndexManager *myself=fam->me;
    vector<int> &query_code=*(fam->que_code);
    vector<double> &query_idf = *(fam->que_idf);
    vector<pair<int,double> > &ResVector=*(fam->resVec);
    btree::btree_map<int,string> &nameMap=*(fam->na);
    btree::btree_map<int,score_ratio> &scoreVec=*(fam->sco_ve);


    btree::btree_map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;
    try
    {
        myself->clearInvertedIndex.Lock();
        myself->Indexes[0]->I0MutexInfo.Lock();
        myself->Indexes[0]->search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
        myself->Indexes[0]->I0MutexInfo.Unlock();
        myself->clearInvertedIndex.Unlock();

        myself->clearInvertedIndex.Lock();//在清理整个InvertedIndex//这里包括镜像在内所有InvertedIndex都不允许变化
        for (auto it_index = myself->Indexes.begin(); it_index != myself->Indexes.end(); it_index++)
        {
            if(it_index->first==0) continue;
            (*it_index->second).search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
        }

        for (auto it_tmp_mirror = myself->mirrorList.begin(); it_tmp_mirror != myself->mirrorList.end(); )
        {
//            (*it_tmp_mirror)->mutex.Lock();//当我进入某个镜像时，不允许该镜像修改

            for (auto it_mirror_index=((*it_tmp_mirror)->mirrorIndexMap)->begin();it_mirror_index!=((*it_tmp_mirror)->mirrorIndexMap)->end();it_mirror_index++)
            {
                (it_mirror_index->second)->search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
            }

            it_tmp_mirror++;

//            (*it_tmp_mirror)->mutex.Unlock();

        }
        myself->clearInvertedIndex.Unlock();

        vector<pair<int, double> > tmp(Result.begin(), Result.end());
        ResVector=tmp;
        sort(ResVector.begin(), ResVector.end(), CompDedcendVal());
    }
    catch (...)
    {
//        cout << "Search has somthing wrong." << endl;
    }
}

void PhoIndexManager::InitialIdf() {
    double begin,end;
    idfNum=0;
    double idf;
    int fd = open("../resouce/idf.mfcc", O_RDONLY);
    if (!fd)
        exit(7);
    else {
        begin=getTime();
//        cout << "Begin idf." << endl;
    }
    ssize_t n;
    float buf[13];
    while ((n=read(fd, (void *)buf, 13*4))>0) {
        SimilarPhoneme term_tmp = SimilarPhoneme(buf);
        if ((n=read(fd, &idf, sizeof(idf))) > 0){
            IdfTable[term_tmp] = IdfCode(idf, IdfCodeNum);
        }
        idfNum++;
        IdfCodeNum++;
    }
    end=getTime();
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
    ////////////////用singleword初始化/////////////////
    map<SimilarPhoneme, count_node> phones;
    char singleword[255] = "/media/billy/New Volume/Billy/single_word.mfcc";
    int fd = open(singleword, O_RDONLY);
    int pcount = 0;
    float buf[13];
    ssize_t n;
//    while ((n=read(fd, (void *)buf, 13*4))>0) {
//        SimilarPhoneme term_tmp = SimilarPhoneme(buf);
//        map<SimilarPhoneme, count_node>::iterator it = phones.find(term_tmp);
//        if (it == phones.end()) {
//            for (it = phones.begin(); it != phones.end(); ++it) {
//                if (it->first == term_tmp){
//                    it->second.current += 1;
//                    it->second.overall += 1;
//                }
//            }
//            if (it == phones.end()) {
//                phones[term_tmp].current = 1;
//                phones[term_tmp].overall = 1;
//                pcount += 1;
////                    cout << "no!\n";
//            }
//        }
//        else {
//            it->second.current += 1;
//            it->second.overall += 1;
////                if (it->first == term_tmp) cout << "yes!\n";
//        }
//    }
    ////////////////寻找相同的phonome//////////////////
    vector<string>::iterator file;
    int fcount = 0;
    for (file = files.begin(); file != files.end() ; ++file) {
        cout << ++fcount << ": " << *file << endl;

        string filename = path + *file;
//        map<Phoneme, int> phones;
        fd = open(filename.c_str(), O_RDONLY);
        while ((n=read(fd, (void *)buf, 13*4))>0) {
            SimilarPhoneme term_tmp = SimilarPhoneme(buf);
            map<SimilarPhoneme, count_node>::iterator it = phones.find(term_tmp);
            if (it == phones.end()) {
                for (it = phones.begin(); it != phones.end(); ++it) {
                    if (it->first == term_tmp){
                        it->second.current += 1;
                        it->second.overall += 1;
                    }
                }
                if (it == phones.end()) {
                    phones[term_tmp].current = 1;
                    phones[term_tmp].overall = 1;
                    pcount += 1;
//                    cout << "no!\n";
                }
            }
            else {
                it->second.current += 1;
                it->second.overall += 1;
//                if (it->first == term_tmp) cout << "yes!\n";
            }
        }
        //////////////////写入文件/////////////////////
        close(fd);
        filename = "/media/billy/New Volume/Billy/info1/";
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
        double idf = test->second.filesNum;
        idfFile.write((char*)&idf, 8);
        out << Itos(test->second.filesNum) << " " << Itos(test->second.overall) << endl;
    }
    out.close();
    idfFile.close();
}