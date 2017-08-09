//
// Created by billy on 17-8-5.
//

#ifndef HASH_0E_I0SORTTHREADFRE_H
#define HASH_0E_I0SORTTHREADFRE_H

#include "InvertedIndex0SortThread.h"

template <class T>
class I0SortThreadFre: public InvertedIndex0SortThread<T> {
public:
    explicit I0SortThreadFre(void* fam): InvertedIndex0SortThread<T>(fam) {}

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
        return (*(InvertedIndex0SortThread<T>::myself)->InfoTable)[n->id].time;
    }
};

#endif //HASH_0E_I0SORTTHREADFRE_H
