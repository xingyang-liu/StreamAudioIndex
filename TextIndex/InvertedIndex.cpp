#include "InvertedIndex.h"

double InvertedIndex::computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
                                   const vector<string> &query, const vector<double> &idf_vec)
{
    double fre = (pow(2, (time - 1.5008*pow(10,9))/864000+0.02)-0.99)*10;
    if(fre>=1)
    {
        fre=1;
    }else if(fre<0)
    {
        fre=0;
    }


//    cout<<(time - 1.5008*pow(10,9))/864000+0.02<<endl;
    double sig = log(score / 10000 + 1);
    if(sig>1)
    {
        sig=1;
    }else
    {
        sig = sqrt(sig);
    }

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
    if(sim+0.2>1)
    {
        sim=1;
    }else
    {
        sim=sqrt(sim+0.2);
    }
    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
    return ScoreAll;
}


double InvertedIndex::computeScore(const double &time, const double &score, map<string, double> &TermFreq, const int &tagsSum,
                                   const vector<string> &query, const vector<double> &idf_vec,map<int,score_ratio> &sco_vec,int id)
{
    double fre = (pow(2, (time - 1.5008*pow(10,9))/864000+0.02)-0.99)*10;
    if(fre>=1)
    {
        fre=1;
    }else if(fre<0)
    {
        fre=0;
    }



//    cout<<(time - 1.5008*pow(10,9))/864000+0.02<<endl;
    double sig = log(score / 100000 + 1);
    if(sig>1)
    {
        sig=1;
    }else
    {
        sig = sqrt(sig);
    }

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
    if(sim>1)
    {
        sim=1;
    }else
    {
        sim=sqrt(sim);
    }
//    score_ratio *tmp=new score_ratio(fre,sig,sim);
    sco_vec[id]=score_ratio(fre,sig,sim);
    double ScoreAll = (fre*weight_fre + sim  * weight_sim + sig*weight_sig);
    return ScoreAll;
}