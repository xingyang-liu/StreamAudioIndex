//
// Created by billy on 17-8-5.
//

#ifndef HASH_0E_I0SORTTHREADSIG_H
#define HASH_0E_I0SORTTHREADSIG_H

#include "InvertedIndex0SortThread.h"

template <class T>
class I0SortThreadSig: public InvertedIndex0SortThread<T> {
public:
    explicit I0SortThreadSig(void* fam): InvertedIndex0SortThread<T>(fam) {}

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
        return (*(InvertedIndex0SortThread<T>::myself)->InfoTable)[n->id].score;
    }
};

#endif //HASH_0E_I0SORTTHREADSIG_H
