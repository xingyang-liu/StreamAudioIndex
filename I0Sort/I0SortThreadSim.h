//
// Created by billy on 17-8-5.
//

#ifndef HASH_0E_I0SORTTHREADSIM_H
#define HASH_0E_I0SORTTHREADSIM_H

#include "InvertedIndex0SortThread.h"

template <class T>
class I0SortThreadSim: public InvertedIndex0SortThread<T> {
public:
    explicit I0SortThreadSim(void* fam): InvertedIndex0SortThread<T>(fam) {}

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

#endif //HASH_0E_I0SORTTHREADSIM_H
