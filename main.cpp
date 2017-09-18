#include <iostream>

#include "utils/test.h"
#include "HybridIndex/HybridIndexManager.h"


int main() {
    using namespace std;
    answerNum=40;
    ratio =2;
    indexAudioSumUnit = 11000;
    audioSum =100;
    indexAudioSumUnit = 20;
    indexTermSumUnit=400*20000;//估摸一个audio的term在300上下(200,400,600,800,1000,1200,1400,1600,1800,2000,2200,2400,2500,2600,2800,3000)


//    IndexManager index(audioSum);
//    PhoIndexManager index(audioSum);
//    ifstream info_in("/home/watson/C++/git_c-/Phonome/text_src/info_release_1000.txt");
//    if (!info_in) exit(7);
//    string idTmp,titleTmp,scoreTmp,contentTmp,tagsTmp;
//    for (int i=0;i<audioSum;i++)
//    {
//
//        getline(info_in,idTmp);
//        getline(info_in,scoreTmp);
//        getline(info_in,titleTmp);
//        getline(info_in,tagsTmp);
//        getline(info_in,contentTmp);
//        index.addAudio(char2int(idTmp.c_str()),atof(scoreTmp.c_str()),titleTmp,tagsTmp,contentTmp);
//        cout<<"Add Audio: "<<idTmp<<endl;
//    }
//    index.output();


//    FamilyTestQueryAndUpdatePho fam(&index,50,2);
//    pthread_t pid;
//    pthread_create(&pid,NULL,test_for_queryPhoThread,(void*)&fam);
//    pthread_join(pid,NULL);


//    string path = "/media/billy/Braavos/Billy/phonome/";
//    initialInfo(path);
//    test_for_QandA(index,1000,1,1000,1);
//
//    cout<<"Num of Live:"<<live_an<<endl;


//    int arr[7]={60000,120000,150000,240000,300000,600000,750000};
//    for (int i=0;i<7;i++)
//    {
//        indexTermSumUnit=arr[i];
//        IndexManager index(audioSum);
//

    //测量在default下topk变化造成的影响
//    answerNum=10;
//    FamilyTestQueryAndUpdate fam(&index,4994,1);
//    pthread_t pid;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
////
//    answerNum=20;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    answerNum=30;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    answerNum=40;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
//    answerNum=50;
//    pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//    pthread_join(pid,NULL);
//
    //测量在default下weight变化造成的影响
//    answerNum=10;
//    for(weight_fre=0;weight_fre<=1;weight_fre+=0.1)
//    {
//        weight_sim=(1-weight_fre)/2;
//        weight_sig=weight_sim;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//    }
//
//    for(weight_sig=0;weight_sig<=1;weight_sig+=0.1)
//    {
//        weight_sim=(1-weight_sig)/2;
//        weight_fre=weight_sim;
//        pthread_create(&pid,NULL,test_for_querytxtThread,(void*)&fam);
//        pthread_join(pid,NULL);
//    }
//
//    for(weight_sim=0;weight_sim<=1;weight_sim+=0.1)
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


//    test_for_index(10,audioSum);

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
//        getline(cin,query_str);
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
////        ofstream score_file("score_ratio.txt");
////        for (int i=0;i<score_vec.size();i+=3)
////        {
////            score_file<<"weight_fre:\t"<<score_vec[i]<<"\tweight_sig:\t"<<score_vec[i+1]<<"\tweight_sim\t"<<score_vec[i+2]<<endl;
////        }
////        score_file.close();
////        score_vec.clear();
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
    HybridIndexManager index(audioSum);
    cout << "000This is end000" << endl;


    while (true) {
        long str_len;
        {
            char hex_str[9] = {0};
            cin.read(hex_str, 8);
            str_len = strtol(hex_str, nullptr, 16);
        }
        char pause[5] = "#!#";
        char* pcha = new char[str_len+1];
        cin.read(pcha, str_len);
        pcha[str_len] = '\0';
        ofstream file("test.txt");
        file << pcha << endl;
        file.close();
        string queryStr(pcha);

        char* tmp = new char[13*4];
        char* mark = new char[5];
        vector<SimilarPhoneme> queryPho;
        while (true) {
            cin.read(mark, 3);
//            cout << "Let's begin " << mark << endl;
            if (!strncmp(mark, pause, 3)) {
                break;
            }
            cin.read(tmp, 13*4);


            SimilarPhoneme pho = SimilarPhoneme((float*)tmp);
            queryPho.push_back(pho);
        }

        index.handleQuery(queryStr, queryPho);

        cout << "000This is end000" << endl;

        delete tmp;
        delete mark;
        delete pcha;
    }
    return 0;
}
