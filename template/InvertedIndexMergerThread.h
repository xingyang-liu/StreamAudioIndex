//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_INVERTEDINDEXMERGERTHREAD_H
#define HASH_0E_INVERTEDINDEXMERGERTHREAD_H

#include "../src/NodeInfo.h"
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

    map<T,ProgramList*> &tmp_pro=*myself->TermIndex;
    map<T,ProgramList*> &tmp1_pro=*other->TermIndex;

    for(it_list_j=other->TermIndex->begin();it_list_j!=other->TermIndex->end();it_list_j++)
    {
        it_list_i=myself->TermIndex->find(it_list_j->first);

//        判断pointer_j是不是由于flag可能需要清空
        pointer_j = get_next(it_list_j->second);
        if (pointer_j->flag != -1) {
            while (pointer_j != NULL && pointer_j->flag >= 0) {
                pointer_j->flag += 1;
                del_tmp = get_next(pointer_j);
                if (pointer_j->flag == 3) {
//                    it_list_j->second->nodeMap->erase(pointer_j->id);
                    delete pointer_j;
                }
                pointer_j = del_tmp;
            }
        }

        if(get_next(it_list_i->second)==pointer_j)//判断InvertedIndex中是否原来有项，如果没有，则把Ij中的所有program按id复制过来（即不考虑复制指针）
        {
            if(pointer_j!=NULL) {
                tmp_node=set_next((*myself->TermIndex)[it_list_j->first], pointer_j);
                pointer_j = get_next(pointer_j);

                while(pointer_j!=NULL)
                {
                    set_next(tmp_node, pointer_j);
                    tmp_node= pointer_j;
                    pointer_j = get_next(pointer_j);
                    while (pointer_j != NULL && pointer_j->flag >= 0) {
                        pointer_j->flag += 1;
                        del_tmp = get_next(pointer_j);
                        if (pointer_j->flag == 3) {
                            delete pointer_j;
                        }
                        pointer_j = del_tmp;
                    }
                }
                set_next(tmp_node, NULL);
            }
            else
            {
                set_next((*myself->TermIndex)[it_list_j->first],NULL);
            }
        } else{//说明myself中可能有，但是没有考虑flag
            if(pointer_j!=NULL) {//如果二者都有
                //当InvertedIndex中有项时，考虑Ij是不是有项，如果没有就根据flag理一下InvertedIndex的顺序，
                // 有的话就先看一下I是不是由于flag需要清空，不要就正式开始归并，要的话就还是判断pointer_i是不是由于flag可能也需要清空
                pointer_i = get_next(it_list_i->second);
                if (pointer_i->flag != -1) {
                    while (pointer_i != NULL && pointer_i->flag >= 0) {
                        pointer_i->flag += 1;
                        del_tmp = get_next(pointer_i);
                        if (pointer_i->flag == 3) {
                            it_list_i->second->nodeMap->erase(pointer_i->id);
                            delete pointer_i;
                        }
                        pointer_i = del_tmp;
                    }
                }

                if (pointer_i == NULL) {
                    tmp_node = set_next((*myself->TermIndex)[it_list_j->first], pointer_j);
                    pointer_j = get_next(pointer_j);

                    while (pointer_j != NULL) {
                        set_next(tmp_node, pointer_j);
                        tmp_node = pointer_j;
                        pointer_j = get_next(pointer_j);
                        while (pointer_j != NULL && pointer_j->flag >= 0) {
                            pointer_j->flag += 1;
                            del_tmp = get_next(pointer_j);
                            if (pointer_j->flag == 3) {
                                delete pointer_j;
                            }
                            pointer_j = del_tmp;
                        }
                    }
                    set_next(tmp_node, NULL);
                } else //两个list都有项，开始merge
                {
                    if (get_value(pointer_i) > get_value(pointer_j)) {
                        set_next((*myself->TermIndex)[it_list_j->first], pointer_i);
                        pointer_i = get_next(pointer_i);
                        while (pointer_i != NULL && pointer_i->flag >= 0) {
                            pointer_i->flag += 1;
                            del_tmp = get_next(pointer_i);
                            if (pointer_i->flag == 3) {
                                it_list_i->second->nodeMap->erase(pointer_i->id);
                                delete pointer_i;
                            }
                            pointer_i = del_tmp;
                        }
                    } else {
                        set_next((*myself->TermIndex)[it_list_j->first], pointer_j);
                        pointer_j = get_next(pointer_j);
                        while (pointer_j != NULL && pointer_j->flag >= 0) {
                            pointer_j->flag += 1;
                            del_tmp = get_next(pointer_j);
                            if (pointer_j->flag == 3) {
                                delete pointer_j;
                            }
                            pointer_j = del_tmp;
                        }
                    }

                    tmp_node = get_next((*myself->TermIndex)[it_list_j->first]);

                    while (pointer_i != NULL && pointer_j != NULL) {
                        if (get_value(pointer_i) > get_value(pointer_j)) {
                            tmp_node = set_next(tmp_node, pointer_i);
                            pointer_i = get_next(pointer_i);
                            while (pointer_i != NULL && pointer_i->flag >= 0) {
                                pointer_i->flag += 1;
                                del_tmp = get_next(pointer_i);
                                if (pointer_i->flag == 3) {
                                    it_list_i->second->nodeMap->erase(pointer_i->id);
                                    delete pointer_i;
                                }
                                pointer_i = del_tmp;
                            }
                        } else {
                            tmp_node = set_next(tmp_node, pointer_j);
                            pointer_j = get_next(pointer_j);
                            while (pointer_j != NULL && pointer_j->flag >= 0) {
                                pointer_j->flag += 1;
                                del_tmp = get_next(pointer_j);
                                if (pointer_j->flag == 3) {
                                    delete pointer_j;
                                }
                                pointer_j = del_tmp;
                            }
                        }
                    }

                    while (pointer_i != NULL) {
                        tmp_node = set_next(tmp_node, pointer_i);
                        pointer_i = get_next(pointer_i);
                        while (pointer_i != NULL && pointer_i->flag >= 0) {
                            pointer_i->flag += 1;
                            del_tmp = get_next(pointer_i);
                            if (pointer_i->flag == 3) {
                                it_list_i->second->nodeMap->erase(pointer_i->id);
                                delete pointer_i;
                            }
                            pointer_i = del_tmp;
                        }
                    }

                    while (pointer_j != NULL) {
                        tmp_node = set_next(tmp_node, pointer_j);
                        pointer_j = get_next(pointer_j);
                        while (pointer_j != NULL && pointer_j->flag >= 0) {
                            pointer_j->flag += 1;
                            del_tmp = get_next(pointer_j);
                            if (pointer_j->flag == 3) {
                                delete pointer_j;
                            }
                            pointer_j = del_tmp;
                        }
                    }
                    set_next(tmp_node, NULL);
                }
            } else {//只有myself里面有,那就简单去掉就好

                pointer_i = get_next(it_list_i->second);
                if (pointer_i->flag != -1) {
                    pointer_i = get_next(it_list_i->second);
                    while (pointer_i != NULL && pointer_i->flag >= 0) {
                        pointer_i->flag += 1;
                        del_tmp = get_next(pointer_i);
                        if (pointer_i->flag == 3) {
                            it_list_i->second->nodeMap->erase(pointer_i->id);
                            delete pointer_i;
                        }
                        pointer_i = del_tmp;
                    }
                }
                if (pointer_i == NULL) {
                    set_next((*myself->TermIndex)[it_list_j->first], NULL);
                } else {
                    tmp_node = set_next(it_list_i->second, pointer_i);
                    pointer_i = get_next(pointer_i);

                    while (pointer_i != NULL && pointer_i->flag >= 0) {
                        pointer_i->flag += 1;
                        del_tmp = get_next(pointer_i);
                        if (pointer_i->flag == 3) {
                            it_list_i->second->nodeMap->erase(pointer_i->id);
                            delete pointer_i;
                        }
                        pointer_i = del_tmp;
                    }
                    while (pointer_i != NULL) {
                        tmp_node = set_next(tmp_node, pointer_i);
                        pointer_i = get_next(pointer_i);
                        while (pointer_i != NULL && pointer_i->flag >= 0) {
                            pointer_i->flag += 1;
                            del_tmp = get_next(pointer_i);
                            if (pointer_i->flag == 3) {
                                it_list_i->second->nodeMap->erase(pointer_i->id);
                                delete pointer_i;
                            }
                            pointer_i = del_tmp;
                        }
                    }
                    set_next(tmp_node, NULL);
                }
            }
        }
    }
}


#endif //HASH_0E_INVERTEDINDEXMERGERTHREAD_H
