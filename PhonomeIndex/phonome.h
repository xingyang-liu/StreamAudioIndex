//
// Created by billy on 17-7-18.
//

#ifndef SAMPLE_CONNECTOR_PHONOME_H
#define SAMPLE_CONNECTOR_PHONOME_H

#include <iostream>
#include <vector>
#include <math.h>
#include "../template/SimpleDTW.h"

using namespace std;

class Phonome {
private:
    vector<float> data;

    static constexpr float THRESHDIFF = 0.98;

    static double euclidean_distance(vector<double> P1, vector<double> P2)
    {
        double total = 0.0;
        for (unsigned int i = 0; i < P1.size(); i++)
        {
            total = total + pow((P1[i] - P2[i]), 2);
        }
        return sqrt(total);
    }

    static float cosine(vector<float> P1, vector<float> P2)
    {
        float up = 0, abs_p1 = 0, abs_p2 = 0;
        for (int i = 0; i < 13; ++i) {
            up += P1[i] * P2[i];
            abs_p1 += P1[i] * P1[i];
            abs_p2 += P2[i] * P2[i];
        }
        return up / (sqrt(abs_p1) * sqrt(abs_p2));
    }

//    float find_minimum(int &choice, float n_, float n0, float n1);
public:
    Phonome() {}

    Phonome(float* a) {
        vector<float> tmp(a, a+12);
        data = tmp;
    }

    Phonome(vector<float> a) {
        data = a;
    }

    Phonome(const Phonome &);

    void output() const;

    void output(char* buf) const;

    Phonome &operator=(const Phonome &);

    bool operator==(const Phonome &) const;
    bool operator<(const Phonome &) const;
    bool operator>(const Phonome &) const;
    bool operator<=(const Phonome &) const;
    bool operator>=(const Phonome &) const;
};


#endif //SAMPLE_CONNECTOR_PHONOME_H
