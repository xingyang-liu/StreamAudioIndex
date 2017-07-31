//
// Created by billy on 17-7-18.
//

#include "phonome.h"


Phonome::Phonome(const Phonome &other) {
    data = other.data;
}

Phonome& Phonome::operator=(const Phonome &other) {
    data = other.data;
}

//float Phonome::find_minimum(int &choice, float n_, float n0, float n1) {
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

void Phonome::output(char* buf) const {
    float* tmp = (float*) buf;
    for (int i = 0; i < 13; ++i) {
        tmp[i] = data[i];
    }
}

void Phonome::output() const{
    for (int i = 0; i < 13; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;
}

bool Phonome::operator==(const Phonome &other) const
{
    float diff = cosine(data, other.data);
//    cout << "cos: " << diff << ((int)diff >= THRESHDIFF) << endl;
    return diff >= THRESHDIFF;
}

bool Phonome::operator<(const Phonome &other) const{
    if (*this == other) return false;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 < dif2;
}

bool Phonome::operator<=(const Phonome &other) const{
    if (*this == other) return true;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 <= dif2;
}

bool Phonome::operator>(const Phonome &other) const{
    if (*this == other) return false;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 > dif2;
}

bool Phonome::operator>=(const Phonome &other) const{
    if (*this == other) return true;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 >= dif2;
}