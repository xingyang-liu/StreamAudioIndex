//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_SPECIFICMERGERTHREAD_H
#define HASH_0E_SPECIFICMERGERTHREAD_H

#include "InvertedIndexMergerThread.h"

template <class T>
class InvertedIndexMergerThreadFre: public InvertedIndexMergerThread<T> {
public:
    InvertedIndexMergerThreadFre(void *fam): InvertedIndexMergerThread<T>(fam) {}

protected:
    NodeInfo* get_next(ProgramList* p) {
        return p->max_fresh;
    }

    NodeInfo* get_next(NodeInfo* n) {
        return n->next_fresh;
    }

    NodeInfo* set_next(ProgramList* p, NodeInfo* des) {
        p->max_fresh = des;
        return des;
    }

    NodeInfo* set_next(NodeInfo* n, NodeInfo* des) {
        n->next_fresh = des;
        return des;
    }

    double get_value(NodeInfo* n) {
        return (*(InvertedIndexMergerThread<T>::myself)->InfoTable)[n->id].time;
    }
};



#endif //HASH_0E_SPECIFICMERGERTHREAD_H
