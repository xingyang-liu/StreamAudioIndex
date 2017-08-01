//
// Created by billy on 17-7-30.
//

#ifndef HASH_0E_PHONOMEINDEX_H
#define HASH_0E_PHONOMEINDEX_H

#include "../TemplateIndex/IndexTemplate.h"

class PhonomeIndex: public IndexTemplate<Phonome> {
public:
    PhonomeIndex(): IndexTemplate<Phonome>() {}

    PhonomeIndex(int l): IndexTemplate<Phonome>(l) {}

    PhonomeIndex(const PhonomeIndex &other): IndexTemplate<Phonome>(other) {}

    double computeScore(const double &time, const double &score, map<Phonome, double> &TermFreq, const int &tagsSum,
                        const vector<Phonome> &query);

};


#endif //HASH_0E_PHONOMEINDEX_H
