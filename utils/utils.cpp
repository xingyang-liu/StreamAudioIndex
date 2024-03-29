
#include "utils.h"

int indexAudioSumUnit = 20;
int audioSum = 999;
int answerNum = 5;
int idfNum=0;
int indexTermSumUnit=0;
int mergeTimes=0;
int live_an=0;
int ratio=0;
int nodeSum=0;
int proSum=0;
//int IdfCodeNum=0;

double AddAduioTime=0;//真正用于add的时间
double mergeTime=0;//真正用于merge的时间（仅含归并排序和预先处理和善后部分，不含复制）
double I0SortTime=0;//I0merge前的排序
double duplicateTime=0;//merge中用于镜像的复制的时间
double MergeSortTime=0;//merge中用于归并排序的时间
double weight_fre=0.2;
double weight_sig=0.2;
double weight_sim=0.6;
vector<double> score_vec;
multimap<int,double> timeOfIndexMerge;
//btree::btree_map<string,double> IdfTable;
//btree::btree_map<string,IdfCode> IdfTable;

const char* const DICT_PATH = "/home/billy/Download/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/billy/Download/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/billy/Download/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/billy/Download/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/billy/Download/cppjieba/dict/stop_words.utf8";

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
void SplitString(const string& s, std::vector<std::string>& v,const string& c)//支持多字符的分割字符
{
    char * strc = new char[strlen(s.c_str())+1];
    strcpy(strc, s.c_str());
    char* tmpStr = strtok(strc, c.c_str());
    while (tmpStr != NULL)
    {
        v.push_back(std::string(tmpStr));
        tmpStr = strtok(NULL, c.c_str());
    }
    delete[] strc;
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
    for(i = 0; isspace(s[i]); ++i);

    if(s[i] == '-')sign = -1;

    if(s[i] == '+' || s[i] == '-')++i;

    for(val = 0.0; isdigit(s[i]); ++i)val = val * 10.0 + (s[i] - '0');

    if(s[i] == '.')++i;

    for(; isdigit(s[i]); ++i)
    {
        val = val * 10.0 + (s[i] - '0');
        ++flag;
    }

    if(s[i] == 'e' || s[i] == 'E')
        //如果写成s[i++] == 'e' || s[i++] == 'E'，if(s[i] == '-')
        //则当输入的字符串带有E时，不能正确得到结果，这是因为在一
        //个语句中使用两次自增操作，引起歧义
        if(s[++i] == '-')
        {
            ++i;
            for(; isdigit(s[i]); ++i){
                expn = expn * 10 + (s[i] - '0');
            }
            expn = expn + flag;
            power = pow(10, expn);
            return sign * val / power;
        }
        else
        {
            for(; isdigit(s[i]); ++i) {
                expn = expn * 10 + (s[i] - '0');
            }
            expn = expn - flag;
            power = pow(10, expn);
            return sign * val * power;
        }

    power = pow(10, flag);
    return sign * val / power;
}





