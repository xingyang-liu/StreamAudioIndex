//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_INFO_H
#define SAMPLE_CONNECTOR_INFO_H

#include <string>
#include <map>
//#include <iostream>

using namespace std;
class AudioInfo {
public:
    int id;
    string title;
    int LikeCount, CommentCount, PlayCount, TagsSum;
    double score, time;
    map<string, int> TagsNum;
	int final;

    AudioInfo() {}

    AudioInfo(int id, string title, int LikeCount, int CommentCount, int PlayCount, double score, \
	map<string, int>TagsNum, int TagsSum, double time,int finalflag)\
		:id(id), title(title), LikeCount(LikeCount), CommentCount(CommentCount),\
		 PlayCount(PlayCount), score(score), TagsNum(TagsNum), TagsSum(TagsSum), time(time),final(finalflag){}

    AudioInfo(const AudioInfo& other);

    bool operator==(const AudioInfo &other);

    map<string, int> get() const;

    AudioInfo &operator=(const AudioInfo&other);//按理说是应该加const，但是要写几个函数提取成员

	void update(AudioInfo& other);
};


#endif //SAMPLE_CONNECTOR_INFO_H
