//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_INDEX_H
#define HASH_0E_INDEX_H



#include "../TextIndex/IndexManager.h"
#include "../PhonomeIndex/PhoIndexManager.h"

using namespace std;

double index_create(int sum);

void test_for_index(int times,int sum);//由于不知名的内存泄露（valgrind检测没有问题），貌似连续新建会有内存积压，所以还是用python调用执行文件比较方便

void *test_for_queryThread(void *Fam);

void *test_for_queryPhoThread(void *Fam);

void *test_for_querytxtThread(void *Fam);

void* test_for_addThread(void *Fam);

void test_for_QandA(IndexManager &Index,int query_times,int query_sleeptime,int add_sum,int add_sleeptime);

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

class FamilyTestQueryAndUpdatePho
{
public:
    PhoIndexManager *index;
    int times;
    int sleeptime;

    FamilyTestQueryAndUpdatePho(PhoIndexManager* i,int t,int s)
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


void* test_for_updateThread(void*Fam);

void* test_for_updatetxtThread(void*Fam);


#endif //HASH_0E_INDEX_H
