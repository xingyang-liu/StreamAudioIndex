
#include "index.h"

using namespace std;

void InitialIdf()
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

void ini(IndexAll &Index,int audio_sum)
{
	double begin, end;
	begin = getTime();
	ifstream info_in("info_live_test.txt");
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
        AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, char2int(LikeCount_tmp.c_str()),\
		 char2int(CommentCount_tmp.c_str()), char2int(PlayCount_tmp.c_str()), atof(score_tmp.c_str()), \
		 TagsNum_tmp, TagsSum, atof(time_tmp.c_str()),atoi(FinalFlag_tmp.c_str()));
		pthread_t id;
        FamilyAll fam(&Index,&tmp_info);
        pthread_create(&id,NULL,addAudioALLThread,(void*)&fam);
        pthread_join(id,NULL);


	}
	end = getTime();

	Index.output();
	cout << end - begin << "s" << endl;
	ofstream writefile("test_of_index.txt",ofstream::app);
	writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<end-begin\
    <<" Times: "<<1<<setprecision(8)<<" Average: "<<end-begin<<endl;
	writefile.close();
	info_in.close();
}

string handleQuery(IndexAll &Index, string query_str)
{

	string s;

	double begin, end;
	string stopwords = "，";
	vector<string> query;
	vector<pair<int, double> > Result;
	map<int, string> name;
//    ofstream out_res("Log.txt", ofstream::app);

    SplitString(query_str, query, stopwords);

	FamilyQuery fam(&Index,&query,&name,&Result);
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

void preprocess(IndexAll &Index,int num)
{
	double begin, end;
	InitialIdf();
	cout << "Initialization of idf is okay." << endl;
	ini(Index,num);
}


double index_create(int sum)
{
    double begin,end;
    begin=getTime();
    IndexAll index;
	ini(index,sum);
	index.freemem();
    end=getTime();
    return end-begin;
}

void test_for_index(int times,int sum=AudioSum)
{
    vector<double> time_list;
    double sumTime=0;

    for (int i=0;i<times;i++)
    {
        time_list.push_back(index_create(sum));
    }
    for (int i=0;i<times;i++)
        sumTime+=time_list[i];
    ofstream writefile("test_of_index.txt",ofstream::app);
    writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<sumTime\
    <<" Times: "<<times<<setprecision(8)<<" Average: "<<sumTime/times<<endl;

}

void* test_for_addThread(void *Fam)
{
	FamilyAdd fam=*(FamilyAdd*)Fam;
	IndexAll &Index=(*fam.index);
	int audio_sum=fam.sum;
	int sleeptime=fam.sleeptime;

	ifstream info_in("info_live_test2.txt");
	if (!info_in) exit(7);
	string DoubleQuestionMark = "??";
	string QuestionMark = "?";
	string SpaceKey = " ";
	string LikeCount_tmp, CommentCount_tmp, PlayCount_tmp, TagsSum_tmp, score_tmp, time_tmp, title_tmp,\
 		term_tmp, id_tmp, num_tmp, TermSum_tmp,FinalFlag_tmp;
	vector<double> time_list;
	double begin,end;
	for (int i = 0; i < audio_sum; i++)
	{
		usleep((rand()/(RAND_MAX+1.0)+sleeptime)*1000);
		begin=getTime();
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
		AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp, char2int(LikeCount_tmp.c_str()),\
		 char2int(CommentCount_tmp.c_str()), char2int(PlayCount_tmp.c_str()), atof(score_tmp.c_str()), \
		 TagsNum_tmp, TagsSum, atof(time_tmp.c_str()),atoi(FinalFlag_tmp.c_str()));
		pthread_t id;
		FamilyAll fam(&Index,&tmp_info);
		pthread_create(&id,NULL,addAudioALLThread,(void*)&fam);
		pthread_join(id,NULL);
		end=getTime();
		time_list.push_back(end-begin);
		if((i+1)%50==0)
		{
			cout<<"Add Audio Sum: "<<i+1<<endl;
		}

	}
	double timeSum=0;
	for (int i=0;i<time_list.size();i++)
	{
		timeSum+=time_list[i];
	}

	cout<<"SumTime: "<<timeSum<<" Sum: "<<audio_sum<<setprecision(8)<<" Average: "<<timeSum/audio_sum<<endl;
	ofstream writefile("test_for_add.txt",ofstream::app);
	writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<timeSum<<" Sum: "<<audio_sum<<setprecision(8)\
 	<<" Average: "<<timeSum/audio_sum<<endl;
	writefile.close();

}

void *test_for_queryThread(void *Fam)
{
	FamilyTestQuery fam=*(FamilyTestQuery *)Fam;
	IndexAll &Index=*(fam.index);
	int times=fam.times;
	int sleeptime=fam.sleeptime;

	vector<string> query_str_list;
	srand((unsigned)time(NULL));
	int length=0;
	string str;
	map<string,double>::iterator it=IdfTable.begin();
	cout<<"Initialize List of Query."<<endl;
	for (int i=0;i<times;i++)
	{
		length=rand()%5+1;
		str="";
		for (int j=0;j<length;j++)
		{
			it++;
			if(it==IdfTable.end())
			{
				it=IdfTable.begin();
			}
			str+=it->first;
			if (j<length-1)
			{
				str+="，";
			}
		}
		query_str_list.push_back(str);
	}

	cout<<"Begin Test of Query."<<endl;
	double begin,end;
	vector<double> time_list;
	for (int i=0;i<times;i++)
	{
		usleep((rand()/(RAND_MAX+1.0)+sleeptime)*20000);
		begin=getTime();
		handleQuery(Index, query_str_list[i]);
		end=getTime();
		time_list.push_back(end-begin);
		if((i+1)%50==0) {
			cout << "QueryCount: " << i + 1 << endl;
		}
	}
	double timeSum=0;
	for (int i=0;i<time_list.size();i++)
	{
		timeSum+=time_list[i];
	}
	cout<<"SumTime: "<<timeSum<<" Times: "<<times<<setprecision(8)<<" Average: "<<timeSum/times<<endl;
    ofstream writefile("test_for_query.txt",ofstream::app);
    writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<timeSum<<" Times: "<<times<<setprecision(8)<<" Average: "<<timeSum/times<<endl;
    writefile.close();
}

void test_for_QandA(IndexAll &Index,int query_times,int query_sleeptime,int add_sum,int add_sleeptime)
{
	FamilyTestQuery famq(&Index,query_times,query_sleeptime);
	pthread_t pidq,pida;
	pthread_create(&pidq,NULL,test_for_queryThread,(void *)&famq);

	FamilyAdd fama(&Index,add_sum,add_sleeptime);
	pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
	pthread_join(pidq,NULL);
	pthread_join(pida,NULL);


}

void* test_for_updateThread(void*Fam)
{
	FamilyUpdate fam=*(FamilyUpdate *)Fam;
	IndexAll &Index=*(fam.me);
	int times=fam.sum;
	int sleeptime=fam.sleeptime;



	double begin,end;
	vector<double> time_list;
	int count=0;
	int tmp_num;
	//生成测试数组
	map<int,double> test_update;
	map<int,Ii*>::iterator it_index;
	map<int,AudioInfo>::iterator it_audio;
	cout<<"Begin test of update"<<endl;
	while(count<times)
	{
		for(it_index=Index.otherIndex.begin();it_index!=Index.otherIndex.end();it_index++)
		{
			usleep((rand()/(RAND_MAX+1.0)+sleeptime)*1000);
			tmp_num=rand()/(RAND_MAX+1.0)*it_index->second->AudioCount;
			int random_count=0;
			if(it_index->second->InfoTable!=NULL)
			{
				for(it_audio=it_index->second->InfoTable->begin();it_audio!=it_index->second->InfoTable->end();it_audio++)
				{
					if(test_update.find(it_audio->second.id)==test_update.end())
					{
						test_update[it_audio->second.id]=it_audio->second.score*10;
						random_count++;
						count++;
					}
					if((count+1)%50==0)
					{
						cout<<"Update Audio Sum: "<<count+1<<endl;
					}
					if(random_count>=tmp_num)break;

					if(count>=times)break;
				}
				if(count>=times)break;
			}
		}
		if(count>=times)break;
	}




	map<int,double>::iterator it_update;
	for(it_update=test_update.begin();it_update!=test_update.end();it_update++)
	{
		begin=getTime();
		Index.updateScore(it_update->first,it_update->second);
		end=getTime();
		time_list.push_back(end-begin);
	}
	double timeSum=0;
	for (int i=0;i<time_list.size();i++)
	{
		timeSum+=time_list[i];
	}
	cout<<"SumTime: "<<timeSum<<" Times: "<<times<<setprecision(8)<<" Average: "<<timeSum/times<<endl;
	ofstream writefile("test_for_update.txt",ofstream::app);
	writefile<<"Sum: "<<AudioSum<<" Unit: "<<IndexUnit<<" SumTime: "<<timeSum<<" Times: "<<times<<setprecision(8)<<" Average: "<<timeSum/times<<endl;
	writefile.close();
}