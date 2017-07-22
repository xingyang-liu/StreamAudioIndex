//
// Created by watson on 18/07/17.
//


#include "ProgramList.h"


ProgramList::ProgramList(const ProgramList&other)
{
    max_termFreq=other.max_termFreq;
    max_sig=other.max_sig;
    max_fresh=other.max_fresh;
    nodeMap=other.nodeMap;
}

NodeInfo* ProgramList::addNode(int tf,int id)
{
    mutex.Lock();
    NodeInfo * tmp;
    if(nodeMap==NULL)
    {
        nodeMap=new map<int,NodeInfo*>;
        tmp= new NodeInfo(tf,id);
        (*nodeMap)[id]=tmp;
    }
    else
    {
        map<int,NodeInfo*>::iterator it_node=nodeMap->find(id);
        if(it_node==nodeMap->end())
        {
            tmp= new NodeInfo(tf,id);
            (*nodeMap)[id]=tmp;
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

ProgramList &ProgramList::operator=(const ProgramList&other)
{
    if(this==&other)
    {
        return *this;
    }
    else
    {
        if(nodeMap!=NULL) delete nodeMap;
        nodeMap=new map<int,NodeInfo*>;
        max_termFreq=other.max_termFreq;
        max_sig=other.max_sig;
        max_fresh=other.max_fresh;
        nodeMap=other.nodeMap;
        return *this;
    }
}

NodeInfo* ProgramList::getNodePointer(int tf,int id) {
    NodeInfo *tmp_pointer;
    map<int, NodeInfo *>::iterator it_node=nodeMap->find(id);
    if(it_node==nodeMap->end())
    {
        tmp_pointer=addNode(tf,id);
    } else{
        tmp_pointer=it_node->second;
    }

    return it_node->second;

}

ProgramList::~ProgramList()
{
    if(nodeMap!=NULL)
    {
        map<int,NodeInfo*>::iterator it_node;
        for(it_node=nodeMap->begin();it_node!=nodeMap->end();it_node++)
        {
            delete it_node->second;
        }
        delete nodeMap;
    }
}