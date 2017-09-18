

#ifndef HASH_0E_InvertedIndex_H
#define HASH_0E_InvertedIndex_H


#include "../BasicStructure/ProgramList.h"
#include "../TemplateIndex/IndexTemplate.h"


class InvertedIndex: public IndexTemplate<int> {
public:
	InvertedIndex() : IndexTemplate<int>() {}

	InvertedIndex(int l) : IndexTemplate<int>(l) {}

	InvertedIndex(const InvertedIndex& other) : IndexTemplate<int>(other) {}

//	double computeScore(const double &time, const double &score, btree::btree_map<int, double> &TermFreq, const int &tagsSum,
//						const vector<int> &query,const vector<double> &idf_vec);
//	double computeScore(const double &time, const double &score, btree::btree_map<int, double> &TermFreq, const int &tagsSum,
//									   const vector<int> &query, const vector<double> &idf_vec,btree::btree_map<int,score_ratio> &sco_vec,int id);



};


#endif //HASH_0E_InvertedIndex_H
