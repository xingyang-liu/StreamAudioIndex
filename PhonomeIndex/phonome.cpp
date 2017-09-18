//
// Created by billy on 17-7-18.
//

#include "phonome.h"


Phoneme::Phoneme(const Phoneme &other) {
    data = other.data;
}

Phoneme::Phoneme(const SimilarPhoneme &other) {
    float a[13];
    other.output((char*) a);
    vector<float> tmp(a, a+13);
    data = tmp;
}

Phoneme& Phoneme::operator=(const Phoneme &other) {
    data = other.data;
}

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


SimilarPhoneme::SimilarPhoneme(const SimilarPhoneme &other) {
    data = other.data;
}

SimilarPhoneme::SimilarPhoneme(const Phoneme &other) {
    float a[13];
    other.output((char*) a);
    vector<float> tmp(a, a+13);
    data = tmp;
}

SimilarPhoneme& SimilarPhoneme::operator=(const SimilarPhoneme &other) {
    data = other.data;
}

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

float SimilarPhoneme::distBetween(const SimilarPhoneme &other) const {
    float dist = cosine(this->data, other.data);
    return dist;
}