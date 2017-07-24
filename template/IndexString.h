//
// Created by billy on 17-7-24.
//

#ifndef HASH_0E_INDEXSTRING_H
#define HASH_0E_INDEXSTRING_H

#include "IndexTemplate.h"

class IndexString: public IndexTemplate<string> {
public:
    IndexString() : IndexTemplate<string>() {}

    IndexString(int l) : IndexTemplate<string>(l) {}

    IndexString(const IndexString& other) : IndexTemplate<string>(other) {}
};


#endif //HASH_0E_INDEXSTRING_H
