//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_CMP_H
#define HASH_0E_CMP_H

#include "IndexTemplate.h"

template <class T>
class IndexTemplate;

template <class T>
class Cmp {
public:
    T term;
    IndexTemplate<T>* myself;
    Cmp(T str,IndexTemplate<T>* me):term(str),myself(me){}

    virtual bool operator()(int a,int b) = 0;
};

template <class T>
class CmpForSig: public Cmp<T> {
public:
    CmpForSig(T str, IndexTemplate<T>* me) : Cmp<T>(str, me) {}

    bool operator()(int a,int b){return (*(Cmp<T>::myself->InfoTable))[a].score>(*(Cmp<T>::myself->InfoTable))[b].score;}

};

template <class T>
class CmpForFre: public Cmp<T> {
public:
    CmpForFre(T str, IndexTemplate<T> *me) : Cmp<T>(str, me) {}

    bool operator()(int a,int b){return (*(Cmp<T>::myself->InfoTable))[a].time>(*(Cmp<T>::myself->InfoTable))[b].time;}

};

template <class T>
class CmpForSim: public Cmp<T> {
public:
    CmpForSim(T str, IndexTemplate<T> *me) : Cmp<T>(str, me) {}

    map<int,NodeInfo*> &tmp=(*(*Cmp<T>::myself->TermIndex)[Cmp<T>::term]->nodeMap);
    bool operator()(int a,int b){return (tmp[a]->tf) > tmp[b]->tf;}

};

#endif //HASH_0E_CMP_H
