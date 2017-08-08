//
// Created by billy on 17-8-5.
//

#ifndef HASH_0E_INVERTEDINDEX0SORTTHREAD_H
#define HASH_0E_INVERTEDINDEX0SORTTHREAD_H

#include "../TemplateIndex/IndexTemplate.h"

template <class T>
class FamilyI0Sort;

template <class T>
class InvertedIndex0SortThread {
public:
    explicit InvertedIndex0SortThread(void* fam){
        auto * ones = (FamilyI0Sort<T>*) fam;
        list = ones->list;
        myself = ones->me;
    }

    void excecuteSort() {
        // 将max_fresh指针指向新的最大节点
        set_next(list, mergeSortList(get_next(list)));
    }

    NodeInfo* mergeSortList(NodeInfo* head);

    NodeInfo* merge(NodeInfo* head1, NodeInfo* head2);

protected:
    ProgramList* list;
    IndexTemplate<T>* myself;

    virtual double get_value(NodeInfo* n)=0;
    virtual NodeInfo* get_next(ProgramList* p)=0; //获得p的max_fre
    virtual NodeInfo* get_next(NodeInfo* n)=0; //获得p的next_fre
    virtual NodeInfo* set_next(ProgramList* p, NodeInfo* des)=0; //将p的max_fre设为des,返回des
    virtual NodeInfo* set_next(NodeInfo* n, NodeInfo* des)=0; //将p的next_fre设为des，返回des
};

template <class T>
NodeInfo* InvertedIndex0SortThread<T>::mergeSortList(NodeInfo *head) {
    if (head == NULL || get_next(head) == NULL) {
        return head;
    }
    // 快慢指针找到中间节点
    NodeInfo* fast = head;
    NodeInfo* slow = head;
    while (get_next(fast) != NULL && get_next(get_next(fast)) != NULL) {
        fast = get_next(get_next(fast));
        slow = get_next(slow);
    }
    fast = get_next(slow);
    set_next(slow, NULL);
    slow = mergeSortList(head);// 前半段排序
    fast = mergeSortList(fast);// 后半段排序
    return merge(slow, fast);
}

template <class T>
NodeInfo* InvertedIndex0SortThread<T>::merge(NodeInfo *head1, NodeInfo *head2) {
    if (head1 == NULL) return head2;
    if (head2 == NULL) return head1;
    NodeInfo *head, *tmp;
    // 设置头节点
    if (get_value(head1) > get_value(head2)) {
        head = head1;
        head1 = get_next(head1);
    } else {
        head = head2;
        head2 = get_next(head2);
    }
    tmp = head;
    // 归并排序
    while (head1 != NULL && head2 != NULL) {
        if (get_value(head1) > get_value(head2)) {
            set_next(tmp, head1);
            head1 = get_next(head1);
        } else {
            set_next(tmp, head2);
            head2 = get_next(head2);
        }
        tmp = get_next(tmp);
    }
    if (head1 != NULL) set_next(tmp, head1);
    else if (head2 != NULL) set_next(tmp, head2);
    return head;
}


#endif //HASH_0E_INVERTEDINDEX0SORTTHREAD_H
