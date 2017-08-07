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
    IndexAudioSumUnit = 20000;
    AudioSum = 1000;
    IndexTermSumUnit=200*300;//估摸一个audio的term在300上下(200,400,600,800,1000,1200,1400,1600,1800,2000,2200,2400,2500,2600,2800,3000)
//    PhoIndexManager index(AudioSum);


    IndexManager index(AudioSum);


    ofstream writefile("BuildAnalysis.txt",ofstream::app);
    cout<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<DuplicateTime<<"\tMergeTime: "<<\
    MergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<MergeTimes<<endl;
    writefile<<"AddAudioTime: "<<AddAduioTime<<"\tI0SortTime: "<<I0SortTime<<"\tDuplicateTime: "<<DuplicateTime<<"\tMergeTime: "<<\
    MergeTime<<"\tMergeSortTime: "<<MergeSortTime<<"\tMergeTimes: "<<MergeTimes<<endl;
    writefile.close();


//    string path = "/media/billy/Braavos/Billy/phonome/";
//    initialInfo(path);
//    test_for_QandA(index,500,1,7000,1);

//    FamilyTestQueryAndUpdate fam(&index,100,2);
//    pthread_t pid;
//    pthread_create(&pid,NULL,test_for_queryThread,(void*)&fam);
//    pthread_join(pid,NULL);

//    FamilyTestQueryAndUpdate fam(&index,500,1);
//    pthread_t pid;
//    pthread_create(&pid,NULL,test_for_updateThread,(void*)&fam);
//    pthread_join(pid,NULL);


    AnswerNum=10;
    FamilyTestQueryAndUpdate fam(&index,5000,1);
    pthread_t pid;
    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
    pthread_join(pid,NULL);

    AnswerNum=20;
    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
    pthread_join(pid,NULL);

    AnswerNum=30;
    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
    pthread_join(pid,NULL);

    AnswerNum=40;
    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
    pthread_join(pid,NULL);

    AnswerNum=50;
    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
    pthread_join(pid,NULL);


//    int arr[7]={60000,120000,150000,240000,300000,600000,750000};
//    for (int i=0;i<7;i++)
//    {
//        IndexTermSumUnit=arr[i];
//        IndexManager index(AudioSum);
//
//        FamilyTestQueryAndUpdate fam(&index,5000,1);
//        pthread_t pid;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//
//        FamilyTestQueryAndUpdate fam3(&index,5000,1);
//        pthread_t pid3;
//        pthread_create(&pid3,NULL,test_for_updatetxtThread,(void*)&fam3);
//        pthread_join(pid,NULL);
//
//        pthread_t pida;
//        FamilyAdd fama(&index,20000,1);
//        pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
//        pthread_join(pida,NULL);
//
//    }


//    test_for_index(10,AudioSum);

//    pthread_t pida;
//    FamilyAdd fama(&index,300,1);
//    pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
//    pthread_join(pida,NULL);
    while (true) {
        cout<<"Please input: "<<endl;
//        int str_len;
//        {
//            char hex_str[9] = {0};
//            cin.read(hex_str, 8);
//            str_len = strtol(hex_str, nullptr, 16);
//        }
//        cout<<"Please input query: "<<endl;
        string query_str;
        cin >> query_str;
//        query_str.resize(str_len);
//        cin.read((char*) query_str.data(), str_len);
//        query_str[str_len] = 0;
//		cout << query_str << endl;
//         parse json string
//         https://github.com/nlohmann/json#implicit-conversions
//		auto in_obj = json::parse(query_str);
//		auto field1_int = in_obj["field1"].get<int>();
//		auto field3_str = in_obj["field3"];
//
//         create a json object
//         https://github.com/nlohmann/json#examples
        string str_back = index.handleQuery(query_str);
        cout<<str_back<<endl;
//        json out_obj = {
//                {"content", str_back},
//                {"link", 3},
//                {"other_field2", {1, 2, 3, 4}}
//        };
//
//        string out_str = out_obj.dump();
//
//        {
//            char hex_str[9] = {0};
//            sprintf(hex_str, "%08x", (int)out_str.size());
//            cout.write(hex_str, 8);
//        }
//
//        cout << out_str;
    }

    return 0;
}
