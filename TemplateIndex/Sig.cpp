//
// Created by watson on 23/07/17.
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