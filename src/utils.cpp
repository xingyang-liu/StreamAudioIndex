//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "utils.h"

int IndexUnit = 20;
int AudioSum = 999;
int AnswerNum = 5;
int IdfNum=0;
map<string, double> IdfTable;

string Itos(int num)
{
    char c[20];
    sprintf(c, "%d", num);
    string tmp(c);
    return tmp;
}

int char2int(const char *text)
{
    int count = 0;
    for (int i = 0; text[i] != '\r' && text[i] != '\0' && text[i] != '\n'; ++i) {
        char t = text[i];
        if (text[i] > '9' || text[i] < '0') continue;
        count = count * 10 + text[i] - '0';
    }
    return count;
}

string Dtos(double i)
{
    string str;
    stringstream ss;
    ss << i;
    ss >> str;
    return str;
}

//分词算法
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

double getTime()
{
    struct timeval ts;
    gettimeofday(&ts, NULL);

    return ts.tv_sec + ts.tv_usec / 1000000.0;
}

//};


double computeScore(const double &time, const double &score, map<string, int> &tagsNum, const int &tagsSum,
                    const vector<string> &query)
{
    double fre = pow(2, time - getTime());
    double sig = log(score / 10000 + 1);
    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        map<string, int>::iterator it = tagsNum.find(query[i]);
        if (it != tagsNum.end())
        {
            try
            {
                if (tagsSum != 0)
                {
                    sim += tagsNum[query[i]] / tagsSum*IdfTable[query[i]];
                }
                else
                {
                    sim += tagsNum[query[i]] * IdfTable[query[i]];
                }
            }
            catch (...)
            {
                map<string, double>::iterator it = IdfTable.find(query[i]);
                if (it != IdfTable.end())
                {
                    continue;
                }
                else
                {
                    cout << "Something wrong with computeScore()";
                }

            }
        }
    }
    double ScoreAll = (fre*0.1 + sim * 20 * 0.6 + sig*0.2)*10;
    return ScoreAll;
}

