#pragma once
#include "simdjson.h"

// {"retcode":0,"message":"OK","data":{"list":[{"id":2,"is_refresh":false},{"id":7,"is_refresh":false},{"id":9,"is_refresh":false}],"expire_time":1664607000}}

namespace MapList {
	struct List {
		int64_t id;
		bool is_refresh;
	};

	struct Data {
		std::vector<List> list{};
		int64_t expire_time = 0;
	};

	struct Response {
		int64_t retcode = 0;
        std::string message;
        Data data;
	};

    Response parse(const simdjson::padded_string &json) {
        Response response;
        simdjson::dom::parser parser;
        simdjson::dom::element doc = parser.parse(json);
        response.retcode = doc["retcode"];
        response.message = doc["message"];
        for (simdjson::dom::element list : doc["data"]["list"]) {
            List item{};
            item.id = list["id"];
            item.is_refresh = list["is_refresh"];
            response.data.list.push_back(item);
        }
        response.data.expire_time = doc["data"]["expire_time"];
        return response;
    }
}