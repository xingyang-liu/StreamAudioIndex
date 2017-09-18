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

    /*************Create***************
     * Description: When the program has been created and new content comes up, you can use this function to add content.
     * @param AudioInfo including title, tags, finalFlag, time and score (or likeCount etc.)
     * @param content the audio's content.
     */
    void createAudio(AudioInfo &audio, string &content);

    /*************Add***************
     * Description: When the program has been created and new content comes up, you can use this function to add content.
     * @param AudioInfo including finalFlag, time and score (or likeCount etc.)
     * @param content the audio's content.
     */
    void addAudio(AudioInfo &audio, string &content);
    void addAudio(AudioInfo &audio, vector<SimilarPhoneme> &content);

    /*************Query***************
     * Description: Use your keywords to search in the index.
     * @param queryStr The string keywords to search in string index.
     * @param queryPho The phoneme keywords to search in phoneme index.
     */
    void handleQuery(string queryStr, vector<SimilarPhoneme> queryPho);
    void handleQuery(string queStr);
    void handleQuery(vector<SimilarPhoneme> queryPho);
};

//////////////用于多线程传递参数//////////////////
struct FamForTextIndex {
    IndexManager* textIndexes;
    int audioSum;
    string query;
    vector<string> result;
};

struct FamForPhoneIndex {
    PhoIndexManager* phoneIndexes;
    int audioSum;
    vector<SimilarPhoneme> query;
    vector<string> result;
};

//////////////用于多线程建立索引//////////////////
void* BuildThreadforText(void* obj);

void* BuildThreadforPhone(void* obj);

/////////////用于多线程搜索///////////////////
void* SearchThreadforText(void* obj);

void* SearchThreadforPhone(void* obj);

#endif //HASH_0E_HYBRIDINDEXMANAGER_H
