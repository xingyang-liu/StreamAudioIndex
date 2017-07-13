//
// Created by 兴阳 刘 on 2017/7/11.
//

#ifndef SAMPLE_CONNECTOR_UTILS_H
#define SAMPLE_CONNECTOR_UTILS_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <sys/time.h>
#include <sstream>
#include <cstring>
#include <limits>

using namespace std;

extern int IndexUnit;
extern int AudioSum;
extern int AnswerNum;
extern map<string, double> IdfTable;

string Itos(int num);

int char2int(const char *t);

string Dtos(double i);

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

double getTime();

struct CompDedcendVal {
    bool operator()(const pair<int, double> &a, const pair<int, double> &b)
    {
        return a.second > b.second;
    }
};

double computeScore(const double &time, const double &score, map<string, int> &tagsNum, const int &tagsSum,
                    const vector<string> &query);
#endif //SAMPLE_CONNECTOR_UTILS_H
