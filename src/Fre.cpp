//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "Fre.h"

Fre::Fre(const Fre &other)
{
    id = other.get_id();
    fresh = other.get_fresh();
}

Fre & Fre::operator=(const Fre& other)
{
    if (this != &other)
    {
        id = other.get_id();
        fresh = other.get_fresh();
    }
    return *this;
}

