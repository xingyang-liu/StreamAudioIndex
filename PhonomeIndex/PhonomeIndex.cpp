//
// Created by billy on 17-7-30.
//

#include "PhonomeIndex.h"

//double PhonomeIndex::computeScore(const double &time, const double &score, map<Phonome, double> &TermFreq, const int &tagsSum,
//                            const vector<Phonome> &query)
//    {
//        double fre = pow(2, time - getTime());
//        double sig = log(score / 10000 + 1);
//        double sim = 0;
//        for (int i = 0; i < query.size(); i++)
//        {
//            typename map<Phonome, double>::iterator it = TermFreq.find(query[i]);
//            if (it != TermFreq.end())
//            {
//                try
//                {
//                    if (tagsSum != 0)
//                    {
//                        sim += TermFreq[query[i]];
//                    }
//                    else
//                    {
//                        sim += TermFreq[query[i]];
//                    }
//                }
//                catch (...)
//                {
//                    continue;
//                }
//            }
//        }
//        double ScoreAll = (fre*0.1 + sim * 20 * 0.6 + sig*0.2)*10;
//        return ScoreAll;
//    }