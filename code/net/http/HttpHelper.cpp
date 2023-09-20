#include "HttpHelper.h"
#include <algorithm>

namespace hxk
{

struct Url ParseUrl(const std::string& url)
{
    struct Url res;
    if(url.empty()) {
        return res;
    }

    // const char* start = &url[0];
    // const char* end = start + url.size();
    // const char* query = std::find(start, end, '?');
    // if(query != end) {
    //     res.query = std::string(query + 1, end);
    // }
    // res.path = std::string(start, query);

    size_t pos = url.find("?");
    if(pos != std::string::npos) {
        res.query = url.substr(pos + 1);   
    }

    res.path = url.substr(0, pos);
    return res;
}

std::string ParseQuery(const std::string& url, const std::string& key)
{
    auto res = ParseUrl(url);
    const auto& query = res.query;
    size_t start = query.find(key + "=");
    if(start == std::string::npos){
        return "";
    }

    start += key.size() + 1; //skip "="
    if(start >= query.size()) {
        return "";
    }

    size_t end = query.find("&", start);    //from pos = start to find &
    if(end != std::string::npos) {
        return query.substr(start, end - start);
    }
    else {
        return query.substr(start);
    }
}

}