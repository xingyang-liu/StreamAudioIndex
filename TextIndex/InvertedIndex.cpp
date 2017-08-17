//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "InvertedIndex.h"

double InvertedIndex::computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
                            const vector<string> &query)
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
                        sim += TermFreq[i] * IdfTableText[i];
                    }
                    else
                    {
                        sim += TermFreq[i] * IdfTableText[i];
                    }
                }
                catch (...)
                {
                    auto iterr = IdfTableText.find(i);
                    if (iterr != IdfTableText.end()) continue;
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
