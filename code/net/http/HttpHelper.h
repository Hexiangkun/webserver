#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include <string>

namespace hxk
{

struct Url
{
    std::string path;
    std::string query;
};

struct Url ParseUrl(const std::string& url);

std::string ParseQuery(const std::string& url, const std::string& key);

}


#endif