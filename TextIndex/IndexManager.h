//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_IndexManager_H
#define HASH_0E_IndexManager_H


#include "InvertedIndex.h"
#include "../TemplateIndex/IndexTemplate.h"



class IndexManager
{
private:
    int TitleWeigh = 200;
    int TagWeigh = 50;
    int ContentWeigh = 1;
public:
	int I0Num,I0TermNum,AudioSum;
	long totalTermSum;
    vector<ForMirror<InvertedIndex>*> mirrorList;
	btree::btree_map<int, InvertedIndex*> Indexes;
    btree::btree_map<int, btree::btree_map<int, NodeInfo *> > livePointer;
    btree::btree_map<int, CMutex> liveIdMutex;//only for livePointer
    CMutex mutexLive;//删除livePointer中某个id时防止进入,搜索I0时需要申请
	CMutex clearInvertedIndex;//正在给map赋新值，无关I0
    cppjieba::Jieba *jieba;
    btree::btree_map<string,IdfCode> IdfTable;
    int IdfCodeNum;
    int keywords_topk;//结巴提取关键词的个数

	IndexManager(){
		I0Num = 0;
        I0TermNum=0;
        AudioSum=0;
		totalTermSum=0;
        keywords_topk=5;
        IdfCodeNum=0;
		Indexes[0]=new InvertedIndex;
        jieba=new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
	}

	IndexManager(int num)
	{
		I0Num = 0;
        I0TermNum=0;
        AudioSum=0;
		totalTermSum=0;
        keywords_topk=5;
        IdfCodeNum=0;
        Indexes[0]=new InvertedIndex;
        jieba=new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
        initialIdf();
		cout << "Initialization of idf is okay." << endl;
		buildIndex(num);
	}

	void output();

	void buildIndex(int audio_sum);

    void createAudio(AudioInfo &audio,string &content);

    void addAudio(AudioInfo &audio, string &content);

	void initialIdf();

	void updateScore(int id,int score);

	void handleQuery(string queryStr, vector<string> &Result);

    ~IndexManager()
    {
        btree::btree_map<int,InvertedIndex*>::iterator it;
        for (it=Indexes.begin();it!=Indexes.end();it++)
        {
            delete((it->second));
        }
    }
};

class FamilyQuery
{
public:
    IndexManager *me;
//    vector<string> * que;
    vector<int> * que_code;
    vector<double> *que_idf;
    btree::btree_map<int, string> * na;
    vector<pair<int, double> > *resVec;
    btree::btree_map<int,score_ratio> *sco_ve;

    FamilyQuery(IndexManager* m,vector<int> * q_c,vector<double> *q_i,btree::btree_map<int, string> * n, vector<pair<int, double> > *r,\
		btree::btree_map<int,score_ratio> *sc)
    {
        me=m;
        que_code=q_c;
        que_idf=q_i;
        na=n;
        resVec=r;
        sco_ve=sc;
    }

};

void *searchThread(void *family);

class FamilyAll
{
public:
    IndexManager *me;
    AudioInfo *audio;
    btree::btree_map<int,double> *tags;
    int termCount;

    FamilyAll(IndexManager*myself,AudioInfo *audioInfo,btree::btree_map<int,double> *t,int tCount):
            me(myself),audio(audioInfo),tags(t),termCount(tCount) {}
};

void *addAudioAllThread(void *Family);//如果要实现多线程，就必须管控所有add与merger

#endif //HASH_0E_IndexManager_H
