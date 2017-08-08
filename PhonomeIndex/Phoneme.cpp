//
// Created by billy on 17-7-18.
//

#include "Phoneme.h"


Phoneme::Phoneme(const Phoneme &other) {
    data = other.data;
}

Phoneme& Phoneme::operator=(const Phoneme &other) {
    data = other.data;
}

//float Phoneme::find_minimum(int &choice, float n_, float n0, float n1) {
//    if (n_ < n0 && n_ < n1) {
//        choice = -1;
//        return n_;
//    }
//    else if (n0 < n1) {
//        choice = 0;
//        return n0;
//    }
//    else {
//        choice = 1;
//        return n1;
//    }
//}

void Phoneme::output(char* buf) const {
    float* tmp = (float*) buf;
    for (int i = 0; i < 13; ++i) {
        tmp[i] = data[i];
    }
}

void Phoneme::output() const{
    for (int i = 0; i < 13; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;
}

bool Phoneme::operator==(const Phoneme &other) const
{
    for (int i = 0; i < 13; ++i) {
        if ((data[i]-other.data[i]) > 0.0001 || (data[i]-other.data[i]) < -0.0001) return false;
    }
//    cout << "cos: " << diff << ((int)diff >= THRESHDIFF) << endl;
    return true;
}

bool Phoneme::operator<(const Phoneme &other) const{
    for (int i = 0; i < 13; ++i) {
        if (data[i]-other.data[i] < -0.0001) return true;
        if (data[i]-other.data[i] > 0.0001) return false;
    }
    return false;
}

bool Phoneme::operator<=(const Phoneme &other) const{
    for (int i = 0; i < 13; ++i) {
        if (data[i]-other.data[i] < -0.0001) return true;
        if (data[i]-other.data[i] > 0.0001) return false;
    }
    return true;
}

bool Phoneme::operator>(const Phoneme &other) const{
    for (int i = 0; i < 13; ++i) {
        if (data[i]-other.data[i] < -0.0001) return false;
        if (data[i]-other.data[i] > 0.0001) return true;
    }
    return false;
}

bool Phoneme::operator>=(const Phoneme &other) const{
    for (int i = 0; i < 13; ++i) {
        if (data[i]-other.data[i] < -0.0001) return false;
        if (data[i]-other.data[i] > 0.0001) return true;
    }
    return true;
}