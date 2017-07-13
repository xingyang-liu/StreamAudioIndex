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
#pragma comment(lib,"ws2_32.lib")

using namespace std;

void InitialIdf();

void ini(IndexAll &Index);

string handleQuery(IndexAll &Index, string query_str);

void preprocess(IndexAll &Index);

#endif //SAMPLE_CONNECTOR_INDEX_H
