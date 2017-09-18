//
// Created by watson on 18/07/17.
//


#include "ProgramList.h"


ProgramList::ProgramList(const ProgramList&other)
{
    max_termFreq=other.max_termFreq;
    max_sig=other.max_sig;
    max_fresh=other.max_fresh;
    nodeMap=new btree::btree_map<int,NodeInfo*> (*other.nodeMap);
//    nodeMap->set_empty_key(-666);
}

NodeInfo* ProgramList::addNode(double tf,int id)
{
    mutex.Lock();
    NodeInfo * tmp;
    nodeSum++;
    if(nodeMap==NULL)
    {
        nodeMap=new btree::btree_map<int,NodeInfo*> ;
        tmp= new NodeInfo(tf,id);
        (*nodeMap)[id]=tmp;
        max_termFreq = tmp;
        max_fresh = tmp;
        max_sig = tmp;
    }
    else
    {
        btree::btree_map<int,NodeInfo*>::iterator it_node=nodeMap->find(id);
        if(it_node==nodeMap->end())
        {
            tmp= new NodeInfo(tf,id);
            (*nodeMap)[id]=tmp;
            tmp->next_termFreq = max_termFreq;
            tmp->next_fresh = max_fresh;
            tmp->next_sig = max_sig;
            max_termFreq = tmp;
            max_fresh = tmp;
            max_sig = tmp;
        }
        else
        {
            tmp=it_node->second;
            tmp->tf=tf;
        }
    }
    mutex.Unlock();
    return tmp;
}

ProgramList* ProgramList::clone()
{
    ProgramList* newplist = new ProgramList();
    if (this->nodeMap == NULL) return newplist;
    newplist->nodeMap = new btree::btree_map<int,NodeInfo*> ;
    NodeInfo* pFreq = max_termFreq;
    while (pFreq)
    {
        NodeInfo* tmp = new NodeInfo(pFreq->tf, pFreq->id, pFreq->flag);
        tmp->next_termFreq = pFreq->next_termFreq;
        pFreq->next_termFreq = tmp;
        pFreq = tmp->next_termFreq;
        (*(newplist->nodeMap))[tmp->id] = tmp;
    }

    pFreq = max_termFreq;
    while (pFreq)
    {
        NodeInfo* tmp = pFreq->next_termFreq;
        if (pFreq->next_sig) tmp->next_sig = pFreq->next_sig->next_termFreq;
        if (pFreq->next_fresh) tmp->next_fresh = pFreq->next_fresh->next_termFreq;
        pFreq = tmp->next_termFreq;
    }

    pFreq = max_termFreq;
    NodeInfo* tmp;
    newplist->max_termFreq = max_termFreq->next_termFreq;
    newplist->max_fresh = max_fresh->next_termFreq;
    newplist->max_sig = max_sig->next_termFreq;
    while (pFreq->next_termFreq)
    {
        tmp = pFreq->next_termFreq;
        pFreq->next_termFreq = tmp->next_termFreq;
        pFreq = tmp;
    }

    return newplist;
}

void ProgramList::output() {
    NodeInfo* pNode = max_termFreq;
    cout << "termFreq: ";
    while (pNode) {
        cout << " (" << pNode->id << ", " << pNode->tf << ", " << pNode->flag << ") ";
        pNode = pNode->next_termFreq;
    }
    cout << endl;

    pNode = max_fresh;
    cout << "fresh: ";
    while (pNode) {
        cout << " (" << pNode->id << ", " << pNode->tf << ", " << pNode->flag << ") ";
        pNode = pNode->next_fresh;
    }
    cout << endl;

    pNode = max_sig;
    cout << "sig: ";
    while (pNode) {
        cout << " (" << pNode->id << ", " << pNode->tf << ", " << pNode->flag << ") ";
        pNode = pNode->next_sig;
    }
    cout << endl;
}

ProgramList &ProgramList::operator=(const ProgramList&other)
{
    if(this==&other)
    {
        return *this;
    }
    else
    {
        if(nodeMap!=NULL) delete nodeMap;
        nodeMap=new btree::btree_map<int,NodeInfo*> ;
        max_termFreq=other.max_termFreq;
        max_sig=other.max_sig;
        max_fresh=other.max_fresh;
        nodeMap=other.nodeMap;
        return *this;
    }
}


ProgramList::~ProgramList()
{
    if(nodeMap!=NULL)
    {
        for(auto it_node=nodeMap->begin();it_node!=nodeMap->end();it_node++)
        {
            delete it_node->second;
        }
    }
    delete nodeMap;
    nodeMap=NULL;
}