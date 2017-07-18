//
// Created by watson on 15/07/17.
//

#ifndef HASH_0E_ILIVE_H
#define HASH_0E_ILIVE_H


#include "AudioInfo.h"
#include "Fre.h"
#include "TermFreq.h"
#include "Sig.h"
#include "utils.h"


using namespace std;

class Ilive
{
public:
    map<int, AudioInfo> *InfoTable;
    map<string, std::list<Fre> > *TermIndex;
    map<string, CMutex> TermMutex;
    CMutex MutexInfo;
    int AudioCount;

    Ilive();

    void addAudio(AudioInfo &tmp_info);


    void term_add(string term, int id, double fresh);

//    void Ilive_sort();

    void output();

    AudioInfo search_id(int id) { return (*InfoTable)[id]; }

    int get_count() { return AudioCount; }

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> &query, map<int, string> &name);

    void clear();

    void term_remove(string term, int id);

    ~Ilive() { if(InfoTable!=NULL) delete InfoTable; if(TermIndex!=NULL) delete TermIndex;  }
};





#endif //HASH_0E_ILIVE_H
