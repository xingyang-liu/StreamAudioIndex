#include "InvertedIndex.h"

//double InvertedIndex::computeScore(const double &time, const double &score, btree::btree_map<int, double> &TermFreq, const int &tagsSum,
//                                   const vector<int> &query, const vector<double> &idf_vec)
//{
//    double fre = (pow(2, (time - getTime())));
//    if(fre>=1)
//    {
//        fre=1;
//    }else if(fre<0)
//    {
//        fre=0;
//    }
//
//    double sig = log(score / 1000000 + 1);
//    if(sig>1)
//    {
//        sig=1;
//    }else if(sig>0)
//    {
//        sig = sqrt(sig);
//    }else
//    {
//        sig=0;
//    }
//
//    double sim = 0;
//    for (int i = 0; i < query.size(); i++)
//    {
//        typename btree::btree_map<int, double>::iterator it = TermFreq.find(query[i]);
//        if (it != TermFreq.end())
//        {
//            if (tagsSum != 0)
//            {
//                sim += TermFreq[query[i]] * idf_vec[i];
//            }
//            else
//            {
//                sim += TermFreq[query[i]] * idf_vec[i];
//            }
//        }
//    }
//    if(sim>1)
//    {
//        sim=1;
//    }else
//    {
//        sim=sqrt(sim);
//    }
//    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
//    return ScoreAll;
//}
//
//
//double InvertedIndex::computeScore(const double &time, const double &score, btree::btree_map<int, double> &TermFreq, const int &tagsSum,
//                                   const vector<int> &query, const vector<double> &idf_vec,btree::btree_map<int,score_ratio> &sco_vec,int id)
//{
//    double fre = (pow(2, (time - getTime())));
//    if(fre>=1)
//    {
//        fre=1;
//    }else if(fre<0)
//    {
//        fre=0;
//    }
//
//    double sig = log(score / 1000000 + 1);
//    if(sig>1)
//    {
//        sig=1;
//    }else if(sig>0)
//    {
//        sig = sqrt(sig);
//    }else
//    {
//        sig=0;
//    }
//
//    double sim = 0;
//    for (int i = 0; i < query.size(); i++)
//    {
//        typename btree::btree_map<int, double>::iterator it = TermFreq.find(query[i]);
//        if (it != TermFreq.end())
//        {
//            if (tagsSum != 0)
//            {
//                sim += TermFreq[query[i]] * idf_vec[i];
//            }
//            else
//            {
//                sim += TermFreq[query[i]] * idf_vec[i];
//            }
//        }
//    }
//    if(sim>1)
//    {
//        sim=1;
//    }else
//    {
//        sim=sqrt(sim);
//    }
//    sco_vec[id]=score_ratio(fre,sig,sim);
//    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
//    return ScoreAll;
//}