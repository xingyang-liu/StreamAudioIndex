

#ifndef HASH_0E_InvertedIndex_H
#define HASH_0E_InvertedIndex_H


#include "ProgramList.h"
#include "../template/IndexTemplate.h"


class InvertedIndex: public IndexTemplate<string> {
public:
	InvertedIndex() : IndexTemplate<string>() {}

	InvertedIndex(int l) : IndexTemplate<string>(l) {}

	InvertedIndex(const InvertedIndex& other) : IndexTemplate<string>(other) {}
};


#endif //HASH_0E_InvertedIndex_H
