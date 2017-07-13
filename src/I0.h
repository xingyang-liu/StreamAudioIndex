//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_I0_H
#define SAMPLE_CONNECTOR_I0_H



#include "AudioInfo.h"
#include "Fre.h"
#include "TermFreq.h"
#include "Sig.h"
#include "utils.h"

using namespace std;

class I0
{
public:

    map<int, AudioInfo> *InfoTable;
    map<string, vector<Fre> > *TermIndex;
    int AudioCount;

    I0();

    void addAudio(int &id, string title, int &LikeCount, int &CommentCount, int &PlayCount, double &score, \
		map<string, int> &TagsNum, int &TagsSum, double &time);

    void term_add(string term, int id, double fresh);

    void I0_sort();

    void output();

    AudioInfo search_id(int id) { return (*InfoTable)[id]; }

    int get_count() { return AudioCount; }

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> &query, map<int, string> &name);

    void clear();

    ~I0() { delete InfoTable; delete TermIndex; }
};


#endif //SAMPLE_CONNECTOR_I0_H
