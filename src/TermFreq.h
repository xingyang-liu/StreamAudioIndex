//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_SIM_H
#define SAMPLE_CONNECTOR_SIM_H


class TermFreq
{
private:
    int id;
    double tf;
public:
    TermFreq() {}

    TermFreq(int i, int t) :id(i), tf(t) {}

    TermFreq(const TermFreq&other);

    TermFreq &operator=(const TermFreq &other);

    int get_id()const
    {
        return id;
    }

    int get_tf() const
    {
        return tf;
    }

    bool operator<(const TermFreq&other) const { return get_tf() > other.get_tf(); }
};



#endif //SAMPLE_CONNECTOR_SIM_H
