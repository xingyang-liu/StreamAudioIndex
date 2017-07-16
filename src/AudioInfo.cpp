//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "AudioInfo.h"

using namespace std;

AudioInfo::AudioInfo(const AudioInfo& other)
{
    id=other.id;
    LikeCount = other.LikeCount;
    CommentCount = other.CommentCount;
    PlayCount = other.PlayCount;
    TagsSum = other.TagsSum;
    score = other.score;
    time = other.time;
    title = other.title;
    final=other.final;
    map<string, int> tmp = other.get();
    map<string, int>::iterator it;
    for (it = tmp.begin(); it != tmp.end(); it++)
    {
        TagsNum[it->first] = it->second;
    }
}

bool AudioInfo::operator==(const AudioInfo &other)
{
    return id == other.id && title == other.title && LikeCount == other.LikeCount \
        && CommentCount == other.CommentCount \
        && PlayCount == other.PlayCount && TagsSum == other.TagsSum&&final==other.final;
}

map<string, int> AudioInfo::get() const
{
    return TagsNum;
}

AudioInfo &AudioInfo::operator=(const AudioInfo&other)//按理说是应该加const，但是要写几个函数提取成员
{
    if (this == &other)
    {
        return *this;
    }
    else
    {
        TagsNum.clear();
        id=other.id;
        LikeCount = other.LikeCount;
        CommentCount = other.CommentCount;
        PlayCount = other.PlayCount;
        TagsSum = other.TagsSum;
        score = other.score;
        title = other.title;
        time = other.time;
        final=other.final;
        map<string, int> tmp = other.get();
        map<string, int>::iterator it;
        for (it = tmp.begin(); it != tmp.end(); it++)
        {
            //cout << it->first << "\t" << it->second << endl;
            TagsNum[it->first] = it->second;
        }
        return *this;
    }
}


void AudioInfo::update(AudioInfo& other)
{
    if (id!=other.id)
    {
//        cout<<"Can't update Audio."<<endl;
        exit(3);
    }
    LikeCount = other.LikeCount;
    CommentCount = other.CommentCount;
    PlayCount = other.PlayCount;
    TagsSum = other.TagsSum;
    score = other.score;
    time = other.time;
    final=other.final;
    map<string, int> tmp = other.get();
    map<string, int>::iterator it;
    for (it = tmp.begin(); it != tmp.end(); it++)
    {
        TagsNum[it->first] += it->second;
    }
}