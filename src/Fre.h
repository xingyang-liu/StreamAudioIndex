//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_FRE_H
#define SAMPLE_CONNECTOR_FRE_H


class Fre {
public:
    int id;
    double fresh;

    Fre() {}

    Fre(int i, double s) : id(i), fresh(s) {}

    Fre(const Fre &other);

    Fre &operator=(const Fre& other);

    int get_id()const
    {
        return id;
    }

    double get_fresh() const
    {
        return fresh;
    }

    bool operator<(const Fre& other) const { return get_fresh() > other.get_fresh(); }//这样写的意义是最小的呢个排在最后一个
    // ，但是在队列中的话就就是在最上面

};


#endif //SAMPLE_CONNECTOR_FRE_H
