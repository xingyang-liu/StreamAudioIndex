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

bool Phonome::operator==(const Phonome &other) const{
    DTW::SimpleDTW fastdtw = DTW::SimpleDTW(data.size(), other.data.size(), euclidean_distance);
    double diff = fastdtw.EvaluateWarpingCost(data, other.data);
    return (int)diff <= THRESHDIFF;
}

bool Phonome::operator<(const Phonome &other) const{
    if (*this == other) return false;
    vector<vector<double>> benchm;
    for (int i = 0; i < data.size(); ++i) {
        vector<double> tmp;
        for (int j = 0; j < data[i].size(); ++j) {
            tmp.push_back(0.0);
        }
        benchm.push_back(tmp);
    }
    DTW::SimpleDTW fastdtw1 = DTW::SimpleDTW(data.size(), benchm.size(), euclidean_distance);
    double dif1 = fastdtw1.EvaluateWarpingCost(data, benchm);
    DTW::SimpleDTW fastdtw2 = DTW::SimpleDTW(other.data.size(), benchm.size(), euclidean_distance);
    double dif2 = fastdtw2.EvaluateWarpingCost(other.data, benchm);
    return dif1 < dif2;
}

bool Phonome::operator<=(const Phonome &other) const{
    if (*this == other) return true;
    vector<vector<double>> benchm;
    for (int i = 0; i < data.size(); ++i) {
        vector<double> tmp;
        for (int j = 0; j < data[i].size(); ++j) {
            tmp.push_back(0.0);
        }
        benchm.push_back(tmp);
    }
    DTW::SimpleDTW fastdtw1 = DTW::SimpleDTW(data.size(), benchm.size(), euclidean_distance);
    double dif1 = fastdtw1.EvaluateWarpingCost(data, benchm);
    DTW::SimpleDTW fastdtw2 = DTW::SimpleDTW(other.data.size(), benchm.size(), euclidean_distance);
    double dif2 = fastdtw2.EvaluateWarpingCost(other.data, benchm);
    return dif1 < dif2;
}

bool Phonome::operator>(const Phonome &other) const{
    if (*this == other) return false;
    vector<vector<double>> benchm;
    for (int i = 0; i < data.size(); ++i) {
        vector<double> tmp;
        for (int j = 0; j < data[i].size(); ++j) {
            tmp.push_back(0.0);
        }
        benchm.push_back(tmp);
    }
    DTW::SimpleDTW fastdtw1 = DTW::SimpleDTW(data.size(), benchm.size(), euclidean_distance);
    double dif1 = fastdtw1.EvaluateWarpingCost(data, benchm);
    DTW::SimpleDTW fastdtw2 = DTW::SimpleDTW(other.data.size(), benchm.size(), euclidean_distance);
    double dif2 = fastdtw2.EvaluateWarpingCost(other.data, benchm);
    return dif1 > dif2;
}

bool Phonome::operator>=(const Phonome &other) const{
    if (*this == other) return true;
    vector<vector<double>> benchm;
    for (int i = 0; i < data.size(); ++i) {
        vector<double> tmp;
        for (int j = 0; j < data[i].size(); ++j) {
            tmp.push_back(0.0);
        }
        benchm.push_back(tmp);
    }
    DTW::SimpleDTW fastdtw1 = DTW::SimpleDTW(data.size(), benchm.size(), euclidean_distance);
    double dif1 = fastdtw1.EvaluateWarpingCost(data, benchm);
    DTW::SimpleDTW fastdtw2 = DTW::SimpleDTW(other.data.size(), benchm.size(), euclidean_distance);
    double dif2 = fastdtw2.EvaluateWarpingCost(other.data, benchm);
    return dif1 > dif2;
}