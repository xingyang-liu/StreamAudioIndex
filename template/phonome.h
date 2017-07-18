//
// Created by billy on 17-7-18.
//

#ifndef SAMPLE_CONNECTOR_PHONOME_H
#define SAMPLE_CONNECTOR_PHONOME_H

#include "dtw.h"

class Phonome {
private:
    float data[13];
public:
    Phonome(float* a) {
        for (int i = 0; i < 13;++i) {
            data[i] = a[i];
        }
    }

    Phonome(Phonome &);

    Phonome &operator=(Phonome &);

    bool operator==(Phonome &);
};


#endif //SAMPLE_CONNECTOR_PHONOME_H
