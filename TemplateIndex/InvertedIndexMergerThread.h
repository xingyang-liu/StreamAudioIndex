//
// Created by billy on 17-7-24.
//


#ifndef HASH_0E_INVERTEDINDEXMERGERTHREAD_H
#define HASH_0E_INVERTEDINDEXMERGERTHREAD_H

#include "../BasicStructure/NodeInfo.h"
#include "IndexTemplate.h"

template <class T>
class FamilyMerger;

template <class T>
class InvertedIndexMergerThread {
public:
    InvertedIndexMergerThread(void *fam){
        FamilyMerger<T> * ones=(FamilyMerger<T> *)fam;
        myself=ones->me;
        other=ones->him;
    }

    void excecuteMerge();

protected:
    virtual double get_value(NodeInfo* n)=0;
    virtual NodeInfo* get_next(ProgramList* p)=0; //获得p的max_fre
    virtual NodeInfo* get_next(NodeInfo* n)=0; //获得p的next_fre
    virtual NodeInfo* set_next(ProgramList* p, NodeInfo* des)=0; //将p的max_fre设为des,返回des
    virtual NodeInfo* set_next(NodeInfo* n, NodeInfo* des)=0; //将p的next_fre设为des，返回des

    IndexTemplate<T>* myself;
    IndexTemplate<T>* other;
};

template <class T>
void InvertedIndexMergerThread<T>::excecuteMerge() {
    typename map<T,ProgramList*>::iterator it_list_i;
    typename map<T,ProgramList*>::iterator it_list_j;
    map<int,NodeInfo*>::iterator it_node_i;
    map<int,NodeInfo*>::iterator it_node_j;

    NodeInfo* pointer_i;
    NodeInfo* pointer_j;
    NodeInfo *tmp_node;
    NodeInfo* del_tmp;
    NodeInfo *val;

    map<T,ProgramList*> &tmp_pro=*myself->TermIndex;
    map<T,ProgramList*> &tmp1_pro=*other->TermIndex;



    for(it_list_i=myself->TermIndex->begin();it_list_i!=myself->TermIndex->end();it_list_i++) {




        it_list_j = other->TermIndex->find(it_list_i->first);


        map<int,NodeInfo*> &tmp_hi=*it_list_i->second->nodeMap;
        ProgramList &tmp_proi=*it_list_i->second;



//        if(tmp_hj.begin()->second->id==11285443)
//        {
//            map<int,NodeInfo*>::iterator it_node_t=tmp_hj.begin();
//            it_node_t++;
//            if(it_node_t!=tmp_hj.end()&&it_node_t->second->id==11285442)
//            {
//                cout<<endl;
//            }
//        }

//        判断pointer_j是不是由于flag可能需要清空

        if (it_list_j != other->TermIndex->end())//说明i的这一项j中有
        {
            map<int,NodeInfo*> &tmp_hj=*it_list_j->second->nodeMap;
            ProgramList &tmp_proj=*it_list_j->second;
            if (get_next(it_list_i->second) == get_next(it_list_j->second))//如果i,j相同，则说明Indexi是Indexj拷贝过来的
            {
                val = get_next(it_list_j->second);
                if (val->flag == -1) {
                    pointer_j = get_next(it_list_j->second);
                } else {
                    pointer_j = get_next(it_list_j->second);
                    while (pointer_j != NULL && pointer_j->flag >= 0) {
                        del_tmp = pointer_j;
                        pointer_j = get_next(pointer_j);
                        del_tmp->flag += 1;
                        if (del_tmp->flag == 3) {
                            it_list_j->second->mutex.Lock();
                            if (it_list_j->second->nodeMap->find(del_tmp->id) != it_list_j->second->nodeMap->end()) {
                                it_list_j->second->nodeMap->erase(del_tmp->id);
                                delete del_tmp;

                            }

                            it_list_j->second->mutex.Unlock();
                        }
                    }
                }

                if (pointer_j != NULL) {
                    tmp_node = set_next(it_list_i->second, pointer_j);
                    pointer_j = get_next(pointer_j);

                    while (pointer_j != NULL&& pointer_j->flag >= 0) {
                        del_tmp = pointer_j;
                        pointer_j = get_next(pointer_j);
                        del_tmp->flag += 1;
                        if (del_tmp->flag == 3) {
                            it_list_j->second->mutex.Lock();
                            if (it_list_j->second->nodeMap->find(del_tmp->id) != it_list_j->second->nodeMap->end()) {
                                it_list_j->second->nodeMap->erase(del_tmp->id);
                                delete del_tmp;
                            }

                            it_list_j->second->mutex.Unlock();
                        }

                    }
                    set_next(tmp_node, NULL);
                } else {
                    set_next(it_list_i->second, NULL);
                }
            } else {//反之，说明不是拷贝过来的，但是可能会因为flag的原因去掉一些
                val = get_next(it_list_j->second);
                if (val->flag == -1) {
                    pointer_j = get_next(it_list_j->second);
                } else {
                    pointer_j = get_next(it_list_j->second);
                    while (pointer_j != NULL && pointer_j->flag >= 0) {
                        del_tmp = pointer_j;
                        if (pointer_j == get_next(pointer_j)) {
                            cout << endl;
                        }
                        pointer_j = get_next(pointer_j);
                        del_tmp->flag += 1;
                        if (del_tmp->flag == 3) {

                            it_list_j->second->mutex.Lock();
                            if (it_list_j->second->nodeMap->find(del_tmp->id) != it_list_j->second->nodeMap->end()) {
                                it_list_j->second->nodeMap->erase(del_tmp->id);
                                delete del_tmp;//如果map中没有，说明已经被处理过了
                            }
                            it_list_j->second->mutex.Unlock();
                        }
                    }
                }

                if (pointer_j != NULL) {//如果j不是空集
                    val = get_next(it_list_i->second);
                    if (val->flag == -1) {
                        pointer_i = get_next(it_list_i->second);
                    } else {
                        pointer_i = get_next(it_list_i->second);
                        while (pointer_i != NULL && pointer_i->flag >= 0) {
                            del_tmp = pointer_i;
//                            if (pointer_i == get_next(pointer_i)) {
//                                cout << endl;
//                            }
                            pointer_i = get_next(pointer_i);
                            del_tmp->flag += 1;
                            if (del_tmp->flag == 3) {
                                it_list_i->second->mutex.Lock();
                                if (it_list_i->second->nodeMap->find(del_tmp->id) !=it_list_i->second->nodeMap->end()) {
                                    it_list_i->second->nodeMap->erase(del_tmp->id);
                                    delete del_tmp;
                                }

                                it_list_i->second->mutex.Unlock();
                            }
                        }
                    }

                    if (pointer_i == NULL) {//如果i因为flag的原因是空集
                        tmp_node = set_next(it_list_i->second, pointer_j);
                        pointer_j = get_next(pointer_j);

                        while (pointer_j != NULL) {
                            set_next(tmp_node, pointer_j);
                            tmp_node = pointer_j;
//                            if (pointer_j == get_next(pointer_j)) {
//                                cout << endl;
//                            }
                            pointer_j = get_next(pointer_j);

                            while (pointer_j != NULL && pointer_j->flag >= 0) {
                                del_tmp = pointer_j;
                                pointer_j = get_next(pointer_j);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_j->second->mutex.Lock();
                                    if (it_list_j->second->nodeMap->find(del_tmp->id) != it_list_j->second->nodeMap->end()) {
                                        it_list_j->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }

                                    it_list_j->second->mutex.Unlock();
                                }
                            }
                        }
                        set_next(tmp_node, NULL);
                    } else //两个list都有项，开始merge
                    {
                        if (get_value(pointer_i) > get_value(pointer_j)) {//选择max
                            set_next(it_list_i->second, pointer_i);

//                            if (pointer_i == get_next(pointer_i)) {
//                                cout << endl;
//                            }
                            pointer_i = get_next(pointer_i);

                            while (pointer_i != NULL && pointer_i->flag >= 0) {
                                del_tmp = pointer_i;
                                pointer_i = get_next(pointer_i);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_i->second->mutex.Lock();
                                    if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                        it_list_i->second->nodeMap->end()) {
                                        it_list_i->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }

                                    it_list_i->second->mutex.Unlock();
                                }
                            }
                        } else {
                            set_next(it_list_i->second, pointer_j);
//                            if (pointer_j == get_next(pointer_j)) {
//                                cout << endl;
//                            }
                            pointer_j = get_next(pointer_j);
                            while (pointer_j != NULL && pointer_j->flag >= 0) {

                                del_tmp = pointer_j;
                                if (pointer_i == get_next(pointer_i)) {
                                    cout << endl;
                                }
                                pointer_j = get_next(pointer_j);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_j->second->mutex.Lock();
                                    if (it_list_j->second->nodeMap->find(del_tmp->id) !=
                                        it_list_j->second->nodeMap->end()) {
                                        it_list_j->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }

                                    it_list_j->second->mutex.Unlock();
                                }
                            }
                        }

                        tmp_node = get_next(it_list_i->second);

                        while (pointer_i != NULL && pointer_j != NULL) {
                            if (get_value(pointer_i) > get_value(pointer_j)) {
                                tmp_node = set_next(tmp_node, pointer_i);
//                                if (pointer_i == get_next(pointer_i)) {
//                                    cout << endl;
//                                }
                                pointer_i = get_next(pointer_i);
                                while (pointer_i != NULL && pointer_i->flag >= 0) {

                                    del_tmp = pointer_i;
                                    pointer_i = get_next(pointer_i);
                                    del_tmp->flag += 1;
                                    if (del_tmp->flag == 3) {
                                        it_list_i->second->mutex.Lock();
                                        if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                            it_list_i->second->nodeMap->end()) {
                                            it_list_i->second->nodeMap->erase(del_tmp->id);
                                            delete del_tmp;
                                        }

                                        it_list_i->second->mutex.Unlock();
                                    }
                                }
                            } else {
                                tmp_node = set_next(tmp_node, pointer_j);
//                                if (pointer_i == get_next(pointer_i)) {
//                                    cout << endl;
//                                }
                                pointer_j = get_next(pointer_j);
                                while (pointer_j != NULL && pointer_j->flag >= 0) {

                                    del_tmp = pointer_j;
                                    pointer_j = get_next(pointer_j);
                                    del_tmp->flag += 1;
                                    if (del_tmp->flag == 3) {
                                        it_list_j->second->mutex.Lock();

                                        if (it_list_j->second->nodeMap->find(del_tmp->id) !=
                                            it_list_j->second->nodeMap->end()) {
                                            it_list_j->second->nodeMap->erase(del_tmp->id);
                                            delete del_tmp;
                                        }

                                        it_list_j->second->mutex.Unlock();
                                    }
                                }
                            }
                        }

                        while (pointer_i != NULL) {
                            tmp_node = set_next(tmp_node, pointer_i);
                            if (pointer_i == get_next(pointer_i)) {
                                cout << endl;
                            }
                            pointer_i = get_next(pointer_i);
                            while (pointer_i != NULL && pointer_i->flag >= 0) {

                                del_tmp = pointer_i;
                                pointer_i = get_next(pointer_i);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_i->second->mutex.Lock();
                                    if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                        it_list_i->second->nodeMap->end()) {
                                        it_list_i->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }

                                    it_list_i->second->mutex.Unlock();
                                }
                            }
                        }

                        while (pointer_j != NULL) {
                            tmp_node = set_next(tmp_node, pointer_j);
                            pointer_j = get_next(pointer_j);
                            while (pointer_j != NULL && pointer_j->flag >= 0) {

                                del_tmp = pointer_j;
                                pointer_j = get_next(pointer_j);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_j->second->mutex.Lock();
                                    if (it_list_j->second->nodeMap->find(del_tmp->id) !=
                                        it_list_j->second->nodeMap->end()) {
                                        it_list_j->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }

                                    it_list_j->second->mutex.Unlock();
                                }
                            }
                        }
                        set_next(tmp_node, NULL);
                    }
                } else {//如果j是空集，只有myself里面有,那就简单去掉就好
                    val = get_next(it_list_i->second);
                    if (val->flag == -1) {
                        pointer_i = get_next(it_list_i->second);
                    } else {
                        pointer_i = get_next(it_list_i->second);
                        while (pointer_i != NULL && pointer_i->flag >= 0) {

                            del_tmp = pointer_i;
                            pointer_i = get_next(pointer_i);
                            del_tmp->flag += 1;
                            if (del_tmp->flag == 3) {
                                it_list_i->second->mutex.Lock();
                                if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                    it_list_i->second->nodeMap->end()) {
                                    it_list_i->second->nodeMap->erase(del_tmp->id);
                                    delete del_tmp;
                                }

                                it_list_i->second->mutex.Unlock();
                            }
                        }
                    }
                    if (pointer_i == NULL) {
                        set_next(it_list_i->second, NULL);
                    } else {
                        tmp_node = set_next(it_list_i->second, pointer_i);
                        pointer_i = get_next(pointer_i);

                        while (pointer_i != NULL && pointer_i->flag >= 0) {

                            del_tmp = pointer_i;
                            pointer_i = get_next(pointer_i);
                            del_tmp->flag += 1;
                            if (del_tmp->flag == 3) {
                                it_list_i->second->mutex.Lock();
                                if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                    it_list_i->second->nodeMap->end()) {
                                    it_list_i->second->nodeMap->erase(del_tmp->id);
                                    delete del_tmp;
                                }

                                it_list_i->second->mutex.Unlock();
                            }
                        }
                        while (pointer_i != NULL) {
                            tmp_node = set_next(tmp_node, pointer_i);
                            pointer_i = get_next(pointer_i);
                            while (pointer_i != NULL && pointer_i->flag >= 0) {

                                del_tmp = pointer_i;
                                pointer_i = get_next(pointer_i);
                                del_tmp->flag += 1;
                                if (del_tmp->flag == 3) {
                                    it_list_i->second->mutex.Lock();
                                    if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                        it_list_i->second->nodeMap->end()) {
                                        it_list_i->second->nodeMap->erase(del_tmp->id);
                                        delete del_tmp;
                                    }
                                    it_list_i->second->mutex.Unlock();
                                }
                            }
                        }
                        set_next(tmp_node, NULL);
                    }
                }
            }
        }else{//i中这一项，j中没有
            val = get_next(it_list_i->second);
            if (val->flag == -1) {
                pointer_i = get_next(it_list_i->second);
            } else {
                pointer_i = get_next(it_list_i->second);
                while (pointer_i != NULL && pointer_i->flag >= 0) {

                    del_tmp = pointer_i;
                    pointer_i = get_next(pointer_i);
                    del_tmp->flag += 1;
                    if (del_tmp->flag == 3) {
                        it_list_i->second->mutex.Lock();
                        if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                            it_list_i->second->nodeMap->end()) {
                            it_list_i->second->nodeMap->erase(del_tmp->id);
                            delete del_tmp;
                        }

                        it_list_i->second->mutex.Unlock();
                    }
                }
            }
            if (pointer_i == NULL) {
                set_next(it_list_i->second, NULL);
            } else {
                tmp_node = set_next(it_list_i->second, pointer_i);
                pointer_i = get_next(pointer_i);

                while (pointer_i != NULL && pointer_i->flag >= 0) {

                    del_tmp = pointer_i;
                    pointer_i = get_next(pointer_i);
                    del_tmp->flag += 1;
                    if (del_tmp->flag == 3) {
                        it_list_i->second->mutex.Lock();
                        if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                            it_list_i->second->nodeMap->end()) {
                            it_list_i->second->nodeMap->erase(del_tmp->id);
                            delete del_tmp;
                        }

                        it_list_i->second->mutex.Unlock();
                    }
                }
                while (pointer_i != NULL) {
                    tmp_node = set_next(tmp_node, pointer_i);
                    pointer_i = get_next(pointer_i);
                    while (pointer_i != NULL && pointer_i->flag >= 0) {

                        del_tmp = pointer_i;
                        pointer_i = get_next(pointer_i);
                        del_tmp->flag += 1;
                        if (del_tmp->flag == 3) {
                            it_list_i->second->mutex.Lock();
                            if (it_list_i->second->nodeMap->find(del_tmp->id) !=
                                it_list_i->second->nodeMap->end()) {
                                it_list_i->second->nodeMap->erase(del_tmp->id);
                                delete del_tmp;
                            }

                            it_list_i->second->mutex.Unlock();
                        }
                    }
                }
                set_next(tmp_node, NULL);
            }
        }
    }
}


#endif //HASH_0E_INVERTEDINDEXMERGERTHREAD_H