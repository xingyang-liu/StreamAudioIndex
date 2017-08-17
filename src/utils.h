//
// Created by 兴阳 刘 on 2017/7/11.
//

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
#include "../PhonomeIndex/Phoneme.h"


using namespace std;

extern int IndexAudioSumUnit;
extern int AudioSum;
extern int AnswerNum;
extern int IdfNum;
extern int MergeTimes;
extern int IndexTermSumUnit;
extern double AddAduioTime;
extern double MergeTime;
extern double I0SortTime;
extern double DuplicateTime;
extern double MergeSortTime;
extern double weight_fre;
extern double weight_sig;
extern double weight_sim;
extern vector<double> score_vec;
extern int live_an;
extern int ratio;
extern multimap<int,double> time_of_index_merge;
//extern map<string, double> IdfTableText;
extern map<SimilarPhoneme, double> IdfTablePho;


extern bool cout_flag;

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


class CMutex
{
public:
    CMutex()
    {
        mutex = PTHREAD_MUTEX_INITIALIZER;
    }
    ~CMutex(){}
    void Lock()
    {
        pthread_mutex_lock(&mutex);
    }
    void Unlock()
    {
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
