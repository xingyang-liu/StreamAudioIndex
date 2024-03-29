//
// Created by watson on 18/07/17.
//

#ifndef HASH_0E_ProgramList_H
#define HASH_0E_ProgramList_H

#include "NodeInfo.h"

class ProgramList
{
public:
    NodeInfo *max_fresh;
    NodeInfo *max_sig;
    NodeInfo *max_termFreq;
    btree::btree_map<int,NodeInfo*> *nodeMap;
    CMutex mutex;

    ProgramList() {

        max_termFreq=NULL;max_fresh=NULL;max_sig=NULL;nodeMap=NULL;
    }

    NodeInfo* addNode(double tf,int id);

    ProgramList(const ProgramList&other);

    ProgramList* clone();

    void output();

    ProgramList &operator=(const ProgramList&other);

    ~ProgramList();

};

#endif //HASH_0E_ProgramList_H
