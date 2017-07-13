//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_SIG_H
#define SAMPLE_CONNECTOR_SIG_H


class Sig
{
private:
    int id;
    double score;
public:
    Sig() {}

    Sig(int i, double s) :id(i), score(s) {}

    Sig(const Sig &other);

    Sig &operator=(const Sig &other);

    int get_id()const
    {
        return id;
    }

    double get_score() const
    {
        return score;
    }

    bool operator<(const Sig&other) const { return score > other.get_score(); }
};


#endif //SAMPLE_CONNECTOR_SIG_H
