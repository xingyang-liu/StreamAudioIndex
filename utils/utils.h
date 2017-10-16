
#ifndef HASH_0E_UTILS_H
#define HASH_0E_UTILS_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <sys/time.h>
#include <sstream>
#include <limits>
#include <pthread.h>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>
#include <set>
#include <queue>
#include <atomic>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include "../cpp-btree-1.0.1/btree_map.h"
#include "../BasicStructure/IdfCode.h"
#include "../cppjieba/include/cppjieba/Jieba.hpp"
//#include "rapidjson/document.h"
//#include "rapidjson/writer.h"
//#include "rapidjson/stringbuffer.h"


using namespace std;

extern int indexAudioSumUnit;
extern int audioSum;
extern int answerNum;
extern int idfNum;
extern int mergeTimes;
extern int indexTermSumUnit;
extern double AddAduioTime;
extern double mergeTime;
extern double I0SortTime;
extern double duplicateTime;
extern double MergeSortTime;
extern double weight_fre;
extern double weight_sig;
extern double weight_sim;
extern vector<double> score_vec;
extern int live_an;
extern int ratio;
extern int nodeSum;
extern int proSum;

extern multimap<int,double> timeOfIndexMerge;
//extern btree::btree_map<string,double> IdfTable;
//extern btree::btree_map<string,IdfCode> IdfTable;


extern const char* const DICT_PATH;
extern const char* const HMM_PATH;
extern const char* const USER_DICT_PATH;
extern const char* const IDF_PATH;
extern const char* const STOP_WORD_PATH;

string Itos(int num);

int char2int(const char *t);

string Dtos(double i);

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

double getTime();


struct CompDedcendVal {
    bool operator()(const pair<int, double> &a, const pair<int, double> &b)
    {
        return a.second > b.second;
    }
};


class CMutex {
public:
    CMutex() {
        mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    ~CMutex() {}

    void Lock() {
        pthread_mutex_lock(&mutex);
    }

    void Unlock() {
        pthread_mutex_unlock(&mutex);
    }

private:
    pthread_mutex_t mutex;
};

double atof_1e(const char s[]);   //将字符串s转换成double型的浮点数(含科学计数法）

class score_ratio
{
public:
    double fre,sig,sim;
    score_ratio(){fre=0;sig=0;sim=0;}
    score_ratio(double f,double s,double si):fre(f),sig(s),sim(si){}
    score_ratio(const score_ratio&other)
    {
        fre=other.fre;
        sig=other.sig;
        sim=other.sim;
    }

    score_ratio &operator=(const score_ratio&other)
    {
        if(&other==this)
        {
            return *this;
        } else{
            fre=other.fre;
            sig=other.sig;
            sim=other.sim;
            return *this;
        }
    }
};

#endif //HASH_0E_UTILS_H
