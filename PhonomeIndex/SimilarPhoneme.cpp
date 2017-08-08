//
// Created by billy on 17-8-7.
//

#include "SimilarPhoneme.h"

SimilarPhoneme::SimilarPhoneme(const SimilarPhoneme &other) {
    data = other.data;
}

SimilarPhoneme& SimilarPhoneme::operator=(const SimilarPhoneme &other) {
    data = other.data;
}

//float SimilarPhoneme::find_minimum(int &choice, float n_, float n0, float n1) {
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

void SimilarPhoneme::output(char* buf) const {
    float* tmp = (float*) buf;
    for (int i = 0; i < 13; ++i) {
        tmp[i] = data[i];
    }
}

void SimilarPhoneme::output() const{
    for (int i = 0; i < 13; ++i) {
        cout << data[i] << " ";
    }
    cout << endl;
}

bool SimilarPhoneme::operator==(const SimilarPhoneme &other) const
{
    float diff = cosine(data, other.data);
//    cout << "cos: " << diff << ((int)diff >= THRESHDIFF) << endl;
    return diff >= THRESHDIFF;
}

bool SimilarPhoneme::operator<(const SimilarPhoneme &other) const{
    if (*this == other) return false;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 < dif2;
}

bool SimilarPhoneme::operator<=(const SimilarPhoneme &other) const{
    if (*this == other) return true;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 <= dif2;
}

bool SimilarPhoneme::operator>(const SimilarPhoneme &other) const{
    if (*this == other) return false;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 > dif2;
}

bool SimilarPhoneme::operator>=(const SimilarPhoneme &other) const{
    if (*this == other) return true;
    vector<float> standard_data = {1,0,0,0,0,0,0,0,0,0,0,0,0};
    float dif1 = cosine(standard_data, data);
    float dif2 = cosine(standard_data, other.data);
    return dif1 >= dif2;
}