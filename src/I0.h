
#ifndef SAMPLE_CONNECTOR_I0_H
#define SAMPLE_CONNECTOR_I0_H



#include "AudioInfo.h"
#include "utils.h"

using namespace std;

class I0
{
public:

    map<int, AudioInfo> *InfoTable;
    map<string, vector<int> > *TermIndex;
	map<string, CMutex> TermMutex;

	CMutex MutexInfo;
    int AudioCount;
	CMutex TermIndexMutex;

    I0();

	I0(const I0&other)
	{
		AudioCount=other.AudioCount;
		InfoTable=new map<int,AudioInfo>;
		TermIndex=new map<string,vector<int> >;
		if(other.InfoTable!=NULL){
			map<int,AudioInfo>::iterator it_info;
			for (it_info=other.InfoTable->begin();it_info!=other.InfoTable->end();it_info++)
			{
				(*InfoTable)[it_info->first]=it_info->second;
			}
			map<string,vector<int> >::iterator it_fre;
			for (it_fre=other.TermIndex->begin();it_fre!=other.TermIndex->end();it_fre++) {
				(*TermIndex)[it_fre->first] = it_fre->second;
			}
		}

	}

	void addAudio(AudioInfo &tmp_info);

    void term_add(string term, int id, double fresh);

    void I0_sort();

    void output();

    AudioInfo search_id(int id) { return (*InfoTable)[id]; }

    int get_count() { return AudioCount; }

    void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> &query, map<int, string> &name);

    void clear();

	void updateScore(int id,double score)
	{
		MutexInfo.Lock();
		(*InfoTable)[id].score=score;
		MutexInfo.Unlock();
	}

    ~I0() {
        map<int,AudioInfo> &other=*InfoTable;
        map<string,vector<int>> &tmp1=*TermIndex;
        if(InfoTable!=NULL)
        {
            InfoTable->clear();
            delete InfoTable;
            InfoTable=NULL;
        }
        if(TermIndex!=NULL)
        {
            map<string,vector<int>>::iterator it_fre;
            for (it_fre=TermIndex->begin();it_fre!=TermIndex->end();it_fre++)
            {
                it_fre->second.clear();
            }
            TermIndex->clear();
            delete TermIndex;
            TermIndex=NULL;
        }
    }
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
