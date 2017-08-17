//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "utils.h"

int IndexUnit = 20;
int AudioSum = 999;
int AnswerNum = 10;
int IdfNum=0;
map<string, double> IdfTableText;
map<SimilarPhoneme, double> IdfTablePho;

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

double atof_1e(const char s[])   //将字符串s转换成double型的浮点数(含科学计数法）
{
    int i;
    int sign;
    int flag;
    int expn;
    double val;
    double power;

    sign = 1;
    flag = 0;
    power = 1.0;
    expn = 0;
    for(i = 0; isspace(s[i]); ++i)
        ;
    if(s[i] == '-')
        sign = -1;
    if(s[i] == '+' || s[i] == '-')
        ++i;
    for(val = 0.0; isdigit(s[i]); ++i)
        val = val * 10.0 + (s[i] - '0');
    if(s[i] == '.')
        ++i;
    for(; isdigit(s[i]); ++i)
    {
        val = val * 10.0 + (s[i] - '0');
        //power = power * 10.0;
        ++flag;
    }
    if(s[i] == 'e' || s[i] == 'E')  //如果写成s[i++] == 'e' || s[i++] == 'E'，if(s[i] == '-')
        //则当输入的字符串带有E时，不能正确得到结果，这是因为在一
        //个语句中使用两次自增操作，引起歧义
        if(s[++i] == '-')
        {
            ++i;
            for(; isdigit(s[i]); ++i)
                expn = expn * 10 + (s[i] - '0');
            expn = expn + flag;
            power = pow(10, expn);
            return sign * val / power;
        }
        else
        {
            for(; isdigit(s[i]); ++i)
                expn = expn * 10 + (s[i] - '0');
            expn = expn - flag;
            power = pow(10, expn);
            return sign * val * power;
        }

    power = pow(10, flag);
    return sign * val / power;

}

//};



