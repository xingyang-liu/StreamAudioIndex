//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "TermFreq.h"

TermFreq::TermFreq(const TermFreq&other)
{
    id = other.get_id();
    tf = other.get_tf();
}

TermFreq & TermFreq::operator=(const TermFreq &other)
{
    if (this == &other)
    {
        return *this;
    }
    else
    {
        id = other.get_id();
        tf = other.get_tf();
        return *this;
    }
}