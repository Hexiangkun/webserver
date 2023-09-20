#include <iostream>
#include <gtest/gtest.h>
#include "code/net/http/HttpHelper.h"

using namespace hxk;

TEST(UrlParser, url_test) {
    std::string url("/cloud/reconnect?stream_id=xxx&sdp=yyy");
    auto result = ParseUrl(url);
    EXPECT_EQ(result.path, "/cloud/reconnect");
    EXPECT_EQ(result.query, "stream_id=xxx&sdp=yyy");

    std::string stream_id = ParseQuery(url, "stream_id");
    EXPECT_EQ(stream_id, "xxx");
    std::string sdp = ParseQuery(url, "sdp");
    EXPECT_EQ(sdp, "yyy");
    std::string empty = ParseQuery(url, "none");
    EXPECT_EQ(empty, "");

    url = "/cloud/reconnect";
    result = ParseUrl(url);
    EXPECT_EQ(result.path, "/cloud/reconnect");
    EXPECT_EQ(result.query, "");

    url = "/cloud/reconnect?";
    result = ParseUrl(url);
    EXPECT_EQ(result.path, "/cloud/reconnect");
    EXPECT_EQ(result.query, "");

    url = "";
    result = ParseUrl(url);
    EXPECT_EQ(result.path, "");
    EXPECT_EQ(result.query, "");
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}