//
// Created by watson on 18/07/17.
//


#include "ListInfo.h"

NodeInfo::NodeInfo(){next_fresh=NULL;next_sig=NULL;next_termFreq=NULL;}

NodeInfo::NodeInfo(const NodeInfo&other)
{
    next_fresh=other.next_fresh;
    next_sig=other.next_sig;
    next_termFreq=other.next_termFreq;
    tf=other.tf;
    id=other.id;
}

NodeInfo &NodeInfo::operator=(const NodeInfo&other)
{
    if(this==&other)
    {
        return *this;
    }
    else
    {
        next_fresh=other.next_fresh;
        next_sig=other.next_sig;
        next_termFreq=other.next_termFreq;
        tf=other.tf;
        id=other.id;
        return *this;
    }

}

