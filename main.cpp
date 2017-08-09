#include <iostream>

#include "src/index.h"
#include "PhonomeIndex/PhoIndexManager.h"
#include "json.hpp"

using json = nlohmann::json;

// use stdio to pass json only.
/*
 * input: "handleQuery query string"
 *
 */

int main() {
    using namespace std;
    AnswerNum=40;
    IndexAudioSumUnit = 50000;
    AudioSum = 2000;
    IndexTermSumUnit=400*1000;//估摸一个audio的term在300上下(200,400,600,800,1000,1200,1400,1600,1800,2000,2200,2400,2500,2600,2800,3000)
//    PhoIndexManager index(AudioSum);



    IndexManager index(AudioSum);



    ofstream writefile2("MergeTimeAnalysis.txt",ofstream::app);
    writefile2<<"Sum: "<<AudioSum<<"\tIndexTermUnit: "<<IndexTermSumUnit <<endl;
    for (multimap<int,double>::iterator it=time_of_index_merge.begin();it!=time_of_index_merge.end();it++)
    {
        writefile2<<it->first<<"\t"<<it->second<<endl;
    }
    writefile2.close();
    ofstream writefile("BuildAnalysis.txt",ofstream::app);
    cout<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<DuplicateTime<<"\tMergeTime: "<<\
    MergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<MergeTimes<<endl;
    writefile<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<DuplicateTime<<"\tMergeTime: "<<\
    MergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<MergeTimes<<endl;
    writefile.close();


//    string path = "/media/billy/Braavos/Billy/phonome/";
//    initialInfo(path);
    test_for_QandA(index,1000,1,1000,1);

    cout<<"Num of Live:"<<live_an<<endl;


//    int arr[7]={60000,120000,150000,240000,300000,600000,750000};
//    for (int i=0;i<7;i++)
//    {
//        IndexTermSumUnit=arr[i];
//        IndexManager index(AudioSum);
//

    //测量在default下topk变化造成的影响
//    AnswerNum=10;
//    FamilyTestQueryAndUpdate fam(&index,5000,1);
//    pthread_t pid;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    AnswerNum=20;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    AnswerNum=30;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    AnswerNum=40;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    AnswerNum=50;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    //测量在default下weight变化造成的影响
//    AnswerNum=40;
//    for(weight_fre=0.2;weight_fre<1;weight_fre+=0.2)
//    {
//        weight_sim=(1-weight_fre)/2;
//        weight_sig=weight_sim;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//    }
//
//    for(weight_sig=0.2;weight_sig<1;weight_sig+=0.2)
//    {
//        weight_sim=(1-weight_sig)/2;
//        weight_fre=weight_sim;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//    }
//
//    for(weight_sim=0.2;weight_sim<1;weight_sim+=0.2)
//    {
//        weight_sig=(1-weight_sim)/2;
//        weight_fre=weight_sig;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//    }


//
//        FamilyTestQueryAndUpdate fam3(&index,10,1);
//        pthread_t pid3;
//        pthread_create(&pid3,NULL,test_for_updatetxtThread,(void*)&fam3);
//        pthread_join(pid,NULL);
////
//        pthread_t pida;
//        FamilyAdd fama(&index,10,1);
//        pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
//        pthread_join(pida,NULL);
//
//    }


//    test_for_index(10,AudioSum);

//    pthread_t pida;
//    FamilyAdd fama(&index,300,1);
//    pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
//    pthread_join(pida,NULL);
//    while (true) {
//        cout<<"Please input: "<<endl;
////        int str_len;
////        {
////            char hex_str[9] = {0};
////            cin.read(hex_str, 8);
////            str_len = strtol(hex_str, nullptr, 16);
////        }
////        cout<<"Please input query: "<<endl;
//        string query_str;
//        cin >> query_str;
////        query_str.resize(str_len);
////        cin.read((char*) query_str.data(), str_len);
////        query_str[str_len] = 0;
////		cout << query_str << endl;
////         parse json string
////         https://github.com/nlohmann/json#implicit-conversions
////		auto in_obj = json::parse(query_str);
////		auto field1_int = in_obj["field1"].get<int>();
////		auto field3_str = in_obj["field3"];
////
////         create a json object
////         https://github.com/nlohmann/json#examples
//        string str_back = index.handleQuery(query_str);
//        cout<<str_back<<endl;
////        json out_obj = {
////                {"content", str_back},
////                {"link", 3},
////                {"other_field2", {1, 2, 3, 4}}
////        };
////
////        string out_str = out_obj.dump();
////
////        {
////            char hex_str[9] = {0};
////            sprintf(hex_str, "%08x", (int)out_str.size());
////            cout.write(hex_str, 8);
////        }
////
////        cout << out_str;
//    }

    return 0;
}
