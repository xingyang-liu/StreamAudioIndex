//
// Created by watson on 22/07/17.
//

#ifndef HASH_0E_NODEINFO_H
#define HASH_0E_NODEINFO_H

#include "AudioInfo.h"

class NodeInfo
{
public:
    NodeInfo *next_fresh;
    NodeInfo *next_sig;
    NodeInfo *next_termFreq;
    int flag;//-1表示不是直播或者直播最后一个片段，不需要处理，0表示需要删除
    double tf;
    int id;

    NodeInfo();

<<<<<<< HEAD
    NodeInfo(double t,int i):tf(t),id(i){flag=-1;next_termFreq=NULL;next_fresh=NULL;next_sig=NULL;}
=======
    NodeInfo(double t, int i, int f):tf(t),id(i),flag(f){next_termFreq=NULL;next_fresh=NULL;next_sig=NULL;}

    NodeInfo(int t,int i):tf(t),id(i){flag=-1;next_termFreq=NULL;next_fresh=NULL;next_sig=NULL;}
>>>>>>> 357f3f719adc510be0005c82f3ca08835c4a213a

    NodeInfo(const NodeInfo&other);

    NodeInfo &operator=(const NodeInfo&other);

};

#endif //HASH_0E_NODEINFO_H
