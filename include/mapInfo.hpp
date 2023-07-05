#pragma once
#include "simdjson.h"

// {"retcode":0,"message":"OK","data":{"info":{"id":2,"name":"Teyvat Interactive Map","parent_id":1,"depth":2,"detail":"{\"slices\":[[{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/e9df2d03fdc71af6641c9e5b909e04d8_2660054586494739343.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/c41795ed79f107691836b2ab13e7fe40_8049678478943455379.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/f92ef4f5ef38c04a3b47f6e48e48a413_679428724686901874.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/982b0a0100f48225f9ca0651bd37d0f2_1294895063725069057.png\"}],[{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/ff166a2ad91b93a0fc00da8d334d95b7_3865881864108399959.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/b3de4cafb74bb90f7f2a4c3d37481e4d_8879011006779968912.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/e9c57fdb5888832174ebaf59832ef36d_8662334368415995503.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/3defa753ad955339f66a79313ba00918_2022138876267985572.png\"}],[{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/051141cb5271b3469ba023ad559e0ef0_304508313144583238.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/363edf05ea347d46e93d715b19175028_2874828104697761864.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/456f6948bdd39dd384f4570315f752c0_3613551708345776383.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/ae42b4d6c10f2ace8dcca362d4790f66_3078108048730929477.png\"}],[{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/3ec6d4ea09c3edc3c23d5585c37c8a12_7415654720791965233.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/e576ee87ecfedf9273d152e93185c673_1181645534349293086.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/b46b7fc417518d9ec76a7eaf8b2a7358_2153194687813563407.png\"},{\"url\":\"https://act.hoyoverse.com/map_manage/20230316/c37bea1c130d96a5e026d754ad960ed4_1102562323004518427.png\"}]],\"origin\":[9450,4333],\"total_size\":[16384,16384],\"padding\":[511,2559]}","node_type":2,"children":[],"icon":"https://act.hoyoverse.com/map_manage/20230316/191a621844575ee5b8e44035834d051f_3328806497146477608.png","ch_ext":""}}}

namespace MapInfo{
    struct Slice {
        std::string url;
    };

    struct Detail {
        std::vector<std::vector<Slice>> slices;
        std::vector<int64_t> origin;
        std::vector<int64_t> total_size;
        std::vector<int64_t> padding;

    };
    
    struct Info {
        int64_t id = 0;
        std::string name;
        int64_t parent_id = 0;
        int64_t depth = 0;
        Detail detail;
        int64_t node_type = 0;
        std::vector<int64_t> children;
        std::string icon;
        std::string ch_ext;
    };

    struct Data {
        Info info;
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
        response.data.info.id = doc["data"]["info"]["id"];
        response.data.info.name = doc["data"]["info"]["name"];
        response.data.info.parent_id = doc["data"]["info"]["parent_id"];
        response.data.info.depth = doc["data"]["info"]["depth"];
        simdjson::dom::parser parser2;
		simdjson::dom::element detail = parser2.parse(std::string(doc["data"]["info"]["detail"]));
		for (simdjson::dom::element slices : detail["slices"]) {
            std::vector<Slice> sliceBuffer{};
            for (simdjson::dom::element slice : slices) {
                Slice item{};
                item.url = slice["url"];
                sliceBuffer.push_back(item);
            }
            response.data.info.detail.slices.push_back(sliceBuffer);
        }
        for (simdjson::dom::element origin : detail["origin"]) {
            response.data.info.detail.origin.push_back(origin);
        }
        for (simdjson::dom::element total_size : detail["total_size"]) {
            response.data.info.detail.total_size.push_back(total_size);
        }
        for (simdjson::dom::element padding : detail["padding"]) {
            response.data.info.detail.padding.push_back(padding);
        }
        response.data.info.node_type = doc["data"]["info"]["node_type"];
        for (simdjson::dom::element children : doc["data"]["info"]["children"]) {
            response.data.info.children.push_back(children);
        }
        response.data.info.icon = doc["data"]["info"]["icon"];
        response.data.info.ch_ext = doc["data"]["info"]["ch_ext"];
        return response;
    }
}