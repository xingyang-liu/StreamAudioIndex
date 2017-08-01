//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "InvertedIndex.h"

double InvertedIndex::computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
                            const vector<string> &query)
    {
        double fre = pow(2, time - getTime());
        double sig = log(score / 10000 + 1);
        double sim = 0;
        for (int i = 0; i < query.size(); i++)
        {
            typename map<string, double>::iterator it = TermFreq.find(query[i]);
            if (it != TermFreq.end())
            {
                try
                {
                    if (tagsSum != 0)
                    {
                        sim += TermFreq[query[i]] * IdfTable[query[i]];
                    }
                    else
                    {
                        sim += TermFreq[query[i]] * IdfTable[query[i]];
                    }
                }
                catch (...)
                {
                    map<string, double>::iterator it = IdfTable.find(query[i]);
                    if (it != IdfTable.end())
                    {
                        continue;
                    }
                    else
                    {
                        cout << "Something wrong with computeScore()";
                    }

                }
            }
        }
        double ScoreAll = (fre*0.1 + sim * 20 * 0.6 + sig*0.2)*10;
        return ScoreAll;
    }
