//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_INDEX_H
#define HASH_0E_INDEX_H



#include "IndexManager.h"
//#pragma comment(lib,"ws2_32.lib")

using namespace std;

double index_create(int sum);

void test_for_index(int times,int sum);

void* test_for_queryThread(void *Fam);

void* test_for_addThread(void *Fam);

void test_for_QandA(IndexManager &Index,int query_times,int query_sleeptime,int add_sum,int add_sleeptime);

void* test_for_updateThread(void*Fam);

class FamilyTestQueryAndUpdate
{
public:
    IndexManager *index;
    int times;
    int sleeptime;

    FamilyTestQueryAndUpdate(IndexManager *i,int t,int s)
    {
        index=i;
        times=t;
        sleeptime=s;
    }
};

class FamilyAdd
{
public:
    IndexManager *index;
    int sum;
    int sleeptime;

    FamilyAdd(IndexManager *i,int s,int sleep)
    {
        index=i;
        sum=s;
        sleeptime=sleep;
    }
};

#endif //HASH_0E_INDEX_H
