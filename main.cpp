#include <iostream>

#include "demoC++/index.h"

#include "json.hpp"

using json = nlohmann::json;

// use stdio to pass json only.
/*
 * input: "handleQuery query string"
 *
 */
int main() {
	using namespace std;

	IndexAll index;
    preprocess(index);

    while (true) {
		int str_len;
		{
			char hex_str[9] = {0};
			cin.read(hex_str, 8);
			str_len = strtol(hex_str, nullptr, 16);
		}

		string query_str;
//        cin >> query_str;
		query_str.resize(str_len);
		cin.read((char*) query_str.data(), str_len);
		query_str[str_len] = 0;
//		cout << query_str << endl;
		// parse json string
		// https://github.com/nlohmann/json#implicit-conversions
//		auto in_obj = json::parse(query_str);
//		auto field1_int = in_obj["field1"].get<int>();
//		auto field3_str = in_obj["field3"];

		// create a json object
		// https://github.com/nlohmann/json#examples
        string str_back = handleQuery(index, query_str);
        json out_obj = {
                {"content", str_back},
                {"link", 3},
                {"other_field2", {1, 2, 3, 4}}
        };

		string out_str = out_obj.dump();

		{
			char hex_str[9] = {0};
			sprintf(hex_str, "%08x", (int)out_str.size());
			cout.write(hex_str, 8);
		}

		cout << out_str;
	}

	return 0;
}
