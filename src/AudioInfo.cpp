//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "AudioInfo.h"

using namespace std;

AudioInfo::AudioInfo(const AudioInfo& other)
{
    id=other.id;
    TagsSum = other.TagsSum;
    score = other.score;
    time = other.time;
    title = other.title;
    final=other.final;
}

bool AudioInfo::operator==(const AudioInfo &other)
{
    return id == other.id && title == other.title && TagsSum == other.TagsSum;
}

AudioInfo &AudioInfo::operator=(const AudioInfo&other)//按理说是应该加const，但是要写几个函数提取成员
{
    if (this == &other)
    {
        return *this;
    }
    else
    {
        id=other.id;
        TagsSum = other.TagsSum;
        score = other.score;
        title = other.title;
        time = other.time;
        final=other.final;
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
    TagsSum = other.TagsSum;
    score = other.score;
    time = other.time;
    final=other.final;
}