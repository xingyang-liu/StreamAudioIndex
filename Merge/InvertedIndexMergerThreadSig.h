//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_INVERTEDINDEXMERGERTHREADSIG_H
#define HASH_0E_INVERTEDINDEXMERGERTHREADSIG_H

#include "InvertedIndexMergerThread.h"

template <class T>
class InvertedIndexMergerThreadSig: public InvertedIndexMergerThread<T> {
public:
    InvertedIndexMergerThreadSig(void *fam): InvertedIndexMergerThread<T>(fam) {}

protected:
    NodeInfo* get_next(ProgramList* p) {
        return p->max_sig;
    }

    NodeInfo* get_next(NodeInfo* n) {
        return n->next_sig;
    }

    NodeInfo* set_next(ProgramList* p, NodeInfo* des) {
        p->max_sig = des;
        return des;
    }

    NodeInfo* set_next(NodeInfo* n, NodeInfo* des) {
        n->next_sig = des;
        return des;
    }

    double get_value(NodeInfo* n) {
        return (*(InvertedIndexMergerThread<T>::myself)->InfoTable)[n->id].score;
    }
};

#endif //HASH_0E_INVERTEDINDEXMERGERTHREADSIG_H
