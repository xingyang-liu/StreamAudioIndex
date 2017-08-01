//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_INVERTEDINDEXMERGERTHREADSIM_H
#define HASH_0E_INVERTEDINDEXMERGERTHREADSIM_H

#include "InvertedIndexMergerThread.h"

template <class T>
class InvertedIndexMergerThreadSim: public InvertedIndexMergerThread<T> {
public:
    InvertedIndexMergerThreadSim(void *fam): InvertedIndexMergerThread<T>(fam) {}

protected:
    NodeInfo* get_next(ProgramList* p) {
        return p->max_termFreq;
    }

    NodeInfo* get_next(NodeInfo* n) {
        return n->next_termFreq;
    }

    NodeInfo* set_next(ProgramList* p, NodeInfo* des) {
        p->max_termFreq = des;
        return des;
    }

    NodeInfo* set_next(NodeInfo* n, NodeInfo* des) {
        n->next_termFreq = des;
        return des;
    }

    double get_value(NodeInfo* n) {
        return n->tf;
    }
};

#endif //HASH_0E_INVERTEDINDEXMERGERTHREADSIM_H
