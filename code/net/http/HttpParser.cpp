#include "HttpParser.h"

namespace hxk
{
const std::string HttpRequest::kEmpty;
const std::string HttpResponse::kEmpty;

HttpParser::HttpParser(llhttp_type type) : complete_(false), type_(type)
{
    llhttp_settings_init(&settings_);

    //request 
    settings_.on_url = &HttpParser::OnUrl;
    settings_.on_url_complete = &HttpParser::OnUrlComplete;
    
    //response
    settings_.on_status = &HttpParser::OnStatus;
    settings_.on_status_complete = &HttpParser::OnStatusComplete;

    //both
    settings_.on_message_begin = &HttpParser::OnMessageBegin;
    settings_.on_message_complete = &HttpParser::OnMessageComplete;

    settings_.on_header_field = &HttpParser::OnHeaderField;
    settings_.on_header_field_complete = &HttpParser::OnHeaderFieldComplete;

    settings_.on_header_value = &HttpParser::OnHeaderValue;
    settings_.on_header_value_complete = &HttpParser::OnHeaderValueComplete;

    settings_.on_headers_complete = &HttpParser::OnHeadersComplete;

    settings_.on_body = &HttpParser::OnBody;

    llhttp_init(&parser_, type, &settings_);    //利用配置文件和特定的类型初始化解析器
    parser_.data = this;
}

bool HttpParser::Execute(const std::string& data)
{
    return Execute(data.c_str(), data.size());
}

/// @brief 解析http
/// @param data 传入的http报文数据
/// @param len http报文数据的大小
/// @return 是否解析成功
bool HttpParser::Execute(const char* data, size_t len)
{
    if(!error_reason_.empty()) {    //error
        return false;
    }
    //解析完整或部分请求/响应，并在此过程中调用用户回调函数
    auto err = llhttp_execute(&parser_, data, len);
    if(err != HPE_OK) {
        error_reason_ = llhttp_get_error_reason(&parser_);
        return false;
    }
    return true;
}

void HttpParser::SetRequestHandler(HttpRequestHandler h)
{
    request_handler = std::move(h);
}

void HttpParser::SetResponseHandler(HttpResponseHandler h)
{
    response_handler = std::move(h);
}

void HttpParser::Reset()
{
    complete_ = false;
    //将已经初始化的解析器重置为启动状态，并保留现有解析器类型，回调设置，用户数据
    llhttp_reset(&parser_);

    request_.Reset();
    response_.Reset();

    key_.clear();
    value_.clear();
    error_reason_.clear();
}

int HttpParser::OnMessageBegin(llhttp_t* h)
{
    HttpParser* httpParser = (HttpParser*)h->data;
    httpParser->Reset();
    return 0;
}

int HttpParser::OnMessageComplete(llhttp_t* h)
{
    HttpParser* httpParser = (HttpParser*)h->data;
    httpParser->complete_ = true;
    if(httpParser->IsRequest()) {   //request
        httpParser->request_.SetMethod(llhttp_method_t(httpParser->parser_.method));
        if(httpParser->request_handler) {
            httpParser->request_handler(httpParser->request_);
        }
    }
    else {                          //response
        httpParser->response_.SetCode(httpParser->parser_.status_code);
        if(httpParser->response_handler) {
            httpParser->response_handler(httpParser->response_);
        }
    }
    return 0;
}   

int HttpParser::OnUrl(llhttp_t* h, const char* data, size_t len)
{
    HttpParser* httpParser = (HttpParser*)h->data;
    if(!httpParser->IsRequest()) {
        return -1;
    }
    httpParser->request_.AppendUrl(data, len);
    return 0;
}

int HttpParser::OnUrlComplete(llhttp_t* h)
{
    return 0;
}

int HttpParser::OnHeaderField(llhttp_t* h, const char* data, size_t len)
{
    if(len > 0) {
        HttpParser* httpParser = (HttpParser*)h->data;
        httpParser->key_.append(data, len);
    }
    return 0;
}

int HttpParser::OnHeaderFieldComplete(llhttp_t* h)
{
    return 0;
}

int HttpParser::OnHeaderValue(llhttp_t* h, const char* data, size_t len)
{
    if(len > 0) {
        HttpParser* httpParser = (HttpParser*)h->data;
        httpParser->value_.append(data, len);
    }
    return 0;
}

//删除字符串前面和后面的空格
inline std::string& Trim(std::string& s)
{
    if(s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

int HttpParser::OnHeaderValueComplete(llhttp_t* h)
{
    HttpParser* httpParser = (HttpParser*)h->data;
    
    Trim(httpParser->key_);
    if(httpParser->key_.empty()) {
        return HPE_INVALID_HEADER_TOKEN;
    }
    if(httpParser->IsRequest()) {
        httpParser->request_.SetHeader(httpParser->key_, httpParser->value_);
    }
    else {
        httpParser->response_.SetHeader(httpParser->key_, httpParser->value_);
    }

    httpParser->key_.clear();
    httpParser->value_.clear();
    return 0;
}

int HttpParser::OnHeadersComplete(llhttp_t* h)
{
    return 0;
}

int HttpParser::OnStatus(llhttp_t* h, const char* data, size_t len)
{
    if(len == 0) {
        return 0;
    }

    HttpParser* httpParser = (HttpParser*)h->data;
    if(httpParser->IsRequest()) {
        return -1;
    }
    httpParser->response_.AppendStatus(data, len);
    return 0;
}

int HttpParser::OnStatusComplete(llhttp_t* h)
{
    return 0;
}

int HttpParser::OnBody(llhttp_t* h, const char* data, size_t len)
{
    if(len == 0) {
        return 0;
    }

    HttpParser* httpParser = (HttpParser*)h->data;
    if(httpParser->IsRequest()) {
        httpParser->request_.AppendBody(data, len);
    }
    else {
        httpParser->response_.AppendBody(data, len);
    }
    return 0;
}

void HttpParser::ReInit()
{
    if(error_reason_.empty()) {
        return;
    }
    error_reason_.clear();
    llhttp_init(&parser_, type_, &settings_);
    parser_.data = this;
    Reset();
}

}