//
// Created by billy on 17-7-30.
//

#ifndef HASH_0E_PHONOMEINDEX_H
#define HASH_0E_PHONOMEINDEX_H

#include "../TemplateIndex/IndexTemplate.h"

class PhonomeIndex: public IndexTemplate<Phoneme> {
public:
    PhonomeIndex(): IndexTemplate<Phoneme>() {}

    explicit PhonomeIndex(int l): IndexTemplate<Phoneme>(l) {}

    PhonomeIndex(const PhonomeIndex &other) = default;

    double computeScore(const double &time, const double &score, map<Phoneme, double> &TermFreq, const int &tagsSum,
                        const vector<Phoneme> &query, const vector<double> &idf_vec,map<int,score_ratio> &sco_vec,int id)override;


};


#endif //HASH_0E_PHONOMEINDEX_H
