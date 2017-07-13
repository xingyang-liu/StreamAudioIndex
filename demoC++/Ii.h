//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_II_H
#define SAMPLE_CONNECTOR_II_H


#include "I0.h"
#include "utils.h"

class Ii
{
public:
	int AudioCount, level, id_tmp;
	map<string, vector<Fre> > *TermIndexFre;
	map<string, vector<Sig> > *TermIndexSig;
	map<string, vector<TermFreq> > *TermIndexSim;
	map<int, AudioInfo> *InfoTable;

	Ii();

	Ii(int level0, I0 &Index0);

	void output();

	int get_count();

//	void test();

	void MergerIndex(Ii &other);//并未考虑存在相同id的情况，否则请重载部分运算符//新的归并旧的

	void search(map<int, double> &Result, double &MinScore, int &AnsNum, int &Sum, const vector<string> query, map<int, string> &name);

	~Ii()
	{
		delete TermIndexFre;
		delete TermIndexSig;
		delete TermIndexSim;
		delete InfoTable;
	}
};

#endif //SAMPLE_CONNECTOR_II_H
