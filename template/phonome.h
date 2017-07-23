//
// Created by billy on 17-7-18.
//

#ifndef SAMPLE_CONNECTOR_PHONOME_H
#define SAMPLE_CONNECTOR_PHONOME_H

#include <iostream>
#include <vector>
#include <math.h>
#include "SimpleDTW.h"

using namespace std;

class Phonome {
private:
    vector<vector<double>> data;

    static const int THRESHDIFF = 500;

    static double euclidean_distance(vector<double> P1, vector<double> P2)
    {
        double total = 0.0;
        for (unsigned int i = 0; i < P1.size(); i++)
        {
            total = total + pow((P1[i] - P2[i]), 2);
        }
        return sqrt(total);
    }

//    float find_minimum(int &choice, float n_, float n0, float n1);
public:
    Phonome(vector<vector<double>> a) {
        data = a;
    }

    Phonome(const Phonome &);

    Phonome &operator=(const Phonome &);

    bool operator==(const Phonome &) const;
    bool operator<(const Phonome &) const;
    bool operator>(const Phonome &) const;
    bool operator<=(const Phonome &) const;
    bool operator>=(const Phonome &) const;
};


#endif //SAMPLE_CONNECTOR_PHONOME_H
