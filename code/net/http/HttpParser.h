#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "llhttp.h"
#include <string>
#include <unordered_map>
#include <functional>

#define CRLF "\r\n"

namespace hxk
{

class HttpRequest
{
public:
    HttpRequest() : method_(HTTP_GET) {}
    ~HttpRequest() = default;

    void Reset()
    {
        method_ = llhttp_method_t(-1);
        url_.clear();
        headers_.clear();
        body_.clear();
    }

    void Swap(HttpRequest& other)
    {
        std::swap(method_, other.method_);
        std::swap(url_, other.url_);
        std::swap(headers_, other.headers_);
        std::swap(body_, other.body_);
    }

    std::string Encode() const
    {
        std::string buf;
        buf.reserve(512);

        buf.append(MethodToString());
        buf.append(" ");
        buf.append(url_);
        buf.append(" HTTP/1.1" CRLF);

        for (const auto& kv : headers_) {
            buf.append(kv.first);
            buf.append(":");
            buf.append(kv.second);
            buf.append(CRLF);
        }
        
        buf.append(CRLF);

        if(!body_.empty()) {
            buf.append(body_);
        }

        return buf;
    }
    //method
    void SetMethod(llhttp_method method) { method_ = method; }
    llhttp_method GetMethod() const { return method_; }
    std::string MethodToString() const { return std::string(llhttp_method_name(method_)); }

    //url
    void SetUrl(const std::string& url) { url_ = url; }
    const std::string& GetUrl() const { return url_; }
    void AppendUrl(const std::string& url) { url_ += url; }
    void AppendUrl(const char* url, size_t len) { url_.append(url, len); }

    //header
    void SetHeader(const std::string& field, const std::string& value) { headers_.insert({field, value}); }
    void SetHeaders(const std::unordered_map<std::string, std::string>& headers) { headers_.insert(headers.begin(), headers.end()); }
    const std::string& GetHeader(const std::string& field) const { return headers_.count(field) ? headers_.at(field) : kEmpty; }
    const std::unordered_map<std::string, std::string>& GetHeaders() const { return headers_; }

    //body
    void SetBody(const std::string& body) { body_ = body; SetHeader("Content-Length", std::to_string(body_.size())); }
    void SetBody(const char* body, size_t len) { SetBody(std::string().assign(body, len)); }
    void AppendBody(const char* body, size_t len) { SetBody(body_ + std::string().assign(body, len)); }
    void AppendBody(const char* body) { SetBody(body_ + std::string(body)); }
    void AppendBody(const std::string& body) { SetBody(body_ + body); }
    const std::string& GetBody() const { return body_; }
    //content
    const std::string& GetContentType() const { return headers_.count("Content-Type") ? headers_.at("Content-Type") : kEmpty; }
    int GetContentLength() const { return headers_.count("Content-Length") ? std::stoi(headers_.at("Content-Length")) : 0; }


private:
    llhttp_method method_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;

    static const std::string kEmpty;
};


class HttpResponse
{
public:
    HttpResponse(): code_(200) {}
    ~HttpResponse() {}

    void Reset()
    {
        code_ = 200;
        status_.clear();
        headers_.clear();
        body_.clear();
    }

    void Swap(HttpResponse& other)
    {
        std::swap(code_, other.code_);
        std::swap(status_, other.status_);
        std::swap(headers_, other.headers_);
        std::swap(body_, other.body_);
    }

    std::string Encode() const
    {
        std::string buf;
        buf.reserve(512);

        buf.append("HTTP/1.1 ");
        buf.append(std::to_string(code_));
        buf.append(" ");
        buf.append(status_);
        buf.append(CRLF);

        for(const auto& kv : headers_) {
            buf.append(kv.first);
            buf.append(":");
            buf.append(kv.second);
            buf.append(CRLF);
        }

        buf.append(CRLF);

        if(!body_.empty()) {
            buf.append(body_);
        }

        return buf;
    }

    void SetCode(int code) { code_ = code; }
    int GetCode() const { return code_; }

    void SetStatus(const std::string& status) { status_ = status; }
    void AppendStatus(const std::string& status) { status_ += status; }
    void AppendStatus(const char* status, size_t len) { status_.append(status, len); }
    const std::string& GetStatus() const { return status_; }

    void SetHeader(const std::string& field, const std::string& value) { headers_.insert({field, value}); }
    void SetHeaders(const std::unordered_map<std::string, std::string>& headers) { headers_.insert(headers.begin(), headers.end()); }
    const std::string& GetHeader(const std::string& field) const { return headers_.count(field) ? headers_.at(field) : kEmpty; }
    const std::unordered_map<std::string, std::string>& GetHeaders() const { return headers_; }

    void SetBody(const std::string& body) { body_ = body; SetHeader("Content-Length", std::to_string(body.size())); }
    void SetBody(const char* body, size_t len) { SetBody(std::string().assign(body, len)); }
    void AppendBody(const char* body, size_t len) { SetBody(body_ + std::string().assign(body, len)); }
    void AppendBody(const std::string& body) { SetBody(body_ + body); }
    const std::string& GetBody() const { return body_; }

    const std::string& GetContentType() const { return headers_.count("Content-Type") ? headers_.at("Content-Type") : kEmpty; }
    int GetContentLength() const { return headers_.count("Content-Length") ? std::stoi(headers_.at("Content-Length")) : 0; }

private:
    int code_;
    std::string status_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;

    static const std::string kEmpty;
};

class HttpParser
{
public:
    using HttpRequestHandler = std::function<void(const HttpRequest&)>;
    using HttpResponseHandler = std::function<void(const HttpResponse&)>;

    explicit HttpParser(llhttp_type type);

    HttpParser(const HttpParser&) = delete;
    void operator=(const HttpParser&) = delete;

    bool Execute(const std::string& data);
    bool Execute(const char* data, size_t len);

    void SetRequestHandler(HttpRequestHandler h);
    void SetResponseHandler(HttpResponseHandler h);

    bool IsRequest() const { return type_ == HTTP_REQUEST; }
    bool IsComplete() const { return complete_; }

    const HttpRequest& GetRequest() const& { return request_; }
    const HttpResponse& GetResponse() const& { return response_; }
    const std::string& GetErrorReason() const { return error_reason_; }

    void ReInit();

private:
    void Reset();

    static int OnMessageBegin(llhttp_t* h);
    static int OnMessageComplete(llhttp_t* h);

    static int OnUrl(llhttp_t* h, const char* data, size_t len);

    static int OnHeaderField(llhttp_t* h, const char* data, size_t len);
    static int OnHeaderValue(llhttp_t* h, const char* data, size_t len);

    static int OnStatus(llhttp_t* h, const char* data, size_t len);
    static int OnBody(llhttp_t* h, const char* data, size_t len);

    static int OnUrlComplete(llhttp_t* h);
    static int OnStatusComplete(llhttp_t* h);

    static int OnHeaderFieldComplete(llhttp_t* h);
    static int OnHeaderValueComplete(llhttp_t* h);
    static int OnHeadersComplete(llhttp_t* h);
private:
    llhttp_t parser_;
    llhttp_settings_t settings_;
    bool complete_;

    const llhttp_type type_;    //request or response
    HttpRequest request_;
    HttpResponse response_;
    
    std::string key_;
    std::string value_;
    std::string error_reason_;

    HttpRequestHandler request_handler;
    HttpResponseHandler response_handler;
};


}


#endif



