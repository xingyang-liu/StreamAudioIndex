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
    likeCount=other.likeCount;
    playCount=other.playCount;
    commentCount=other.commentCount;
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
        likeCount=other.likeCount>0?other.likeCount:likeCount;
        playCount=other.playCount>0?other.playCount:playCount;
        commentCount=other.commentCount>0?other.commentCount:commentCount;
        return *this;
    }
}


void AudioInfo::update(AudioInfo& other)
{
    if (id!=other.id)
    {
        exit(3);
    }
    TagsSum = other.TagsSum;
    score = other.score;
    time = other.time;
    final=other.final;
    likeCount=other.likeCount>0?other.likeCount:likeCount;
    playCount=other.playCount>0?other.playCount:playCount;
    commentCount=other.commentCount>0?other.commentCount:commentCount;
}

void AudioInfo::computeScore() {
    score = double(likeCount>0?likeCount:0)*0.05 + double(commentCount>0?commentCount:0)*0.25 + double(playCount>0?playCount:0)*0.01;
}