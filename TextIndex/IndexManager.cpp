
#include "IndexManager.h"

void IndexManager::output()//输出每个Index的Info
{
    ofstream writefile("Components of Indexes.txt",ofstream::app);
    btree::btree_map<int, InvertedIndex*>::iterator it_index;
    writefile<<"Sum: "<<audioSum<<"\tTotalTermSum: "<<totalTermSum<<"\tIndexTermUnit: "<<indexTermSumUnit<<endl;
    for (it_index = Indexes.begin(); it_index != Indexes.end(); it_index++)
    {
        cout << "I" << it_index->second->level << "_AudioCount: " << it_index->second->audioCount <<"\tTermCount: "\
            <<it_index->second->termCount<<"\tMergeCount: "<<it_index->second->mergeCount<< "\n";
        writefile<< "I" << it_index->second->level << "_AudioCount: " << it_index->second->audioCount <<"\tTermCount: "\
            <<it_index->second->termCount<<"\tMergeCount: "<<it_index->second->mergeCount<< "\n";
    }
    writefile.close();

    ofstream writefile2("MergeTimeAnalysis.txt",ofstream::app);
    writefile2<<"Sum: "<<audioSum<<"\tIndexTermUnit: "<<indexTermSumUnit <<endl;
    for (multimap<int,double>::iterator it=timeOfIndexMerge.begin();it!=timeOfIndexMerge.end();it++)
    {
        writefile2<<it->first<<"\t"<<it->second<<endl;
    }
    writefile2.close();
    ofstream writefile3("BuildAnalysis.txt",ofstream::app);
    cout<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<duplicateTime<<"\tMergeTime: "<<\
    mergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<mergeTimes<<endl;
    writefile3<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<duplicateTime<<"\tMergeTime: "<<\
    mergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<mergeTimes<<endl;
    writefile3.close();

    cout<<"Sum: "<<audioSum<<"\tTotalTermSum: "<<totalTermSum<<"\tIdfCodeNum: "<<IdfCodeNum<<"\tIndexTermUnit: "<<indexTermSumUnit<<endl;
//    cout<<"NodeSum: "<<nodeSum<<endl;
}

void *searchThread(void *family)
{
    FamilyQuery *fam=(FamilyQuery *)family;
    IndexManager *myself=fam->me;
//    vector<string> &query=*(fam->que);
    vector<int> &query_code=*(fam->que_code);
    vector<double>&query_idf=*(fam->que_idf);
    vector<pair<int,double> > &resultVec=*(fam->resVec);
    btree::btree_map<int,string> &nameMap=*(fam->na);
    btree::btree_map<int,score_ratio> &scoreVec=*(fam->sco_ve);


    btree::btree_map<int, double> Result;
    double MinScore = 0;
    int Sum = 0;

    //判断Index0是否正在被清空或者修改Info
    myself->clearInvertedIndex.Lock();//在清理整个InvertedIndex，查询时要求包括镜像在内除所有InvertedIndex都不允许变化，防止出现查询时删除这种现象
    myself->Indexes[0]->I0MutexInfo.Lock();
    myself->Indexes[0]->search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
    myself->Indexes[0]->I0MutexInfo.Unlock();

    btree::btree_map<int, InvertedIndex*>::iterator itIndex;
    for (itIndex = myself->Indexes.begin(); itIndex != myself->Indexes.end(); itIndex++)
    {
        if(itIndex->first==0) continue;
        (*itIndex->second).search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
    }

    vector<ForMirror<InvertedIndex>*>::iterator itTmpMirror;
    for (itTmpMirror = myself->mirrorList.begin(); itTmpMirror != myself->mirrorList.end();)
    {
        btree::btree_map<int,InvertedIndex*>::iterator it_mirror_index;
        for (it_mirror_index=((*itTmpMirror)->mirrorIndexMap)->begin();it_mirror_index!=((*itTmpMirror)->mirrorIndexMap)->end();it_mirror_index++)
        {
            (it_mirror_index->second)->search(Result, MinScore, answerNum, Sum, query_code,query_idf, nameMap,scoreVec);
        }

        itTmpMirror++;//写在这里的原因是防止忘记迭代器删除这个坑
    }
    myself->clearInvertedIndex.Unlock();


    vector<pair<int, double> > tmp(Result.begin(), Result.end());
    resultVec=tmp;//这里八成或许可以优化，但是估计影响不大
    sort(resultVec.begin(), resultVec.end(), CompDedcendVal());
}

void *addAudioAllThread(void *Family)//如果要实现多线程，就必须管控所有add与merger
{
    FamilyAll *ones=(FamilyAll*) Family;
    AudioInfo &tmpInfo=(*ones->audio);
    IndexManager *myself=ones->me;
    btree::btree_map<int,double> &tagsNum=*(ones->tags);
    vector<ForMirror<InvertedIndex>* > &mirrorList=myself->mirrorList;//用于放置镜像Index的容器
    int tCount=(ones->termCount);
    myself->AudioSum++;

    double begin,end;
    begin=getTime();
    if (tmpInfo.final>=0)
    {
        myself->liveIdMutex[tmpInfo.id].Lock();
        myself->mutexLive.Lock();//有意义，勿删，内部会有一个对此的判断（需要一点时间），如果没人使用，说明可以query，而且不需要阻塞
        myself->mutexLive.Unlock();
        myself->Indexes[0]->addAudioLive(tmpInfo, tagsNum, myself->livePointer, myself->mutexLive);
        myself->liveIdMutex[tmpInfo.id].Unlock();
    }
    else if(tmpInfo.final==-1)
    {
        myself->Indexes[0]->addAudio(tmpInfo,tagsNum);
    }
    end=getTime();
    AddAduioTime+=end-begin;
    myself->I0Num++;
    myself->I0TermNum+=tCount;
    myself->totalTermSum+=tCount;
    myself->Indexes[0]->termCount+=tCount;


    if (myself->I0Num >= indexAudioSumUnit||myself->I0TermNum>=indexTermSumUnit)
    {
        myself->clearInvertedIndex.Lock();

        //复制I0的过程开始了
        begin=getTime();
        myself->Indexes[0]->I0_sort();
        end=getTime();
        I0SortTime+=end-begin;

        begin=getTime();
        InvertedIndex *indexTmp;

        (myself->liveIdMutex)[tmpInfo.id].Lock();
        myself->Indexes[0]->level++;
//        cout << "start to new I0" << endl;
//        sleep(10);
        indexTmp=new InvertedIndex(*myself->Indexes[0]);//这一份将会成为镜像备份，真正的Index0将会前去归并
//        cout << "finish creating I0: " << indexTmp << endl;
//        sleep(10);
        (myself->liveIdMutex)[tmpInfo.id].Unlock();

        btree::btree_map<int,InvertedIndex*> *mirrorIndex=new btree::btree_map<int,InvertedIndex*>;//相当于备份参与归并的Index到另一个map中
        ForMirror<InvertedIndex>  *forMirror=new ForMirror<InvertedIndex> (mirrorIndex);
        mirrorList.push_back(forMirror);

        (*mirrorIndex)[0]=indexTmp;//镜像置入，新Index0创建，可以add，旧Index0可以开始merge
        indexTmp=myself->Indexes[0];
        myself->Indexes[0]=new InvertedIndex;
        myself->I0Num = 0;
        myself->I0TermNum=0;

        myself->clearInvertedIndex.Unlock();


        int l=1;
        end=getTime();
        duplicateTime+=end-begin;

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


                if(myself->Indexes[l]->mergeCount+indexTmp->mergeCount>ratio-1)
                {
                    begin=getTime();
                    myself->clearInvertedIndex.Lock();
                    indexTmp->level++;
                    indexTmp->mergeCount=1;
                    (*indexTmp).mergerIndex(*(myself->Indexes[l]));
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
                    indexTmp->mergeCount+=myself->Indexes[l]->mergeCount;
                    (*indexTmp).mergerIndex(*(myself->Indexes[l]));
                    delete myself->Indexes[l];
                    end=getTime();

                    pair<int,double> p(l,end-begin);
                    timeOfIndexMerge.insert(p);
                    mergeTime+=end-begin;
                    mergeTimes++;
                    (myself->Indexes)[l]=indexTmp;

                    btree::btree_map<int,InvertedIndex*>::iterator itMirrorIndex;
                    for (itMirrorIndex=(*mirrorIndex).begin(); itMirrorIndex != (*mirrorIndex).end(); itMirrorIndex++)
                    {
                        delete itMirrorIndex->second;
                    }
                    delete mirrorIndex;
                    forMirror->mirrorIndexMap=NULL;

                    myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),forMirror),myself->mirrorList.end());
                    delete forMirror;
                    myself->clearInvertedIndex.Unlock();
                    break;
                }
            }
            else
            {
                myself->clearInvertedIndex.Lock();
                myself->Indexes[l] = indexTmp;
                indexTmp->mergeCount=1;
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
                forMirror->mirrorIndexMap=NULL;
                myself->mirrorList.erase(remove(myself->mirrorList.begin(),myself->mirrorList.end(),forMirror),myself->mirrorList.end());
                delete forMirror;
                myself->clearInvertedIndex.Unlock();
                break;
            }
        }
    }
}

void IndexManager::initialIdf()
{
    double begin,end;
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey="";
    ifstream in("../resouce/idf.txt");

    if (!in)
        exit(7);
    else {
        begin=getTime();
        cout << "Begin idf." << endl;
    }

    idfNum=0;

    string termTmp;
    string idfTmp;
    while (!in.eof())
    {
        getline(in, termTmp);
        getline(in, idfTmp);
        if (QuestionMark.compare(termTmp) && DoubleQuestionMark.compare(termTmp) && SpaceKey.compare(termTmp)&&BlankKey.compare(termTmp))
        {
            (IdfTable)[termTmp] = IdfCode(atof(idfTmp.c_str()),IdfCodeNum);
            idfNum++;
        }
        IdfCodeNum++;
    }
    end=getTime();
    cout<<"Idf is okay."<<endl;
    cout<<"Time is "<<end-begin<<"s"<<endl;
    cout<<"idfNum is "<<idfNum<<endl;
}

void IndexManager::buildIndex(int audio_sum)
{
    int who =0;//0代表本进程，-1代表子进程（用不太出来），一定要bash跑，不然这玩意测的是虚拟机的内存
    struct rusage usage;

    int tagsSum,termSum;
    double begin, end;
    begin = getTime();
    ifstream info_in("../resouce/info_live_test10000.txt");
    if (!info_in) exit(7);
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey="";
    string likeCountTmp, commentCountTmp, playCountTmp, tagsSumTmp, scoreTmp, timeTmp, titleTmp,\
 		termTmp, idTmp, numTmp, termSumTmp,finalFlagTmp,posTmp;
    for (int i = 0; i < audio_sum; i++)
    {
        if(!getline(info_in, idTmp)) break;
        getline(info_in, titleTmp);
        getline(info_in, likeCountTmp);
        getline(info_in, commentCountTmp);
        getline(info_in, playCountTmp);
        getline(info_in, scoreTmp);
        getline(info_in, tagsSumTmp);
        getline(info_in, timeTmp);
        getline(info_in, termSumTmp);
        getline(info_in,finalFlagTmp);
//        getline(info_in, posTmp);
        tagsSum = atoi(tagsSumTmp.c_str());
        termSum=0;
        btree::btree_map<int, double> TagsNum_tmp;
        btree::btree_map<string,IdfCode>::iterator it_idf;
        for (int j = 0; j < atoi(termSumTmp.c_str()); j++)
        {
            getline(info_in, termTmp);
            getline(info_in, numTmp);
            if (QuestionMark.compare(termTmp) && DoubleQuestionMark.compare(termTmp) && SpaceKey.compare(termTmp)&&BlankKey.compare(termTmp))
            {
                it_idf=IdfTable.find(termTmp);
                if(it_idf!=IdfTable.end())
                {
                    (TagsNum_tmp)[it_idf->second.code] = atof_1e(numTmp.c_str());
                    termSum++;
                }else{
                    IdfTable[termTmp]=IdfCode(1,IdfCodeNum);
                    (TagsNum_tmp)[IdfCodeNum] = atof_1e(numTmp.c_str());
                    IdfCodeNum++;
                    termSum++;
                }
            }
            else
            {
                tagsSum -= 2;//示例性的减2，没有实际意义
            }
        }
        char tmp[20];
        strcpy(tmp, idTmp.c_str());
        AudioInfo tmpInfo(char2int(idTmp.c_str()), titleTmp, atof(scoreTmp.c_str()), \
		  tagsSum, atof(timeTmp.c_str()),atoi(finalFlagTmp.c_str()));
        pthread_t id;
        FamilyAll fam(this,&tmpInfo,&TagsNum_tmp,termSum);
        pthread_create(&id, NULL, addAudioAllThread, (void *) &fam);
        pthread_join(id,NULL);
    }
    end = getTime();

    output();
    cout<<endl;
    getrusage(who,&usage);
    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<audioSum<<"\tTotalTermSum: "<<totalTermSum<<"\tIndexTermUnit: "<<indexTermSumUnit<<"\tSumTime: "<<end-begin\
    <<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    ofstream writefile2("memory_of_index.txt",ofstream::app);
    writefile2<<"Sum: "<<audioSum<<"\tTotalTermSum: "<<totalTermSum<<"\tIndexTermUnit: "<<indexTermSumUnit<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    writefile2.close();
    writefile.close();
    info_in.close();
    cout<<"\tSumTime: "<<end-begin<<setprecision(8)<<"\tMaxResidentMemory(MB): "<<usage.ru_maxrss/1024<<endl;
    cout<<endl;
}

void IndexManager::handleQuery(string queryStr, vector<string> &Result)
{
    double begin, end;
    string s;
    string stopwords = " ";
    vector<string> query;
    vector<int> query_code;
    vector<double> query_idf;
    vector<pair<int, double> > resultVec;
    btree::btree_map<int, string> name;
    btree::btree_map<int,score_ratio> scoVec;

    SplitString(queryStr, query, stopwords);


    btree::btree_map<string,IdfCode>::iterator it_idf;
    for (int i=0;i<query.size();i++)
    {
        it_idf=IdfTable.find(query[i]);
        if(it_idf!=IdfTable.end())
        {
            query_code.push_back(it_idf->second.code);
            query_idf.push_back(log(audioSum/(it_idf->second.idf+1.0)));
        }
    }

    FamilyQuery fam(this,&query_code,&query_idf,&name,&resultVec,&scoVec);
    pthread_t pid;
    pthread_create(&pid,NULL,searchThread,(void*)&fam);
    pthread_join(pid,NULL);

    const char* resultTemplate = " { \"id\" : 0, \"title\" : \" \" , \"link\" : \" \", \" score\": 0.0 } ";
    end = getTime();
    if (resultVec.size() == 0)
    {
        return;
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
//            strBack += "id： " + Itos(resultVec[i].first) + "\n"+"title： " + name[resultVec[i].first] +
//                        "\nlink：http://www.ximalaya.com/sound/" + Itos(resultVec[i].first) + "\nscore:\t" + Dtos(resultVec[i].second) +"\tfre:\t"+\
//                        Dtos(scoVec[resultVec[i].first].fre)+"\tsig:\t"+Dtos(scoVec[resultVec[i].first].sig)+"\tsim:\t"+Dtos(scoVec[resultVec[i].first].sim)+ "\n\n";
        }
    }
    string strBack = "Time: "+ Dtos(end - begin) + "s" +"\n";
    Result.push_back(strBack);
    query.clear();
    resultVec.clear();
    name.clear();
    scoVec.clear();
    return;
}

void IndexManager::updateScore(int id,int score)
{
    btree::btree_map<int,InvertedIndex*>::iterator it_Index;
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

void IndexManager::createAudio(AudioInfo &audio,string &content)
{
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey = "";
    string termTmp;


    map<string, int> termMap;
    map<string, int>::iterator itTerm;
    btree::btree_map<string,IdfCode>::iterator it_idf;
    btree::btree_map<int, double> TagsNum_tmp;
    vector<cppjieba::Word> jiebawords;
    vector<cppjieba::Word>::iterator it_tmp;

    int tagsSum=0;//用于计算tf
    int termSum=0;//用于判断是否term达到临界值，可以删去，直接传进去

    //////////////////计算各类term的加权数量////////////////////////
    jieba->Cut(audio.title, jiebawords, true);//true是否启动hmm算法
    for (it_tmp = jiebawords.begin(); it_tmp != jiebawords.end(); it_tmp++) {
        itTerm = termMap.find(it_tmp->word);
        if (itTerm != termMap.end()) {
            itTerm->second += TitleWeigh;
        } else {
            termMap[it_tmp->word] = TitleWeigh;
        }
        tagsSum+=TitleWeigh;
    }
    jiebawords.clear();

    jieba->Cut(audio.tags, jiebawords, true);
    for (it_tmp = jiebawords.begin(); it_tmp != jiebawords.end(); it_tmp++)
    {
        itTerm=termMap.find(it_tmp->word);
        if(itTerm!=termMap.end())
        {
            itTerm->second+=TagWeigh;
        } else{
            termMap[it_tmp->word] = TagWeigh;
        }
        tagsSum+=TagWeigh;
    }
    jiebawords.clear();

    jieba->CutForSearch(content,jiebawords,true);
    for (it_tmp = jiebawords.begin(); it_tmp != jiebawords.end(); it_tmp++)
    {
        itTerm=termMap.find(it_tmp->word);
        if(itTerm!=termMap.end())
        {
            itTerm->second+=ContentWeigh;
        } else{
            termMap[it_tmp->word] = ContentWeigh;
        }
        tagsSum+=ContentWeigh;
    }

    /////////////////////////更新IDF表/////////////////////////////
    for (itTerm=termMap.begin();itTerm!=termMap.end();itTerm++)
    {
        termTmp=itTerm->first;

        if (QuestionMark.compare(termTmp) && DoubleQuestionMark.compare(termTmp) && SpaceKey.compare(termTmp)&&BlankKey.compare(termTmp))
        {
            it_idf=IdfTable.find(termTmp);
            if(it_idf!=IdfTable.end())
            {
                it_idf->second.idf+=1;
                (TagsNum_tmp)[it_idf->second.code] = itTerm->second/(tagsSum+1.0);
            }else{
                IdfTable[termTmp]=IdfCode(1.0,IdfCodeNum);
                (TagsNum_tmp)[IdfCodeNum] = itTerm->second/(tagsSum+1.0);
                IdfCodeNum++;
            }
            termSum++;
        }
    }
    pthread_t pid;
    FamilyAll fam(this,&audio,&TagsNum_tmp,termSum);
    pthread_create(&pid, NULL, addAudioAllThread, (void *) &fam);
    pthread_join(pid,NULL);
}

void IndexManager::addAudio(AudioInfo &audio, string &content) {
    string DoubleQuestionMark = "??";
    string QuestionMark = "?";
    string SpaceKey = " ";
    string BlankKey = "";
    string termTmp;

    map<string, int> termMap;
    map<string, int>::iterator itTerm;
    btree::btree_map<string,IdfCode>::iterator it_idf;
    btree::btree_map<int, double> TagsNum_tmp;
    vector<cppjieba::Word> jiebawords;
    vector<cppjieba::Word>::iterator it_tmp;

    int tagsSum=0;//用于计算tf
    int termSum=0;//用于判断是否term达到临界值，可以删去，直接传进去

    //////////////////计算各类term的加权数量////////////////////////
    jieba->CutForSearch(content,jiebawords,true);
    for (it_tmp = jiebawords.begin(); it_tmp != jiebawords.end(); it_tmp++)
    {
        itTerm=termMap.find(it_tmp->word);
        if(itTerm!=termMap.end())
        {
            itTerm->second+=ContentWeigh;
        } else{
            termMap[it_tmp->word] = ContentWeigh;
        }
        tagsSum+=ContentWeigh;
    }

    /////////////////////////更新IDF表/////////////////////////////
    for (itTerm=termMap.begin();itTerm!=termMap.end();itTerm++)
    {
        termTmp=itTerm->first;

        if (QuestionMark.compare(termTmp) && DoubleQuestionMark.compare(termTmp) && SpaceKey.compare(termTmp)&&BlankKey.compare(termTmp))
        {
            it_idf=IdfTable.find(termTmp);
            if(it_idf!=IdfTable.end())
            {
                it_idf->second.idf+=1;
                (TagsNum_tmp)[it_idf->second.code] = itTerm->second/(tagsSum+1.0);
            }else{
                IdfTable[termTmp]=IdfCode(1.0,IdfCodeNum);
                (TagsNum_tmp)[IdfCodeNum] = itTerm->second/(tagsSum+1.0);
                IdfCodeNum++;
            }
            termSum++;
        }
    }

    AudioInfo tmpInfo(audio);
    pthread_t pid;
    FamilyAll fam(this,&tmpInfo,&TagsNum_tmp,termSum);
    pthread_create(&pid, NULL, addAudioAllThread, (void *) &fam);
    pthread_join(pid,NULL);
}