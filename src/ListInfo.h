//
// Created by watson on 18/07/17.
//

#ifndef HASH_0E_LISTINFO_H
#define HASH_0E_LISTINFO_H

#include "AudioInfo.h"


class NodeInfo
{
public:
    NodeInfo *next_fresh;
    NodeInfo *next_sig;
    NodeInfo *next_termFreq;
    int flag;//-1表示不是直播或者直播最后一个片段，不需要处理，0表示需要删除
    int tf;
    int id;

    NodeInfo();

    NodeInfo(int t,int i):tf(t),id(i){flag=-1;next_termFreq=NULL;next_fresh=NULL;next_sig=NULL;}

    NodeInfo(const NodeInfo&other);

    NodeInfo &operator=(const NodeInfo&other);

};



class ListInfo
{
public:
    NodeInfo *max_fresh;
    NodeInfo *max_sig;
    NodeInfo *max_termFreq;
    map<int,NodeInfo*> *nodeMap;
    CMutex mutex;


    ListInfo()
    {
        max_termFreq=NULL;
        max_fresh=NULL;
        max_sig=NULL;
        nodeMap=NULL;
    }

    NodeInfo* add_id(int tf,int id)
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
                nodeMap->erase(it_node);
                tmp= new NodeInfo(tf,id);//外面给进来的值必须是已经更新过的
                (*nodeMap)[id]=tmp;
            }
        }
        mutex.Unlock();
        return tmp;

    }

    ListInfo(const ListInfo&other)
    {
        max_termFreq=other.max_termFreq;
        max_sig=other.max_sig;
        max_fresh=other.max_fresh;
        nodeMap=other.nodeMap;
    }

    ListInfo &operator=(const ListInfo&other)
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

    NodeInfo* id2pointer(int tf,int id) {
        NodeInfo *tmp_pointer;
        map<int, NodeInfo *>::iterator it_node=nodeMap->find(id);
        if(it_node==nodeMap->end())
        {
            tmp_pointer=add_id(tf,id);
        } else{
            tmp_pointer=it_node->second;
        }

        return it_node->second;

    }

};

#endif //HASH_0E_LISTINFO_H
