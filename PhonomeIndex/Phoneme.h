//
// Created by billy on 17-7-18.
//

#ifndef SAMPLE_CONNECTOR_PHONOME_H
#define SAMPLE_CONNECTOR_PHONOME_H

#include <iostream>
#include <vector>
#include <cmath>
#include "../src/SimpleDTW.h"

using namespace std;

class Phoneme {
private:
    vector<float> data;

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
    Phoneme() {}

    Phoneme(float* a) {
        vector<float> tmp(a, a+13);
        data = tmp;
    }

    Phoneme(vector<float> a) {
        data = a;
    }

    Phoneme(const Phoneme &);

    void output() const;

    void output(char* buf) const;

    Phoneme &operator=(const Phoneme &);

    bool operator==(const Phoneme &) const;
    bool operator<(const Phoneme &) const;
    bool operator>(const Phoneme &) const;
    bool operator<=(const Phoneme &) const;
    bool operator>=(const Phoneme &) const;
};


#endif //SAMPLE_CONNECTOR_PHONOME_H
