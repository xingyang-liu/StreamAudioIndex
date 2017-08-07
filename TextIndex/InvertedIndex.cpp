//
// Created by 兴阳 刘 on 2017/7/11.
//

#include "InvertedIndex.h"

double InvertedIndex::computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
                            const vector<string> &query, const vector<double> &idf_vec)
{
    double fre = pow(2, time - getTime());
    double sig = log(score / 10000 + 1);
    double sim = 0;
    for (int i = 0; i < query.size(); i++)
    {
        typename map<string, double>::iterator it = TermFreq.find(query[i]);
        if (it != TermFreq.end())
        {
            if (tagsSum != 0)
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
            else
            {
                sim += TermFreq[query[i]] * idf_vec[i];
            }
        }
    }
    double ScoreAll = (fre*weight_fre + sim * 20 * weight_sim + sig*weight_sig)*10;
    return ScoreAll;
}
