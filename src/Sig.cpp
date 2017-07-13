//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "Sig.h"

Sig::Sig(const Sig &other)
{
    id = other.get_id();
    score = other.get_score();
}

Sig & Sig::operator=(const Sig &other)
{
    if (this == &other)
    {
        return *this;
    }
    else
    {
        id = other.get_id();
        score = other.get_score();
        return *this;
    }
}