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
	map<string, CMutex> TermMutex;
	CMutex MutexInfo;
    int AudioCount;

    I0();

	void addAudio(AudioInfo &tmp_info);

    void term_add(string term, int id, double fresh);

    void I0_sort();

    void output();

    AudioInfo search_id(int id) { return (*InfoTable)[id]; }

    int get_count() { return AudioCount; }

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> &query, map<int, string> &name);

    void clear();

    ~I0() { if(InfoTable!=NULL) delete InfoTable; if(TermIndex!=NULL) delete TermIndex;  }
};


class FamilyI0
{
public:
	I0*me;
	I0*him;

	FamilyI0(I0* myself,I0* other)
	{
		me=myself;
		him=other;
	}
};


#endif //SAMPLE_CONNECTOR_I0_H
