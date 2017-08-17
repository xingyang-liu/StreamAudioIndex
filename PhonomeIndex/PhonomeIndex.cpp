//
// Created by billy on 17-7-30.
//

#include "PhonomeIndex.h"

double PhonomeIndex::computeScore(const double &time, const double &score, map<Phoneme, double> &TermFreq, const int &tagsSum,
                            const vector<Phoneme> &query)
    {
        double fre = (pow(2, (time - 1.5008*pow(10,9))/864000+0.02)-0.99)*10;
        if(fre>=1)
        {
            fre=1;
        }else if(fre<0)
        {
            fre=0;
        }
        double sig = log(score / 100000 + 1);
        if(sig>1)
        {
            sig=1;
        }else
        {
            sig = sqrt(sig);
        }
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
        if(sim>1)
        {
            sim=1;
        }else
        {
            sim=sqrt(sim);
        }
        double ScoreAll = (fre*0.2 + sim  * 0.7 + sig*0.1);
        return ScoreAll;
    }