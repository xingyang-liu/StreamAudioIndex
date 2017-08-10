//
// Created by billy on 17-8-10.
//

#ifndef HASH_0E_HYBRIDINDEXMANAGER_H
#define HASH_0E_HYBRIDINDEXMANAGER_H


#include "../PhonomeIndex/PhoIndexManager.h"
#include "../TextIndex/IndexManager.h"
#include <pthread.h>

class HybridIndexManager {
private:
    PhoIndexManager* PhonemeIndexes{};

    IndexManager* TextIndexes;

public:
    HybridIndexManager() {
        PhonemeIndexes = new PhoIndexManager();
        TextIndexes = new IndexManager();
    }

    explicit HybridIndexManager(int num) {
        PhonemeIndexes = new PhoIndexManager(num);
        TextIndexes = new IndexManager(num);
    }

    void output();

    void buildIndex(int audio_sum);

    void updateScore(int id,int score);

    /*  queryStr: string#!#phonemes#!#  */
    void handleQuery(char* queryStr);
};

//////////////用于多线程传递参数//////////////////
struct FamForTextIndex {
    IndexManager* textIndexes;
    int audioSum;
    string query;
};

struct FamForPhoneIndex {
    PhoIndexManager* phoneIndexes;
    int audioSum;
    vector<SimilarPhoneme> query;
};

//////////////用于多线程建立索引//////////////////
void* BuildThreadforText(void* obj);

void* BuildThreadforPhone(void* obj);

/////////////用于多线程搜索///////////////////
void* SearchThreadforText(void* obj);

void* SearchThreadforPhone(void* obj);

#endif //HASH_0E_HYBRIDINDEXMANAGER_H
