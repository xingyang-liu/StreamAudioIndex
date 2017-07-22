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



