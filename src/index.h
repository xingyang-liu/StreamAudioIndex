//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_INDEX_H
#define SAMPLE_CONNECTOR_INDEX_H


#include "utils.h"
#include "AudioInfo.h"
#include "Sig.h"
#include "TermFreq.h"
#include "Fre.h"
#include "I0.h"
#include "Ii.h"
#include "IndexAll.h"
//#pragma comment(lib,"ws2_32.lib")

using namespace std;

void InitialIdf();

void ini(IndexAll &Index);

string handleQuery(IndexAll &Index, string query_str);

void preprocess(IndexAll &Index,int sum);

double test_for_add(IndexAll &Index,int ini_num,int audio_sum);

void test_for_index(int times,int sum);

void* test_for_queryThread(void *Fam);

void* test_for_addThread(void *Fam);

class FamilyTestQuery
{
public:
    IndexAll *index;
    int times;
    int sleeptime;

    FamilyTestQuery(IndexAll *i,int t,int s)
    {
        index=i;
        times=t;
        sleeptime=s;
    }
};

class FamilyAdd
{
public:
    IndexAll *index;
    int sum;
    int sleeptime;

    FamilyAdd(IndexAll *i,int s,int sleep)
    {
        index=i;
        sum=s;
        sleeptime=sleep;
    }
};

void test_for_QandA(IndexAll &Index,int query_times,int query_sleeptime,int add_sum,int add_sleeptime);


class FamilyUpdate
{
public:
    IndexAll *me;
    int sum;
    int sleeptime;

    FamilyUpdate(IndexAll *i,int s,int sleep):me(i),sum(s),sleeptime(sleep) {}
};


void* test_for_updateThread(void*Fam);

#endif //SAMPLE_CONNECTOR_INDEX_H
