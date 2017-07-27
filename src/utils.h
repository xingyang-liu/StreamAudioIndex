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


using namespace std;

extern int IndexUnit;
extern int AudioSum;
extern int AnswerNum;
extern int IdfNum;
extern map<string, double> IdfTable;

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

#endif //HASH_0E_UTILS_H
