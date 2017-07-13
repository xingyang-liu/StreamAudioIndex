//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_INDEXALL_H
#define SAMPLE_CONNECTOR_INDEXALL_H

#include "utils.h"
#include "I0.h"
#include "Ii.h"

class IndexAll
{
public:
	int I0Num;
	I0 Index0;
	map<int, Ii*> otherIndex;

	IndexAll()
	{
		I0Num = 0;
	}

	void addAudio(int id, string title, int LikeCount, int CommentCount, int PlayCount, double score, map<string, int> TagsNum, int TagsSum, double time);

	void output();

	void get_count();

	vector<pair<int, double> > search(const vector<string> &query, map<int, string> &name);
};


#endif //SAMPLE_CONNECTOR_INDEXALL_H
