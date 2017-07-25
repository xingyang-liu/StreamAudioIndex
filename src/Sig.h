//
// Created by watson on 23/07/17.
//

#ifndef HASH_0E_SIG_H
#define HASH_0E_SIG_H



class Sig
{
public:
    int id;
    double score;
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


#endif //HASH_0E_SIG_H
