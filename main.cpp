#include <iostream>
#include <fcntl.h>

#include "src/index.h"
#include "PhonomeIndex/PhoIndexManager.h"
#include "json.hpp"
#include "faiss/IndexIVFPQ.h"
#include "src/HybridIndexManager.h"

using json = nlohmann::json;

// use stdio to pass json only.
/*
 * input: "handleQuery query string"
 *
 */

int main() {
    using namespace std;
    IndexAudioSumUnit = 2;
    ratio=2;
    AudioSum = 10;
    cout_flag=true;//辅助输出开关

    PhoIndexManager index(AudioSum);
//    IndexManager index(AudioSum);
    cout << "Finish" << endl;
//
//    string queryStr;
//    cin >> queryStr;
//    string filename = "/media/billy/New Volume/Billy/audio/" + queryStr + ".mfcc";
//    int fd = open(filename.c_str(), O_RDONLY);
//    vector<SimilarPhoneme> queryPho;
//    int n;
//    float buf[13];
//    while ((n=read(fd, (void *)buf, 13*4))>0) {
//        SimilarPhoneme term_tmp = Phoneme(buf);
//        queryPho.push_back(term_tmp);
//    }
//    cout << index.handleQuery(queryPho);

//    string path = "/media/billy/New Volume/Billy/phonome/";
//    initialInfo(path);
//    test_for_QandA(index,500,1,7000,1);
//
    FamilyTestQueryAndUpdatePho fam(&index,50,2);
    pthread_t pid;
    pthread_create(&pid,NULL,test_for_queryPhoThread,(void*)&fam);
    pthread_join(pid,NULL);
    test_for_index(10,AudioSum);

//    pthread_t pida;
//    FamilyAdd fama(&index,3000,1);
//    pthread_create(&pida,NULL,test_for_addThread,(void*)&fama);
//    pthread_join(pida,NULL);
//    while (true) {
//        int str_len;
//        {
//            char hex_str[9] = {0};
//            cin.read(hex_str, 8);
//            str_len = strtol(hex_str, nullptr, 16);
//        }
//        cout<<"Please input query: "<<endl;
//        string query_str;
//        cin >> query_str;
//        query_str.resize(str_len);
//        cin.read((char*) query_str.data(), str_len);
//        query_str[str_len] = 0;
//		cout << query_str << endl;
//         parse json string
//         https://github.com/nlohmann/json#implicit-conversions
//		auto in_obj = json::parse(query_str);
//		auto field1_int = in_obj["field1"].get<int>();
//		auto field3_str = in_obj["field3"];

//         create a json object
//         https://github.com/nlohmann/json#examples
//        string str_back = index.handleQuery(query_str);
//        cout<<str_back<<endl;
//        json out_obj = {
//                {"content", str_back},
//                {"link", 3},
//                {"other_field2", {1, 2, 3, 4}}
//        };

//        string out_str = out_obj.dump();

//        {
//            char hex_str[9] = {0};
//            sprintf(hex_str, "%08x", (int)out_str.size());
//            cout.write(hex_str, 8);
//        }

//        cout << out_str;
//    }

//    return 0;
//    HybridIndexManager index(AudioSum);
//    cout << "000This is end000" << endl;
//
//
//    while (true) {
//        long str_len;
//        {
//            char hex_str[9] = {0};
//            cin.read(hex_str, 8);
//            str_len = strtol(hex_str, nullptr, 16);
//        }
//        char pause[5] = "#!#";
//        char* pcha = new char[str_len+1];
//        cin.read(pcha, str_len);
//        pcha[str_len] = '\0';
//        ofstream file("test.txt");
//        file << pcha << endl;
//        file.close();
//        string queryStr(pcha);
//
//        char* tmp = new char[13*4];
//        char* mark = new char[5];
//        vector<SimilarPhoneme> queryPho;
//        while (true) {
//            cin.read(mark, 3);
////            cout << "Let's begin " << mark << endl;
//            if (!strncmp(mark, pause, 3)) {
//                break;
//            }
//            cin.read(tmp, 13*4);
//
//
//            SimilarPhoneme pho = SimilarPhoneme((float*)tmp);
//            queryPho.push_back(pho);
//        }
//
//        index.handleQuery(queryStr, queryPho);
//
//        cout << "000This is end000" << endl;
//
//        delete tmp;
//        delete mark;
//        delete pcha;
//    }
}
