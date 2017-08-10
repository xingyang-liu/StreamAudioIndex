//
// Created by billy on 17-8-10.
//

#include "HybridIndexManager.h"

void HybridIndexManager::output() {
    cout << "**********Text Indexing**********\n";
    TextIndexes->output();
    cout << "\n*********Phoneme Indexing*********\n";
    PhonemeIndexes->output();
}

void HybridIndexManager::buildIndex(int audio_sum) {
    pthread_t buildForText, buildForPho;

    FamForTextIndex* FamText = new FamForTextIndex;
    FamText->audioSum = audio_sum;
    FamText->textIndexes = TextIndexes;
    pthread_create(&buildForText, NULL, BuildThreadforText, (void*)FamText);

    FamForPhoneIndex* FamPhone = new FamForPhoneIndex;
    FamPhone->audioSum = audio_sum;
    FamPhone->phoneIndexes = PhonemeIndexes;
    pthread_create(&buildForPho, NULL, BuildThreadforPhone, (void*)FamPhone);

    pthread_join(buildForText, NULL);
    pthread_join(buildForPho, NULL);

    delete FamPhone;
    delete FamText;
}

void* BuildThreadforText(void* obj) {
    auto * TextIndexes = (FamForTextIndex*) obj;
    TextIndexes->textIndexes->buildIndex(TextIndexes->audioSum);
}

void* BuildThreadforPhone(void* obj) {
    auto * TextIndexes = (FamForPhoneIndex*) obj;
    TextIndexes->phoneIndexes->buildIndex(TextIndexes->audioSum);
}

void HybridIndexManager::updateScore(int id, int score) {
    TextIndexes->updateScore(id, score);
    PhonemeIndexes->updateScore(id, score);
}

void HybridIndexManager::handleQuery(char *query) {
    pthread_t searchForText, searchForPhone;

    char* temp;
    temp = strtok(query, "#!#");
    string queryStr(temp);
    FamForTextIndex* FamText = new FamForTextIndex;
    FamText->textIndexes = TextIndexes;
    FamText->query = queryStr;
    pthread_create(&searchForText, NULL, SearchThreadforText, (void*)FamText);

    vector<SimilarPhoneme> queryPhone;
    while (temp != NULL){
        temp = strtok(NULL, "#!#");
        auto phone = (float*)temp;
        auto tmpPhone = SimilarPhoneme(phone);
        queryPhone.push_back(tmpPhone);
    }
    FamForPhoneIndex* FamPhone = new FamForPhoneIndex;
    FamPhone->phoneIndexes = PhonemeIndexes;
    FamPhone->query = queryPhone;
    pthread_create(&searchForPhone, NULL, SearchThreadforPhone, (void*)FamPhone);

    pthread_join(searchForText, NULL);
    pthread_join(searchForPhone, NULL);
    delete FamText;
    delete FamPhone;
}

void* SearchThreadforPhone(void* obj) {
    FamForPhoneIndex* fam = (FamForPhoneIndex*) obj;
    fam->phoneIndexes->handleQuery(fam->query);
}

void* SearchThreadforText(void* obj) {
    FamForTextIndex* fam = (FamForTextIndex*) obj;
    fam->textIndexes->handleQuery(fam->query);
}