
#include "index.h"

using namespace std;

void InitialIdf()
{
	string DoubleQuestionMark = "??";
	string QuestionMark = "?";
	string SpaceKey = " ";
	ifstream in("idf.txt");
	if (!in)
        exit(7);
    else {
//        cout << "Begin idf." << endl;
    }
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
	}
}



void ini(IndexAll &Index)
{
	double begin, end;
	begin = getTime();
	ifstream info_in("AudioInfo.txt");
	if (!info_in) exit(7);
	string DoubleQuestionMark = "??";
	string QuestionMark = "?";
	string SpaceKey = " ";
	string LikeCount_tmp, CommentCount_tmp, PlayCount_tmp, TagsSum_tmp, score_tmp, time_tmp, title_tmp, term_tmp, id_tmp, num_tmp, TermSum_tmp;
	for (int i = 0; i < AudioSum; i++)
	{
		getline(info_in, id_tmp);
		getline(info_in, title_tmp);
		getline(info_in, LikeCount_tmp);
		getline(info_in, CommentCount_tmp);
		getline(info_in, PlayCount_tmp);
		getline(info_in, score_tmp);
		getline(info_in, TagsSum_tmp);
		getline(info_in, time_tmp);
		getline(info_in, TermSum_tmp);
		//cout << id_tmp << title_tmp << LikeCount_tmp << CommentCount_tmp << PlayCount_tmp << score_tmp << TagsSum_tmp << time_tmp << endl;
		int TagsSum = atoi(TagsSum_tmp.c_str());
		map<string, int> TagsNum_tmp;
		for (int j = 0; j < atoi(TermSum_tmp.c_str()); j++)
		{
			getline(info_in, term_tmp);
			getline(info_in, num_tmp);
			if (QuestionMark.compare(term_tmp) && DoubleQuestionMark.compare(term_tmp) && SpaceKey.compare(term_tmp))
			{
				(TagsNum_tmp)[term_tmp] = atoi(num_tmp.c_str());
				//cout << term_tmp << ' ' << num_tmp << endl;
			}
			else
			{
				TagsSum -= atoi(num_tmp.c_str());
				//cout << "delete " << term_tmp << endl;
			}
		}
        char tmp[20];
        strcpy(tmp, id_tmp.c_str());
		Index.addAudio(char2int(id_tmp.c_str()), title_tmp, char2int(LikeCount_tmp.c_str()), char2int(CommentCount_tmp.c_str()), \
			char2int(PlayCount_tmp.c_str()), atof(score_tmp.c_str()), TagsNum_tmp, TagsSum, atof(time_tmp.c_str()));

	}
	end = getTime();
//	cout << end - begin << "s" << endl;
	Index.output();
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
    ofstream out_res("Result.txt", ofstream::app);

    SplitString(query_str, query, stopwords);
    for (int j = 0; j < query.size(); ++j) {
        query[j] += '\r';
    }
    Result = Index.search(query, name);
    //cout << name.size() << endl;

    out_res << query[0] << endl;

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
            str_back += "id： " + Itos(Result[i].first) + "\ntitle： " + name[Result[i].first] +
                    "link：http://www.ximalaya.com/sound/" + Itos(Result[i].first) + "\nscore: " + Dtos(Result[i].second) + "\n\n";
//            cout << Result[i].first << "-" << name[Result[i].first] << "\:" << Result[i].second << "\n";
            out_res << Result[i].first << "-" << name[Result[i].first] << ":" << Result[i].second << "\n";

        }
    }
//    str_back += "Time: "+ Dtos(end - begin) + "s" +"\n";
//		send(serConn, str_back.c_str(), strlen(str_back.c_str()) +5, 0);
//    cout << "Time: " << end - begin << "s" << endl;
//		out_res << "Time: " << end - begin << "s" << endl;
    query.clear();
    Result.clear();
    name.clear();
//		out_res.close()

//	closesocket(serConn);   //关闭
//	WSACleanup();           //释放资源的操作
//	cout << "连接断开" << endl;
    return str_back;
}


void preprocess(IndexAll &Index)
{
	double begin, end;
	begin = getTime();
	InitialIdf();
//	cout << "Initialization of idf is okay." << endl;
	end = getTime();
//	cout << end - begin << "s" << endl;

//	IndexAll Index;

	ini(Index);

//	handleQuery(Index);

	

//	system("pause");


}
