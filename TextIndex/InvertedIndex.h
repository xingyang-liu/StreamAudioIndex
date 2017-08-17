

#ifndef HASH_0E_InvertedIndex_H
#define HASH_0E_InvertedIndex_H


#include "../BasicStructure/ProgramList.h"
#include "../TemplateIndex/IndexTemplate.h"


class InvertedIndex: public IndexTemplate<string> {
public:
	InvertedIndex() : IndexTemplate<string>() {}

	InvertedIndex(int l) : IndexTemplate<string>(l) {}

	InvertedIndex(const InvertedIndex& other) : IndexTemplate<string>(other) {}

	double computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
						const vector<string> &query);

};


#endif //HASH_0E_InvertedIndex_H
