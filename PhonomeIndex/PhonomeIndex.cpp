//
// Created by billy on 17-7-30.
//

#include "PhonomeIndex.h"

double PhonomeIndex::computeScore(const double &time, const double &score, map<Phoneme, double> &TermFreq, const int &tagsSum,
                            const vector<Phoneme> &query)
    {
        double fre = pow(2, time - getTime());
        double sig = log(score / 10000 + 1);
        double sim = 0;
        for (const auto &i : query) {
            auto it = TermFreq.find(i);
            if (it != TermFreq.end())
            {
                try
                {
                    if (tagsSum != 0)
                    {
                        sim += TermFreq[i] * IdfTablePho[SimilarPhoneme(i)];
                    }
                    else
                    {
                        sim += TermFreq[i] * IdfTablePho[SimilarPhoneme(i)];
                    }
                }
                catch (...)
                {
                    auto iterr = IdfTablePho.find(SimilarPhoneme(i));
                    if (iterr != IdfTablePho.end()) continue;
                    cout << "Something wrong with computeScore()";
                }
            }
        }
        double ScoreAll = (fre*0.1 + sim * 20 * 0.6 + sig*0.2)*10;
        return ScoreAll;
    }