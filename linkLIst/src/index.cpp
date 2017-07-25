
#include "index.h"

using namespace std;

double index_create(int sum)
{
    double begin,end;
    begin=getTime();
    IndexManager index(sum);
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
	IndexManager &Index=(*fam.index);
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
		getline(info_in, TagsSum_tmp);//其实本来没什么用处，但是我后面判断上下界的时候用这个作为了识别标志，所以留着也没坏处
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
			}
			else
			{
				TagsSum -= 2;//本来这个地方我的想法是去掉无法识别的字符数，但是由于现在放进来的是频率，不好处理了，所以就象征性的减一点
			}
		}
		char tmp[20];
		strcpy(tmp, id_tmp.c_str());
		AudioInfo tmp_info(char2int(id_tmp.c_str()), title_tmp,atof(score_tmp.c_str()), \
		 TagsSum, atof(time_tmp.c_str()),atoi(FinalFlag_tmp.c_str()));
		pthread_t id;
		FamilyAll fam(&Index,&tmp_info,&TagsNum_tmp);
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
	FamilyTestQueryAndUpdate fam=*(FamilyTestQueryAndUpdate *)Fam;
	IndexManager &Index=*(fam.index);
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
		Index.handleQuery(query_str_list[i]);
		end=getTime();
		time_list.push_back(end-begin);
        if((i+1)%50==0)
        {
            cout<<"QueryCount: "<<i+1<<endl;
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

void test_for_QandA(IndexManager &Index,int query_times,int query_sleeptime,int add_sum,int add_sleeptime)
{
	FamilyTestQueryAndUpdate famq(&Index,query_times,query_sleeptime);
	pthread_t pidq,pida;
	pthread_create(&pidq,NULL,test_for_queryThread,(void *)&famq);

	FamilyAdd fama(&Index,add_sum,add_sleeptime);
	pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
	pthread_join(pidq,NULL);
	pthread_join(pida,NULL);
}

void* test_for_updateThread(void*Fam)
{
	FamilyTestQueryAndUpdate fam=*(FamilyTestQueryAndUpdate *)Fam;
	IndexManager &Index=*(fam.index);
	int times=fam.times;
	int sleeptime=fam.sleeptime;



	double begin,end;
	vector<double> time_list;
	int count=0;
	int tmp_num;
	//生成测试数组
	map<int,double> test_update;
	map<int,InvertedIndex*>::iterator it_index;
	map<int,AudioInfo>::iterator it_audio;
	while(count<times)
	{
		for(it_index=Index.Indexes.begin();it_index!=Index.Indexes.end();it_index++)
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


	cout<<"Begin test of update"<<endl;

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