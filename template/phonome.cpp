//
// Created by billy on 17-7-18.
//

#include "phonome.h"

Phonome::Phonome(Phonome &other) {
    for (int i = 0; i < 13;++i) {
        data[i] = other.data[i];
    }
}

Phonome& Phonome::operator=(Phonome &other) {
    for (int i = 0; i < 13;++i) {
        data[i] = other.data[i];
    }
}

