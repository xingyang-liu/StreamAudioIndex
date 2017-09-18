//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef HASH_0E_INFO_H
#define HASH_0E_INFO_H

#include "../utils/utils.h"

using namespace std;
class AudioInfo {
private:
	int likeCount;
	int commentCount;
	int playCount;

	void computeScore();
public:
    string title;
	string tags;
    int TagsSum;
    double time;
	int id;
	double score;
	int final;

    AudioInfo() {}

	AudioInfo(int id, string title, int score, int TagsSum, double time,int finalflag): id(id), title(title), score(score),\
 	TagsSum(TagsSum), time(time), final(finalflag){}

    AudioInfo(int id, string title, int TagsSum, double time,int finalflag, string tags = "", int playCount = -1, int commentCount = -1,\
	 int likeCount = -1) :id(id), title(title), TagsSum(TagsSum), time(time),final(finalflag), playCount(playCount),\
 	 commentCount(commentCount), likeCount(likeCount), tags(tags)
	{
		computeScore();
	}

    AudioInfo(const AudioInfo& other);

    bool operator==(const AudioInfo &other);

    AudioInfo &operator=(const AudioInfo&other);//按理说是应该加const，但是要写几个函数提取成员

	void update(AudioInfo& other);
};


#endif //HASH_0E_INFO_H
